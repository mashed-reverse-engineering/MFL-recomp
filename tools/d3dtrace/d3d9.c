/* d3d9.dll draw-trace proxy for MFL.exe under Wine.  Loads the real (Wine) d3d9, patches the vtables of IDirect3D9 / IDirect3DDevice9 /
 * texture / buffer objects in place (no wrapper objects: `this` pointers stay real) and logs state changes + one D line per draw call.
 * The log is a CHANGE stream (a line is written only when a shadow value changes); normalise.py expands it to one full record per draw.
 * Config: d3dtrace.cfg in the working directory of the game (App_Executables), lines key=value:
 *   out=<file>            log file (default d3dtrace.log)
 *   start_phase=<n>       log only while *(DWORD*)phase_addr >= n (0 = always)
 *   phase_addr=0x652f68   game race phase variable (g_race_phase)
 *   racef_addr=<hex>      race frame counter (harness race_frames); with stop_racef the log stops when it reaches that value
 *   stop_racef=<n>        log only while the race frame counter < n (0 = off)
 *   stop_phase=<n>        log only while the race phase < n (6 = countdown segment only)
 *   vclock=1 step_us=20 anchor_phase=3   virtual clock for the main thread (see below)
 *   vsnap=1|2|3          1: clock to the next second at a race-phase change; 2: + every frame starts on a 50 ms grid; 3: + 1/60 s grid and
 *                        main-thread Sleeps do not move the clock (every frame exactly one step: statehash runs of MFL_FIX_FIXED_STEP builds)
 *   max_frames=<n>        stop after n logged Presents (0 = unlimited; also stops statehash, which records only while logging is active)
 *   trace=0               no draw / state records (W13 hx7, statehash runs): the active window, PRESENT lines and statehash stay; default 1
 *   geom=1                hash vertex/index buffer contents at Unlock (default 1)
 *   alock=1               worker threads in lockstep with the main thread (V6, see alock_step; default off)
 *   vds=1                 virtual DirectSound play cursor (V8, see h_dsb_GetCurrentPosition; default off, use with alock=1)
 *   statehash=1 sh_delta=<hex> [sh_base= sh_end= sh_out= sh_poke=<frame>:<va>:<xor>]   N1 per-frame state pages (see statehash_frame)
 *   sh_seed=<A statehash.bin> sh_seed_k=<K> [sh_seed_from=<frame>] [sh_seed_range=<lo>:<hi>]... [sh_seed_skip=<file>]   mode B re-seed (see seed_frame)
 *   rec=1 | play=<file>   replay file v1: keyboard state per Present + the determinism settings (see rp_step); needs mkbd=
 *   pace=1                with vsnap=3: main-thread Sleeps really sleep (a person can play a recording run at about 60 fps); NOT tested
 * Everything printed is deterministic: ids are creation order, floats %.9g, no pointers, no times.
 * Build: make (i686-w64-mingw32-gcc). */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <d3d9.h>

#define C(T, i, ...) ((T)orig_dev[i])(__VA_ARGS__)
typedef HRESULT (WINAPI *PFN)();
typedef uint64_t u64;
typedef uint32_t u32;

static FILE *lg;
static CRITICAL_SECTION cs;
static int cfg_start_phase, cfg_stop_racef, cfg_max_frames, cfg_geom = 1, cfg_vclock = 1, cfg_step_us = 20, cfg_stop_phase, cfg_trace = 1;
static u32 cfg_phase_addr = 0x652f68, cfg_racef_addr;
static int active, nframes, ndraws, nunhooked;
static char cfg_rwtap[260], cfg_rwtap_out[260] = "rwtap.log"; static unsigned cfg_rwtap_delta;   /* rwtap=<funcs.txt> rwtap_delta=<hex>: load mfltap.dll (tools/rwtap) next to the exe, log RW calls */
static volatile LONGLONG vt_us;
static void *orig_dev[128], *orig_d3d[24], *orig_tex[24], *orig_vb[16], *orig_ib[16];

/* ---- object registry: pointer -> id (creation order per kind) --------------------------------------------------------------- */
enum { K_TEX = 1, K_VB, K_IB, K_VS, K_PS, K_DECL, K_SURF, K_UNK, K_LVL };
static const char *kn[] = { "?", "TEX", "VB", "IB", "VS", "PS", "DECL", "SURF", "UNK" };
typedef struct Obj { void *p; int kind, id, w, h, fmt, levels, size, lvl; u64 lh[16]; u64 h64; struct Obj *par; unsigned char *shadow; void *lk_ptr; UINT lk_off, lk_size; DWORD lk_fl; } Obj;
#define NOBJ (1 << 17)
static Obj objs[NOBJ];
static int nid[10];
static Obj *find(void *p, int create_kind) {
    if (!p) return NULL;
    u32 i = (u32)(((uintptr_t)p >> 3) * 2654435761u) & (NOBJ - 1);
    for (int n = 0; n < NOBJ; n++, i = (i + 1) & (NOBJ - 1)) {
        if (objs[i].p == p) { if (create_kind) { memset(&objs[i], 0, sizeof objs[i]); objs[i].p = p; objs[i].kind = create_kind; objs[i].id = nid[create_kind]++; } return &objs[i]; }
        if (!objs[i].p) { if (!create_kind) create_kind = K_UNK; memset(&objs[i], 0, sizeof objs[i]); objs[i].p = p; objs[i].kind = create_kind; objs[i].id = nid[create_kind]++; return &objs[i]; }
    }
    return NULL;
}
static int idof(void *p) { Obj *o = find(p, 0); return o ? o->id : -1; }

static u64 fnv(const void *d, size_t n, u64 h) { const unsigned char *p = d; while (n--) { h ^= *p++; h *= 1099511628211ull; } return h; }
#define FNV0 14695981039346656037ull

/* ---- shadow state ------------------------------------------------------------------------------------------------------------ */
static DWORD rs[256]; static unsigned char rs_set[256];
static DWORD tss[8][33]; static unsigned char tss_set[8][33];
static DWORD smp[16][14]; static unsigned char smp_set[16][14];
static int texid[16]; static unsigned char tex_set[16];
#define NXF 280
static D3DMATRIX xf[NXF]; static unsigned char xf_set[NXF];
static D3DMATERIAL9 mat; static int mat_set;
static D3DLIGHT9 lt[64]; static unsigned char lt_set[64]; static signed char lt_en[64] = {[0 ... 63] = -1};
static D3DVIEWPORT9 vp; static int vp_set;
static DWORD fvf; static int fvf_set;
static int decl = -2, decl_set;
static struct { int vb; UINT off, stride; int set; UINT freq; int fset; } st[16];
static int ib = -2, ib_set;
static int vs = -2, ps = -2, vs_set, ps_set;
static float vsc[256][4]; static unsigned char vsc_set[256];
static float psc[224][4]; static unsigned char psc_set[224];
static int vsci[16][4]; static unsigned char vsci_set[16];
static int vscb[16]; static signed char vscb_set[16];
static float clip[6][4]; static unsigned char clip_set[6];
static RECT scissor; static int scissor_set;
static int rt0 = -2, rt_set, zs = -2, zs_set;
static void shadow_reset(void) { memset(rs_set, 0, sizeof rs_set); memset(tss_set, 0, sizeof tss_set); memset(smp_set, 0, sizeof smp_set); memset(tex_set, 0, sizeof tex_set); memset(xf_set, 0, sizeof xf_set);
    mat_set = 0; memset(lt_set, 0, sizeof lt_set); memset(lt_en, -1, sizeof lt_en); vp_set = 0; fvf_set = 0; decl_set = 0; memset(st, 0, sizeof st); ib_set = 0; vs_set = ps_set = 0;
    memset(vsc_set, 0, sizeof vsc_set); memset(psc_set, 0, sizeof psc_set); memset(vsci_set, 0, sizeof vsci_set); memset(vscb_set, 0, sizeof vscb_set); memset(clip_set, 0, sizeof clip_set); scissor_set = 0; rt_set = zs_set = 0; }

#define L(...) fprintf(lg, __VA_ARGS__)
static void pf(float f) { L(" %.9g", (double)f); }
static void pmat(const D3DMATRIX *m) { const float *f = (const float *)m; for (int i = 0; i < 16; i++) pf(f[i]); }
static int xslot(DWORD s) { if (s == D3DTS_VIEW) return 0; if (s == D3DTS_PROJECTION) return 1; if (s >= D3DTS_TEXTURE0 && s <= D3DTS_TEXTURE7) return 2 + (s - D3DTS_TEXTURE0); if (s >= 256 && s < 260) return 10 + (s - 256); return -1; }
static DWORD xstate(int slot) { return slot == 0 ? D3DTS_VIEW : slot == 1 ? D3DTS_PROJECTION : slot < 10 ? D3DTS_TEXTURE0 + slot - 2 : 256 + slot - 10; }

static void e_rs(int i) { L("RS %d %x\n", i, rs[i]); }
static void e_tss(int s, int t) { L("TSS %d %d %x\n", s, t, tss[s][t]); }
static void e_smp(int s, int t) { L("SS %d %d %x\n", s, t, smp[s][t]); }
static void e_tex(int s) { L("TEX %d %d\n", s, texid[s]); }
static void e_xf(int k) { L("XF %u", xstate(k)); pmat(&xf[k]); L("\n"); }
static void e_mat(void) { L("MAT"); const float *f = (const float *)&mat; for (int i = 0; i < 17; i++) pf(f[i]); L("\n"); }
static void e_lt(int i) { const D3DLIGHT9 *l = &lt[i]; L("LIGHT %d %u", i, l->Type); const float *f = (const float *)&l->Diffuse; for (int k = 0; k < 12; k++) pf(f[k]); pf(l->Position.x); pf(l->Position.y); pf(l->Position.z); pf(l->Direction.x); pf(l->Direction.y); pf(l->Direction.z); pf(l->Range); pf(l->Falloff); pf(l->Attenuation0); pf(l->Attenuation1); pf(l->Attenuation2); pf(l->Theta); pf(l->Phi); L("\n"); }
static void e_vp(void) { L("VP %u %u %u %u", vp.X, vp.Y, vp.Width, vp.Height); pf(vp.MinZ); pf(vp.MaxZ); L("\n"); }
static void e_vsc(int r) { L("VSC %d", r); for (int k = 0; k < 4; k++) pf(vsc[r][k]); L("\n"); }
static void e_psc(int r) { L("PSC %d", r); for (int k = 0; k < 4; k++) pf(psc[r][k]); L("\n"); }
static void e_clip(int i) { L("CLIP %d", i); for (int k = 0; k < 4; k++) pf(clip[i][k]); L("\n"); }

/* full dump of the current shadow state: written when logging becomes active, so the first draw record is complete */
static void dump_state(void) {
    L("STATE\n");
    for (int i = 0; i < 256; i++) if (rs_set[i]) e_rs(i);
    for (int s = 0; s < 8; s++) for (int t = 0; t < 33; t++) if (tss_set[s][t]) e_tss(s, t);
    for (int s = 0; s < 16; s++) for (int t = 0; t < 14; t++) if (smp_set[s][t]) e_smp(s, t);
    for (int s = 0; s < 16; s++) if (tex_set[s]) e_tex(s);
    for (int k = 0; k < NXF; k++) if (xf_set[k]) e_xf(k);
    if (mat_set) e_mat();
    for (int i = 0; i < 64; i++) { if (lt_set[i]) e_lt(i); if (lt_en[i] >= 0) L("LEN %d %d\n", i, lt_en[i]); }
    if (vp_set) e_vp();
    if (fvf_set) L("FVF %x\n", fvf);
    if (decl_set) L("DECL %d\n", decl);
    for (int s = 0; s < 16; s++) { if (st[s].set) L("SRC %d %d %u %u\n", s, st[s].vb, st[s].off, st[s].stride); if (st[s].fset) L("FREQ %d %u\n", s, st[s].freq); }
    if (ib_set) L("IB %d\n", ib);
    if (vs_set) L("VS %d\n", vs);
    if (ps_set) L("PS %d\n", ps);
    for (int r = 0; r < 256; r++) if (vsc_set[r]) e_vsc(r);
    for (int r = 0; r < 224; r++) if (psc_set[r]) e_psc(r);
    for (int r = 0; r < 16; r++) { if (vsci_set[r]) L("VSI %d %d %d %d %d\n", r, vsci[r][0], vsci[r][1], vsci[r][2], vsci[r][3]); }
    for (int i = 0; i < 6; i++) if (clip_set[i]) e_clip(i);
    if (scissor_set) L("SCISSOR %ld %ld %ld %ld\n", scissor.left, scissor.top, scissor.right, scissor.bottom);
    if (rt_set) L("RT %d\n", rt0);
    if (zs_set) L("ZS %d\n", zs);
    L("ENDSTATE\n");
}

static void check_active(void) {
    int on = 1;
    DWORD ph = 0, rf = 0;
    if (cfg_start_phase || cfg_stop_racef || cfg_stop_phase) {
        if (!IsBadReadPtr((void *)(uintptr_t)cfg_phase_addr, 4)) ph = *(DWORD *)(uintptr_t)cfg_phase_addr;
        if (cfg_racef_addr && !IsBadReadPtr((void *)(uintptr_t)cfg_racef_addr, 4)) rf = *(DWORD *)(uintptr_t)cfg_racef_addr;
        if (cfg_start_phase && (int)ph < cfg_start_phase) on = 0;
        if (cfg_stop_phase && (int)ph >= cfg_stop_phase) on = 0;
        if (cfg_stop_racef && (int)ph >= 5 && (int)rf >= cfg_stop_racef) on = 0;
    }
    if (cfg_max_frames && nframes >= cfg_max_frames) on = 0;
    if (on && !active) { active = 1; if (cfg_trace) dump_state(); }
    else if (!on && active) { active = 0; L("INACTIVE\n"); fflush(lg); }
}

/* ---- device hooks ------------------------------------------------------------------------------------------------------------ */
#define ACT if (active && cfg_trace)
static HRESULT WINAPI h_SetRenderState(void *d, DWORD s, DWORD v) {
    if (s < 256 && (!rs_set[s] || rs[s] != v)) { rs[s] = v; rs_set[s] = 1; ACT e_rs(s); }
    return C(HRESULT (WINAPI *)(void *, DWORD, DWORD), 57, d, s, v);
}
static HRESULT WINAPI h_SetTSS(void *d, DWORD s, DWORD t, DWORD v) {
    if (s < 8 && t < 33 && (!tss_set[s][t] || tss[s][t] != v)) { tss[s][t] = v; tss_set[s][t] = 1; ACT e_tss(s, t); }
    return C(HRESULT (WINAPI *)(void *, DWORD, DWORD, DWORD), 67, d, s, t, v);
}
static HRESULT WINAPI h_SetSS(void *d, DWORD s, DWORD t, DWORD v) {
    if (s < 16 && t < 14 && (!smp_set[s][t] || smp[s][t] != v)) { smp[s][t] = v; smp_set[s][t] = 1; ACT e_smp(s, t); }
    return C(HRESULT (WINAPI *)(void *, DWORD, DWORD, DWORD), 69, d, s, t, v);
}
static HRESULT WINAPI h_SetTexture(void *d, DWORD s, void *t) {
    int id = t ? idof(t) : -1;
    if (s < 16 && (!tex_set[s] || texid[s] != id)) { texid[s] = id; tex_set[s] = 1; ACT e_tex(s); }
    return C(HRESULT (WINAPI *)(void *, DWORD, void *), 65, d, s, t);
}
static void set_xf(DWORD state, const D3DMATRIX *m) { int k = xslot(state); if (k < 0) return; if (!xf_set[k] || memcmp(&xf[k], m, sizeof *m)) { xf[k] = *m; xf_set[k] = 1; ACT e_xf(k); } }
static HRESULT WINAPI h_SetTransform(void *d, DWORD s, const D3DMATRIX *m) { set_xf(s, m); return C(HRESULT (WINAPI *)(void *, DWORD, const D3DMATRIX *), 44, d, s, m); }
static HRESULT WINAPI h_MultiplyTransform(void *d, DWORD s, const D3DMATRIX *m) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, DWORD, const D3DMATRIX *), 46, d, s, m);
    D3DMATRIX n; if (C(HRESULT (WINAPI *)(void *, DWORD, D3DMATRIX *), 45, d, s, &n) == 0) set_xf(s, &n);
    return r;
}
static HRESULT WINAPI h_SetMaterial(void *d, const D3DMATERIAL9 *m) { if (!mat_set || memcmp(&mat, m, sizeof mat)) { mat = *m; mat_set = 1; ACT e_mat(); } return C(HRESULT (WINAPI *)(void *, const D3DMATERIAL9 *), 49, d, m); }
static HRESULT WINAPI h_SetLight(void *d, DWORD i, const D3DLIGHT9 *l) { if (i < 64 && (!lt_set[i] || memcmp(&lt[i], l, sizeof *l))) { lt[i] = *l; lt_set[i] = 1; ACT e_lt(i); } return C(HRESULT (WINAPI *)(void *, DWORD, const D3DLIGHT9 *), 51, d, i, l); }
static HRESULT WINAPI h_LightEnable(void *d, DWORD i, BOOL e) { e = e ? 1 : 0; if (i < 64 && lt_en[i] != e) { lt_en[i] = e; ACT L("LEN %u %d\n", i, e); } return C(HRESULT (WINAPI *)(void *, DWORD, BOOL), 53, d, i, e); }
static HRESULT WINAPI h_SetViewport(void *d, const D3DVIEWPORT9 *v) { if (!vp_set || memcmp(&vp, v, sizeof vp)) { vp = *v; vp_set = 1; ACT e_vp(); } return C(HRESULT (WINAPI *)(void *, const D3DVIEWPORT9 *), 47, d, v); }
static HRESULT WINAPI h_SetFVF(void *d, DWORD f) { if (!fvf_set || fvf != f) { fvf = f; fvf_set = 1; ACT L("FVF %x\n", f); } return C(HRESULT (WINAPI *)(void *, DWORD), 89, d, f); }
static HRESULT WINAPI h_SetVertexDeclaration(void *d, void *p) { int id = p ? idof(p) : -1; if (!decl_set || decl != id) { decl = id; decl_set = 1; ACT L("DECL %d\n", id); } return C(HRESULT (WINAPI *)(void *, void *), 87, d, p); }
static Obj *st_o[16], *ib_o;
static HRESULT WINAPI h_SetStreamSource(void *d, UINT s, void *vb, UINT off, UINT stride) {
    if (s < 16) st_o[s] = vb ? find(vb, 0) : NULL;
    int id = vb ? idof(vb) : -1;
    if (s < 16 && (!st[s].set || st[s].vb != id || st[s].off != off || st[s].stride != stride)) { st[s].vb = id; st[s].off = off; st[s].stride = stride; st[s].set = 1; ACT L("SRC %u %d %u %u\n", s, id, off, stride); }
    return C(HRESULT (WINAPI *)(void *, UINT, void *, UINT, UINT), 100, d, s, vb, off, stride);
}
static HRESULT WINAPI h_SetStreamSourceFreq(void *d, UINT s, UINT f) { if (s < 16 && (!st[s].fset || st[s].freq != f)) { st[s].freq = f; st[s].fset = 1; ACT L("FREQ %u %u\n", s, f); } return C(HRESULT (WINAPI *)(void *, UINT, UINT), 102, d, s, f); }
static HRESULT WINAPI h_SetIndices(void *d, void *p) { ib_o = p ? find(p, 0) : NULL; int id = p ? idof(p) : -1; if (!ib_set || ib != id) { ib = id; ib_set = 1; ACT L("IB %d\n", id); } return C(HRESULT (WINAPI *)(void *, void *), 104, d, p); }
static HRESULT WINAPI h_SetVertexShader(void *d, void *p) { int id = p ? idof(p) : -1; if (!vs_set || vs != id) { vs = id; vs_set = 1; ACT L("VS %d\n", id); } return C(HRESULT (WINAPI *)(void *, void *), 92, d, p); }
static HRESULT WINAPI h_SetPixelShader(void *d, void *p) { int id = p ? idof(p) : -1; if (!ps_set || ps != id) { ps = id; ps_set = 1; ACT L("PS %d\n", id); } return C(HRESULT (WINAPI *)(void *, void *), 107, d, p); }
static HRESULT WINAPI h_SetVSCF(void *d, UINT r, const float *f, UINT n) {
    for (UINT i = 0; i < n && r + i < 256; i++) if (!vsc_set[r + i] || memcmp(vsc[r + i], f + 4 * i, 16)) { memcpy(vsc[r + i], f + 4 * i, 16); vsc_set[r + i] = 1; ACT e_vsc(r + i); }
    return C(HRESULT (WINAPI *)(void *, UINT, const float *, UINT), 94, d, r, f, n);
}
static HRESULT WINAPI h_SetPSCF(void *d, UINT r, const float *f, UINT n) {
    for (UINT i = 0; i < n && r + i < 224; i++) if (!psc_set[r + i] || memcmp(psc[r + i], f + 4 * i, 16)) { memcpy(psc[r + i], f + 4 * i, 16); psc_set[r + i] = 1; ACT e_psc(r + i); }
    return C(HRESULT (WINAPI *)(void *, UINT, const float *, UINT), 109, d, r, f, n);
}
static HRESULT WINAPI h_SetVSCI(void *d, UINT r, const int *f, UINT n) {
    for (UINT i = 0; i < n && r + i < 16; i++) { memcpy(vsci[r + i], f + 4 * i, 16); vsci_set[r + i] = 1; ACT L("VSI %u %d %d %d %d\n", r + i, f[4 * i], f[4 * i + 1], f[4 * i + 2], f[4 * i + 3]); }
    return C(HRESULT (WINAPI *)(void *, UINT, const int *, UINT), 96, d, r, f, n);
}
static HRESULT WINAPI h_SetVSCB(void *d, UINT r, const BOOL *f, UINT n) {
    for (UINT i = 0; i < n && r + i < 16; i++) ACT L("VSB %u %d\n", r + i, f[i] ? 1 : 0);
    return C(HRESULT (WINAPI *)(void *, UINT, const BOOL *, UINT), 98, d, r, f, n);
}
static HRESULT WINAPI h_SetClipPlane(void *d, DWORD i, const float *p) { if (i < 6 && (!clip_set[i] || memcmp(clip[i], p, 16))) { memcpy(clip[i], p, 16); clip_set[i] = 1; ACT e_clip(i); } return C(HRESULT (WINAPI *)(void *, DWORD, const float *), 55, d, i, p); }
static HRESULT WINAPI h_SetScissorRect(void *d, const RECT *r) { if (!scissor_set || memcmp(&scissor, r, sizeof *r)) { scissor = *r; scissor_set = 1; ACT L("SCISSOR %ld %ld %ld %ld\n", r->left, r->top, r->right, r->bottom); } return C(HRESULT (WINAPI *)(void *, const RECT *), 75, d, r); }
static HRESULT WINAPI h_SetRenderTarget(void *d, DWORD i, void *s) { if (i == 0) { int id = s ? idof(s) : -1; if (!rt_set || rt0 != id) { rt0 = id; rt_set = 1; ACT L("RT %d\n", id); } } return C(HRESULT (WINAPI *)(void *, DWORD, void *), 37, d, i, s); }
static HRESULT WINAPI h_SetDepthStencilSurface(void *d, void *s) { int id = s ? idof(s) : -1; if (!zs_set || zs != id) { zs = id; zs_set = 1; ACT L("ZS %d\n", id); } return C(HRESULT (WINAPI *)(void *, void *), 39, d, s); }
static HRESULT WINAPI h_Clear(void *d, DWORD n, const D3DRECT *r, DWORD f, D3DCOLOR c, float z, DWORD s) { ACT { L("CLEAR %u %u %x", n, f, c); pf(z); L(" %u\n", s); } return C(HRESULT (WINAPI *)(void *, DWORD, const D3DRECT *, DWORD, D3DCOLOR, float, DWORD), 43, d, n, r, f, c, z, s); }
static void game_vars(DWORD *ph, DWORD *rf) {
    *ph = *rf = 0;
    if (!IsBadReadPtr((void *)(uintptr_t)cfg_phase_addr, 4)) *ph = *(DWORD *)(uintptr_t)cfg_phase_addr;
    if (cfg_racef_addr && !IsBadReadPtr((void *)(uintptr_t)cfg_racef_addr, 4)) *rf = *(DWORD *)(uintptr_t)cfg_racef_addr;
}
static int cfg_anchor_phase = 3, anchored;     /* anchor_phase=n: when the race phase first reaches n the virtual clock jumps to the next multiple of 100 s: absolute times (seeds, camera animations) then agree between runs although the menu path took a different number of clock calls */
static HRESULT WINAPI h_BeginScene(void *d) {
    if (cfg_vclock && cfg_anchor_phase && !anchored) { DWORD ph, rf; game_vars(&ph, &rf); if ((int)ph >= cfg_anchor_phase && (int)ph < 100) { anchored = 1; vt_us = (vt_us / 100000000 + 1) * 100000000; L("ANCHOR %lld\n", (long long)vt_us); } }
    check_active(); if (active) { DWORD ph, rf; game_vars(&ph, &rf); L("SCENE %u %u\n", ph, rf); } return C(HRESULT (WINAPI *)(void *), 41, d); }
/* ---- N1 state hash (statehash=1): at every logged Present the game's writable image range [sh_base, sh_end) (+ sh_delta in the standalone
 * link) is compared page by page with a shadow copy; the changed 4 KB pages are appended to sh_out (default statehash.bin, read by statecmp.py),
 * frame 0 writes all pages.  Record: u32 "SHF1" frame phase racef npages nreg imgbase imgsize base delta, nreg x (base size) of the committed
 * private memory (heap ranges), npages x (u32 page index, 4096 bytes; the last page of the range is zero padded).
 * sh_poke=<frame>:<orig VA>:<xor>: after recording frame <frame> the dword at VA + sh_delta is xored (negative fixture: a perturbed car). */
static int cfg_sh, cfg_poke_frame = -1; static u32 cfg_sh_base = 0x5fb000, cfg_sh_end = 0x949d64, cfg_sh_delta, cfg_poke_va, cfg_poke_xor;
static char cfg_sh_out[260] = "statehash.bin";
static int is_main(void); static int cfg_vsnap;
static int cfg_alock, ato; static volatile LONG nth; static void alock_step(int settle);
#ifndef VDSSNAP_DEF
#define VDSSNAP_DEF 0
#endif
static int cfg_vdssnap = VDSSNAP_DEF; static volatile LONG nvds_snap; static void vds_snap(DWORD ph);   /* lane v2_w11 pin 1, see vds_snap */
static volatile LONG nrd, npend, nst_get, nst_play, nst_stop, nst_lock, nst_lockwc;static int cfg_vds, cfg_apoll, nvds_buf; static volatile LONG npres, nvds_read, nvds_step;     /* vds=1 (V8): Present count (all frames), cursor reads, fixed steps */
/* sh_seed=<A's statehash.bin> sh_seed_k=<K> [sh_seed_from=<frame>] [sh_seed_range=<lo>:<hi> ...] [sh_seed_skip=<file>] (RPL-T mode B, lane replay_w17):
 * every K frames (frame >= from, (frame - from) % K == 0), after this run's frame is recorded, the words of the seed ranges (original VAs; default
 * cars, vehicles, aibots = statecmp DENY_RANGES) are set to run A's values of the same frame.  Words listed in the skip file (first token of a
 * line = 8 hex digits: state_ptrwords.txt) are never written (pointers of A's heap / code layout).  So this run's record of a seed frame holds
 * the drift of one window and the next window starts from A's state.  A is read as a stream, one record per frame; a frame whose phase / racef
 * differ from A's is not seeded.  Heap contents are not in A's file: only image words can be seeded. */
#define NSR 64
static char cfg_seed[260], cfg_seed_skip[260]; static int cfg_seed_k, cfg_seed_from, nsr; static u32 sr[NSR][2];
static char cfg_rec[260], cfg_play[260]; static int cfg_pace;      /* replay file, see rp_step */
static void seed_frame(int fr, DWORD ph, DWORD rf) {
    static unsigned char *img, *skip; static int next; static u32 aph, arf; static long off;
    u32 n = cfg_sh_end - cfg_sh_base, np = (n + 4095) / 4096, hd[10], i;
    if (!cfg_seed[0] || cfg_seed_k <= 0) return;
    FILE *sf = fopen(cfg_seed, "rb");      /* opened and closed in every call: a handle held open would move the handle values the game stores later (state compare) */
    if (!img) {
        img = calloc(np, 4096); skip = calloc(np, 128);      /* skip: one bit per image word */
        FILE *k = cfg_seed_skip[0] && skip ? fopen(cfg_seed_skip, "r") : NULL; char ln[256]; int ns = 0;
        while (k && fgets(ln, sizeof ln, k)) { char *e; u32 va = strtoul(ln, &e, 16); if (e - ln == 8 && va >= cfg_sh_base && va < cfg_sh_end) { skip[(va - cfg_sh_base) >> 5] |= 1 << ((va - cfg_sh_base) >> 2 & 7); ns++; } }
        if (k) fclose(k);
        if (!nsr) { sr[0][0] = 0x8B06E0; sr[0][1] = 0x8B06E0 + 0xD040; sr[1][0] = 0x659f00; sr[1][1] = 0x659f00 + 0xAB0; sr[2][0] = 0x8c9ae0; sr[2][1] = 0x8c9ae0 + 0x1e0; nsr = 3; }
        L("SEED file=%s open=%d k=%d from=%d ranges=%d skipwords=%d\n", cfg_seed, sf != NULL, cfg_seed_k, cfg_seed_from, nsr, ns);
    }
    if (!sf || !img || !skip) { cfg_seed[0] = 0; if (sf) fclose(sf); return; }
    fseek(sf, off, SEEK_SET);
    while (next <= fr) {
        if (fread(hd, 4, 10, sf) != 10 || hd[0] != 0x31464853) { L("SEED A ends before frame %d\n", fr); cfg_seed[0] = 0; fclose(sf); return; }
        fseek(sf, 8 * hd[5], SEEK_CUR);
        for (u32 k = 0; k < hd[4]; k++) { if (fread(&i, 4, 1, sf) != 1) break; if (i < np) fread(img + i * 4096, 1, 4096, sf); else fseek(sf, 4096, SEEK_CUR); }
        next = hd[1] + 1; aph = hd[2]; arf = hd[3];
    }
    off = ftell(sf); fclose(sf);
    if (fr < cfg_seed_from || (fr - cfg_seed_from) % cfg_seed_k) return;
    if (aph != ph || arf != rf) { L("SEED frame=%d NOT APPLIED: A phase=%u racef=%u, here phase=%u racef=%u\n", fr, aph, arf, ph, rf); return; }
    int nw = 0, nk = 0;
    for (int r = 0; r < nsr; r++) for (u32 va = sr[r][0] & ~3u; va + 4 <= sr[r][1] && va + 4 <= cfg_sh_end; va += 4) {
        if (va < cfg_sh_base) continue;
        u32 o = va - cfg_sh_base, a; memcpy(&a, img + o, 4); DWORD *p = (DWORD *)(uintptr_t)(va + cfg_sh_delta);
        if (*p == a) continue;
        if (skip[o >> 5] >> (o >> 2 & 7) & 1) { nk++; continue; }
        *p = a; nw++;
    }
    L("SEED frame=%d phase=%u racef=%u written=%d ptrskip=%d\n", fr, ph, rf, nw, nk);
}
static void statehash_frame(int fr, DWORD ph, DWORD rf) {
    static unsigned char *shadow; static FILE *f; static u32 reg[2 * 4096], idx[4096];
    u32 n = cfg_sh_end - cfg_sh_base, np = (n + 4095) / 4096, nr = 0, nc = 0, hd[10];
    unsigned char *b = (unsigned char *)(uintptr_t)(cfg_sh_base + cfg_sh_delta), page[4096];
    if (!cfg_sh || np > 4096) return;
    if (!f) { f = fopen(cfg_sh_out, "wb"); shadow = calloc(np, 4096); if (!f || !shadow) { cfg_sh = 0; return; } setvbuf(f, NULL, _IOFBF, 1 << 22); }
    MEMORY_BASIC_INFORMATION mi;
    for (uintptr_t a = 0x10000; a < 0x7fff0000 && VirtualQuery((void *)a, &mi, sizeof mi); a = (uintptr_t)mi.BaseAddress + mi.RegionSize)
        if (mi.State == MEM_COMMIT && mi.Type == MEM_PRIVATE && nr < 4096) {
            if (nr && reg[2 * nr - 2] + reg[2 * nr - 1] == (u32)(uintptr_t)mi.BaseAddress) reg[2 * nr - 1] += mi.RegionSize;
            else { reg[2 * nr] = (u32)(uintptr_t)mi.BaseAddress; reg[2 * nr + 1] = mi.RegionSize; nr++; } }
    for (u32 i = 0; i < np; i++) {
        u32 len = n - i * 4096 < 4096 ? n - i * 4096 : 4096;
        memset(page, 0, sizeof page); memcpy(page, b + i * 4096, len);
        if (fr == 0 || memcmp(page, shadow + i * 4096, 4096)) { memcpy(shadow + i * 4096, page, 4096); idx[nc++] = i; }
    }
    unsigned char *m = (unsigned char *)GetModuleHandleA(NULL); IMAGE_NT_HEADERS *nt = (void *)(m + ((IMAGE_DOS_HEADER *)m)->e_lfanew);
    hd[0] = 0x31464853; hd[1] = fr; hd[2] = ph; hd[3] = rf; hd[4] = nc; hd[5] = nr; hd[6] = (u32)(uintptr_t)m; hd[7] = nt->OptionalHeader.SizeOfImage; hd[8] = cfg_sh_base; hd[9] = cfg_sh_delta;
    fwrite(hd, 4, 10, f); fwrite(reg, 8, nr, f);
    for (u32 k = 0; k < nc; k++) { fwrite(&idx[k], 4, 1, f); fwrite(shadow + idx[k] * 4096, 1, 4096, f); }
    fflush(f);
    if (fr == cfg_poke_frame) { DWORD *p = (DWORD *)(uintptr_t)(cfg_poke_va + cfg_sh_delta); DWORD old; VirtualProtect(p, 4, PAGE_READWRITE, &old); *p ^= cfg_poke_xor; L("POKE frame=%d va=%x xor=%x\n", fr, cfg_poke_va, cfg_poke_xor); }
    seed_frame(fr, ph, rf);
}
static void mstep_frame(void);
static HRESULT WINAPI h_Present(void *d, const RECT *a, const RECT *b, HWND w, const RGNDATA *c) {
    DWORD ph = 0, rf = 0;
    if (is_main()) mstep_frame();
    if (!IsBadReadPtr((void *)(uintptr_t)cfg_phase_addr, 4)) ph = *(DWORD *)(uintptr_t)cfg_phase_addr;
    if (cfg_racef_addr && !IsBadReadPtr((void *)(uintptr_t)cfg_racef_addr, 4)) rf = *(DWORD *)(uintptr_t)cfg_racef_addr;
    InterlockedIncrement(&npres);      /* before alock_step: the workers it releases see the new frame's cursor */
    { static DWORD rp_ph = 0xFFFFFFFF; if (lg && cfg_rec[0] && ph != rp_ph && is_main()) { rp_ph = ph; L("RP P %ld %u\n", (long)npres, (unsigned)ph); } }
    if (cfg_vds && cfg_vdssnap && is_main()) vds_snap(ph);
    if (cfg_vdssnap && active && nframes % 100 == 0) L("VDSSNAP snaps=%d\n", (int)nvds_snap);
    if (cfg_alock && is_main()) { alock_step(1); if (active && nframes % 100 == 0) L("ALOCK threads=%d timeouts=%d\n", (int)nth, ato); }
    if (cfg_vds && active && nframes % 100 == 0) L("VDS buffers=%d reads=%d steps=%d readfile=%d pending=%d getstatus=%d play=%d stop=%d lock=%d lockwc=%d\n", nvds_buf, (int)nvds_read, (int)nvds_step, (int)nrd, (int)npend,
        (int)nst_get, (int)nst_play, (int)nst_stop, (int)nst_lock, (int)nst_lockwc);
    if (active) statehash_frame(nframes, ph, rf);      /* before the real Present: the state the frame was drawn from */
    if (cfg_vclock && cfg_vsnap == 2 && is_main()) vt_us = (vt_us / 50000 + 1) * 50000;   /* vsnap=2: every frame starts on a 50 ms grid, a clock-call count that differs inside one frame does not carry over */
    if (cfg_vclock && cfg_vsnap == 3 && is_main()) vt_us = (vt_us / 16667 + 1) * 16667;   /* vsnap=3 (FPS W17, MFL_FIX_FIXED_STEP runs): a 1/60 s grid and main-thread Sleeps do not move the clock, so every frame lasts exactly one step */
    HRESULT r = C(HRESULT (WINAPI *)(void *, const RECT *, const RECT *, HWND, const RGNDATA *), 17, d, a, b, w, c);
    if (active) { L("PRESENT %d phase=%u racef=%u draws=%d\n", nframes++, ph, rf, ndraws); fflush(lg); }
    return r;
}
static HRESULT WINAPI h_Reset(void *d, void *pp) { L("RESET\n"); shadow_reset(); return C(HRESULT (WINAPI *)(void *, void *), 16, d, pp); }
static HRESULT WINAPI h_CreateStateBlock(void *d, DWORD t, void **o) { L("WARN CreateStateBlock (state blocks are not shadowed)\n"); return C(HRESULT (WINAPI *)(void *, DWORD, void **), 59, d, t, o); }
static HRESULT WINAPI h_BeginStateBlock(void *d) { L("WARN BeginStateBlock (state blocks are not shadowed)\n"); return C(HRESULT (WINAPI *)(void *), 60, d); }

static UINT vcount(D3DPRIMITIVETYPE t, UINT pc) { switch (t) { case D3DPT_POINTLIST: return pc; case D3DPT_LINELIST: return pc * 2; case D3DPT_LINESTRIP: return pc + 1; case D3DPT_TRIANGLELIST: return pc * 3; default: return pc + 2; } }
/* hash of the vertex bytes a draw reads (all bound streams) and of the index range: from the CPU shadow of the locked data, so it is comparable between implementations */
static u64 vhash(INT start, UINT n) {
    u64 h = FNV0;
    for (int s = 0; s < 16; s++) { Obj *o = st_o[s]; if (!st[s].set || !o || !o->shadow || !st[s].stride) continue;
        long b = (long)st[s].off + (long)start * st[s].stride; long len = (long)n * st[s].stride;
        if (b < 0 || b + len > o->size) { h = fnv("OOR", 3, h); continue; }
        h = fnv(&s, 4, h); h = fnv(o->shadow + b, len, h); }
    return h;
}
static u64 ihash(UINT si, UINT n) {
    Obj *o = ib_o; if (!o || !o->shadow) return 0; size_t isz = o->fmt == D3DFMT_INDEX32 ? 4 : 2;
    if ((size_t)(si + n) * isz > (size_t)o->size) return 1;
    return fnv(o->shadow + (size_t)si * isz, (size_t)n * isz, FNV0);
}
/* draws: one D line.  D <index> <kind> <prim> <args...> */
static int cfg_vdump;
static void vdump(UINT sv, UINT n) {   /* VDUMP=1 (cfg vdump=1): raw stream-0 vertices of each DP, for A/B debugging (not part of the normalised trace: line starts with "# VD") */
    if (!cfg_vdump) return;
    Obj *o = st_o[0]; if (!o || !o->shadow || !st[0].stride) return; long b = (long)st[0].off + (long)sv * st[0].stride, len = (long)n * st[0].stride; if (b < 0 || b + len > o->size) return;
    L("# VD %d", ndraws); for (long i = 0; i < len; i++) L("%s%02x", i % st[0].stride ? "" : " ", (unsigned char)o->shadow[b + i]); L("\n");
}
static HRESULT WINAPI h_DrawPrimitive(void *d, D3DPRIMITIVETYPE t, UINT sv, UINT pc) { ACT { vdump(sv, vcount(t, pc)); L("D %d DP %d %u %u vh=%016llx\n", ndraws, t, sv, pc, (unsigned long long)vhash(sv, vcount(t, pc))); ndraws++; } return C(HRESULT (WINAPI *)(void *, D3DPRIMITIVETYPE, UINT, UINT), 81, d, t, sv, pc); }
static HRESULT WINAPI h_DrawIndexedPrimitive(void *d, D3DPRIMITIVETYPE t, INT bv, UINT mi, UINT nv, UINT si, UINT pc) { ACT { L("D %d DIP %d %d %u %u %u %u vh=%016llx ih=%016llx\n", ndraws, t, bv, mi, nv, si, pc, (unsigned long long)vhash(bv + mi, nv), (unsigned long long)ihash(si, vcount(t, pc))); ndraws++; } return C(HRESULT (WINAPI *)(void *, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT), 82, d, t, bv, mi, nv, si, pc); }
static HRESULT WINAPI h_DrawPrimitiveUP(void *d, D3DPRIMITIVETYPE t, UINT pc, const void *data, UINT stride) {
    ACT { L("D %d DPUP %d %u %u %016llx\n", ndraws, t, pc, stride, (unsigned long long)fnv(data, (size_t)vcount(t, pc) * stride, FNV0)); ndraws++; }
    return C(HRESULT (WINAPI *)(void *, D3DPRIMITIVETYPE, UINT, const void *, UINT), 83, d, t, pc, data, stride);
}
static HRESULT WINAPI h_DrawIndexedPrimitiveUP(void *d, D3DPRIMITIVETYPE t, UINT mi, UINT nv, UINT pc, const void *idx, D3DFORMAT ifmt, const void *vd, UINT stride) {
    size_t isz = ifmt == D3DFMT_INDEX32 ? 4 : 2;
    ACT { L("D %d DIPUP %d %u %u %u %d %u %016llx %016llx\n", ndraws, t, mi, nv, pc, ifmt, stride, (unsigned long long)fnv(idx, (size_t)vcount(t, pc) * isz, FNV0), (unsigned long long)fnv((const char *)vd + (size_t)mi * stride, (size_t)nv * stride, FNV0)); ndraws++; }
    return C(HRESULT (WINAPI *)(void *, D3DPRIMITIVETYPE, UINT, UINT, UINT, const void *, D3DFORMAT, const void *, UINT), 84, d, t, mi, nv, pc, idx, ifmt, vd, stride);
}

/* ---- resource creation / content hashes ------------------------------------------------------------------------------------- */
static int pf_bits(int f, int *blk) { *blk = 0; switch (f) {
    case D3DFMT_A8R8G8B8: case D3DFMT_X8R8G8B8: case D3DFMT_A2B10G10R10: return 32;
    case D3DFMT_R5G6B5: case D3DFMT_X1R5G5B5: case D3DFMT_A1R5G5B5: case D3DFMT_A4R4G4B4: case D3DFMT_A8L8: case D3DFMT_V8U8: case D3DFMT_L16: return 16;
    case D3DFMT_A8: case D3DFMT_L8: case D3DFMT_P8: case D3DFMT_A4L4: return 8;
    case D3DFMT_R8G8B8: return 24;
    case D3DFMT_DXT1: *blk = 8; return 0; case D3DFMT_DXT2: case D3DFMT_DXT3: case D3DFMT_DXT4: case D3DFMT_DXT5: *blk = 16; return 0;
    default: return 32; } }
static void *vt_patch(void **vt, int i, void *hook, void **save) {
    if (vt[i] == hook) return NULL;
    DWORD old; if (!VirtualProtect(&vt[i], sizeof(void *), PAGE_EXECUTE_READWRITE, &old)) return NULL;
    *save = vt[i]; vt[i] = hook; VirtualProtect(&vt[i], sizeof(void *), old, &old); return hook;
}
static void hash_level(Obj *o, UINT lvl, const D3DLOCKED_RECT *lr) {
    int w = o->w >> lvl, h = o->h >> lvl; if (w < 1) w = 1; if (h < 1) h = 1;
    int blk, bits = pf_bits(o->fmt, &blk); size_t rowb; int rows;
    if (blk) { rowb = (size_t)((w + 3) / 4) * blk; rows = (h + 3) / 4; } else { rowb = (size_t)w * bits / 8; rows = h; }
    u64 hh = FNV0; for (int y = 0; y < rows; y++) hh = fnv((char *)lr->pBits + (size_t)y * lr->Pitch, rowb, hh);
    o->lh[lvl] = hh | 1; u64 all = 0; for (int i = 0; i < o->levels && i < 16; i++) all += (o->lh[i] ^ ((u64)(i + 1) * 0x9E3779B97F4A7C15ull)) * 0xff51afd7ed558ccdull;
    if (all != o->h64) { o->h64 = all; L("TEXH %d %016llx\n", o->id, (unsigned long long)all); }
}
typedef HRESULT (WINAPI *LOCKFN)(void *, ...);
/* texture data is written through IDirect3DTexture9::LockRect or through a level surface (GetSurfaceLevel + IDirect3DSurface9::LockRect): both hash the whole level after Unlock */
static HRESULT WINAPI h_tex_Unlock(void *t, UINT lvl) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, UINT))orig_tex[20])(t, lvl);
    Obj *o = find(t, 0);
    if (o && o->kind == K_TEX && lvl < 16 && (int)lvl < o->levels) {
        D3DLOCKED_RECT lr;
        if (((HRESULT (WINAPI *)(void *, UINT, D3DLOCKED_RECT *, const RECT *, DWORD))orig_tex[19])(t, lvl, &lr, NULL, D3DLOCK_READONLY) == 0) { hash_level(o, lvl, &lr); ((HRESULT (WINAPI *)(void *, UINT))orig_tex[20])(t, lvl); }
    }
    return r;
}
static void *orig_surf[24];
static HRESULT WINAPI h_surf_Unlock(void *s) {
    HRESULT r = ((HRESULT (WINAPI *)(void *))orig_surf[14])(s);
    Obj *o = find(s, 0);
    if (o && o->kind == K_LVL && o->par) {
        D3DLOCKED_RECT lr;
        if (((HRESULT (WINAPI *)(void *, D3DLOCKED_RECT *, const RECT *, DWORD))orig_surf[13])(s, &lr, NULL, D3DLOCK_READONLY) == 0) { hash_level(o->par, o->lvl, &lr); ((HRESULT (WINAPI *)(void *))orig_surf[14])(s); }
    }
    return r;
}
static HRESULT WINAPI h_tex_GetSurfaceLevel(void *t, UINT lvl, void **out) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, UINT, void **))orig_tex[18])(t, lvl, out);
    Obj *o = find(t, 0);
    if (r == 0 && out && *out && o && o->kind == K_TEX) {
        Obj *x = find(*out, K_LVL); x->par = o; x->lvl = lvl;
        void **vt = *(void ***)*out;
        if (!orig_surf[0]) { memcpy(orig_surf, vt, sizeof(void *) * 20); void *d; vt_patch(vt, 14, h_surf_Unlock, &orig_surf[14]); }
    }
    return r;
}
static HRESULT WINAPI h_UpdateSurface(void *d, void *src, const RECT *sr, void *dst, const POINT *dp) {
    Obj *a = find(src, 0), *b = find(dst, 0);
    HRESULT r = C(HRESULT (WINAPI *)(void *, void *, const RECT *, void *, const POINT *), 30, d, src, sr, dst, dp);
    if (a && b && a->kind == K_LVL && b->kind == K_LVL && a->par && b->par) { if (sr || dp) L("WARN UpdateSurface partial\n"); b->par->lh[b->lvl] = a->par->lh[a->lvl]; u64 all = 0; Obj *o = b->par; for (int i = 0; i < o->levels && i < 16; i++) all += (o->lh[i] ^ ((u64)(i + 1) * 0x9E3779B97F4A7C15ull)) * 0xff51afd7ed558ccdull; if (all != o->h64) { o->h64 = all; L("TEXH %d %016llx\n", o->id, (unsigned long long)all); } }
    return r;
}
static HRESULT WINAPI h_CreateTexture(void *d, UINT w, UINT h, UINT lv, DWORD us, D3DFORMAT f, D3DPOOL p, void **o, void *sh) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, void **, void *), 23, d, w, h, lv, us, f, p, o, sh);
    if (r == 0 && o && *o) {
        Obj *x = find(*o, K_TEX); x->w = w; x->h = h; x->fmt = f;
        void **vt = *(void ***)*o;
        if (!orig_tex[0]) { memcpy(orig_tex, vt, sizeof(void *) * 22); vt_patch(vt, 20, h_tex_Unlock, &orig_tex[20]); vt_patch(vt, 18, h_tex_GetSurfaceLevel, &orig_tex[18]); }
        x->levels = ((DWORD (WINAPI *)(void *))orig_tex[13])(*o); if (x->levels > 16) x->levels = 16;
        L("TEXDEF %d %u %u %d fmt=%u usage=%x pool=%d\n", x->id, w, h, x->levels, f, us, p);
    }
    return r;
}
static HRESULT WINAPI buf_Lock(void **orig, void *b, UINT off, UINT sz, void **pp, DWORD fl) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, UINT, UINT, void **, DWORD))orig[11])(b, off, sz, pp, fl); Obj *o = find(b, 0);
    if (r == 0 && o && pp) { o->lk_ptr = *pp; o->lk_off = off; o->lk_size = sz ? sz : o->size - off; o->lk_fl = fl; }
    return r;
}
static HRESULT WINAPI buf_Unlock(void **orig, void *b) {
    Obj *o = find(b, 0);
    if (o && o->shadow && o->lk_ptr && !(o->lk_fl & D3DLOCK_READONLY) && o->lk_off + o->lk_size <= (UINT)o->size) memcpy(o->shadow + o->lk_off, o->lk_ptr, o->lk_size);
    if (o) o->lk_ptr = NULL;
    return ((HRESULT (WINAPI *)(void *))orig[12])(b);
}
static HRESULT WINAPI h_vb_Lock(void *b, UINT off, UINT sz, void **pp, DWORD fl) { return buf_Lock(orig_vb, b, off, sz, pp, fl); }
static HRESULT WINAPI h_vb_Unlock(void *b) { return buf_Unlock(orig_vb, b); }
static HRESULT WINAPI h_ib_Lock(void *b, UINT off, UINT sz, void **pp, DWORD fl) { return buf_Lock(orig_ib, b, off, sz, pp, fl); }
static HRESULT WINAPI h_ib_Unlock(void *b) { return buf_Unlock(orig_ib, b); }
static void vtbl_buf_patch(void **vt, void **orig, void *lock, void *unlock) { if (!orig[0]) { memcpy(orig, vt, sizeof(void *) * 14); void *d; vt_patch(vt, 11, lock, &orig[11]); vt_patch(vt, 12, unlock, &orig[12]); } }
static HRESULT WINAPI h_CreateVertexBuffer(void *d, UINT len, DWORD us, DWORD fv, D3DPOOL p, void **o, void *sh) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, UINT, DWORD, DWORD, D3DPOOL, void **, void *), 26, d, len, us, fv, p, o, sh);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_VB); x->size = len; x->shadow = cfg_geom ? calloc(1, len) : NULL; vtbl_buf_patch(*(void ***)*o, orig_vb, h_vb_Lock, h_vb_Unlock); L("VBDEF %d %u usage=%x fvf=%x pool=%d\n", x->id, len, us, fv, p); }
    return r;
}
static HRESULT WINAPI h_CreateIndexBuffer(void *d, UINT len, DWORD us, D3DFORMAT f, D3DPOOL p, void **o, void *sh) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, UINT, DWORD, D3DFORMAT, D3DPOOL, void **, void *), 27, d, len, us, f, p, o, sh);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_IB); x->size = len; x->fmt = f; x->shadow = cfg_geom ? calloc(1, len) : NULL; vtbl_buf_patch(*(void ***)*o, orig_ib, h_ib_Lock, h_ib_Unlock); L("IBDEF %d %u usage=%x fmt=%d pool=%d\n", x->id, len, us, f, p); }
    return r;
}
static HRESULT WINAPI h_CreateRT(void *d, UINT w, UINT h, D3DFORMAT f, D3DMULTISAMPLE_TYPE m, DWORD q, BOOL l, void **o, void *sh) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, void **, void *), 28, d, w, h, f, m, q, l, o, sh);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_SURF); L("SURFDEF %d RT %u %u fmt=%u\n", x->id, w, h, f); } return r;
}
static HRESULT WINAPI h_CreateDS(void *d, UINT w, UINT h, D3DFORMAT f, D3DMULTISAMPLE_TYPE m, DWORD q, BOOL l, void **o, void *sh) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, UINT, UINT, D3DFORMAT, D3DMULTISAMPLE_TYPE, DWORD, BOOL, void **, void *), 29, d, w, h, f, m, q, l, o, sh);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_SURF); L("SURFDEF %d DS %u %u fmt=%u\n", x->id, w, h, f); } return r;
}
static HRESULT WINAPI h_UpdateTexture(void *d, void *s, void *t) { Obj *a = find(s, 0), *b = find(t, 0); if (a && b && a->h64 != b->h64) { b->h64 = a->h64; L("TEXH %d %016llx\n", b->id, (unsigned long long)b->h64); } return C(HRESULT (WINAPI *)(void *, void *, void *), 31, d, s, t); }
static u64 shader_hash(const DWORD *c, int *n) { int i = 0; u64 h = FNV0; for (;; i++) { h = fnv(&c[i], 4, h); if (c[i] == 0x0000FFFF) break; if (i > 4096) break; } *n = i + 1; return h; }
static HRESULT WINAPI h_CreateVS(void *d, const DWORD *c, void **o) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, const DWORD *, void **), 91, d, c, o);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_VS); int n; u64 h = shader_hash(c, &n); L("VSDEF %d %016llx %d ver=%x\n", x->id, (unsigned long long)h, n, c[0]); }
    return r;
}
static HRESULT WINAPI h_CreatePS(void *d, const DWORD *c, void **o) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, const DWORD *, void **), 106, d, c, o);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_PS); int n; u64 h = shader_hash(c, &n); L("PSDEF %d %016llx %d ver=%x\n", x->id, (unsigned long long)h, n, c[0]); }
    return r;
}
static HRESULT WINAPI h_CreateVD(void *d, const D3DVERTEXELEMENT9 *e, void **o) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, const D3DVERTEXELEMENT9 *, void **), 86, d, e, o);
    if (r == 0 && o && *o) { Obj *x = find(*o, K_DECL); L("DECLDEF %d", x->id); for (int i = 0; i < 32 && e[i].Stream != 0xFF; i++) L(" %u:%u:%u:%u:%u:%u", e[i].Stream, e[i].Offset, e[i].Type, e[i].Method, e[i].Usage, e[i].UsageIndex); L("\n"); }
    return r;
}

static void dump_caps(void *d3d, void *dev) {
    D3DCAPS9 c; memset(&c, 0, sizeof c);
    if (C(HRESULT (WINAPI *)(void *, D3DCAPS9 *), 7, dev, &c) != 0) return;
    D3DADAPTER_IDENTIFIER9 id; memset(&id, 0, sizeof id);
    if (((HRESULT (WINAPI *)(void *, UINT, DWORD, D3DADAPTER_IDENTIFIER9 *))orig_d3d[5])(d3d, 0, 0, &id) == 0) L("CAPS adapter=\"%s\" driver=\"%s\"\n", id.Description, id.Driver);
    L("CAPS DeviceType=%d Caps=%x Caps2=%x Caps3=%x PresentationIntervals=%x\n", c.DeviceType, c.Caps, c.Caps2, c.Caps3, c.PresentationIntervals);
    L("CAPS DevCaps=%x PrimitiveMiscCaps=%x RasterCaps=%x ZCmpCaps=%x SrcBlendCaps=%x DestBlendCaps=%x AlphaCmpCaps=%x\n", c.DevCaps, c.PrimitiveMiscCaps, c.RasterCaps, c.ZCmpCaps, c.SrcBlendCaps, c.DestBlendCaps, c.AlphaCmpCaps);
    L("CAPS ShadeCaps=%x TextureCaps=%x TextureFilterCaps=%x CubeTextureFilterCaps=%x VolumeTextureFilterCaps=%x TextureAddressCaps=%x LineCaps=%x\n", c.ShadeCaps, c.TextureCaps, c.TextureFilterCaps, c.CubeTextureFilterCaps, c.VolumeTextureFilterCaps, c.TextureAddressCaps, c.LineCaps);
    L("CAPS MaxTextureWidth=%u MaxTextureHeight=%u MaxVolumeExtent=%u MaxTextureRepeat=%u MaxTextureAspectRatio=%u MaxAnisotropy=%u\n", c.MaxTextureWidth, c.MaxTextureHeight, c.MaxVolumeExtent, c.MaxTextureRepeat, c.MaxTextureAspectRatio, c.MaxAnisotropy);
    L("CAPS StencilCaps=%x FVFCaps=%x TextureOpCaps=%x MaxTextureBlendStages=%u MaxSimultaneousTextures=%u VertexProcessingCaps=%x\n", c.StencilCaps, c.FVFCaps, c.TextureOpCaps, c.MaxTextureBlendStages, c.MaxSimultaneousTextures, c.VertexProcessingCaps);
    L("CAPS MaxActiveLights=%u MaxUserClipPlanes=%u MaxVertexBlendMatrices=%u MaxVertexBlendMatrixIndex=%u MaxPrimitiveCount=%u MaxVertexIndex=%u MaxStreams=%u MaxStreamStride=%u\n", c.MaxActiveLights, c.MaxUserClipPlanes, c.MaxVertexBlendMatrices, c.MaxVertexBlendMatrixIndex, c.MaxPrimitiveCount, c.MaxVertexIndex, c.MaxStreams, c.MaxStreamStride);
    L("CAPS VertexShaderVersion=%x PixelShaderVersion=%x MaxVertexShaderConst=%u NumSimultaneousRTs=%u PS20Caps=%x VS20Caps=%x\n", c.VertexShaderVersion, c.PixelShaderVersion, c.MaxVertexShaderConst, c.NumSimultaneousRTs, c.PS20Caps.Caps, c.VS20Caps.Caps);
    L("CAPS GuardBandLeft"); pf(c.GuardBandLeft); pf(c.GuardBandTop); pf(c.GuardBandRight); pf(c.GuardBandBottom); L(" MaxPointSize"); pf(c.MaxPointSize); L(" PixelShader1xMaxValue"); pf(c.PixelShader1xMaxValue); L(" ExtentsAdjust"); pf(c.ExtentsAdjust); L("\n");
}
static HRESULT WINAPI h_d3d_CheckDeviceFormat(void *d3d, UINT a, D3DDEVTYPE t, D3DFORMAT af, DWORD us, D3DRESOURCETYPE rt, D3DFORMAT cf) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, UINT, D3DDEVTYPE, D3DFORMAT, DWORD, D3DRESOURCETYPE, D3DFORMAT))orig_d3d[10])(d3d, a, t, af, us, rt, cf);
    L("CHECKFMT adapter=%u type=%d adapterfmt=%u usage=%x rtype=%d fmt=%u -> %s\n", a, t, af, us, rt, cf, r == 0 ? "ok" : "fail"); return r;
}
static HRESULT WINAPI h_d3d_CreateDevice(void *d3d, UINT a, D3DDEVTYPE t, HWND w, DWORD bf, D3DPRESENT_PARAMETERS *pp, void **o) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS *, void **))orig_d3d[16])(d3d, a, t, w, bf, pp, o);
    L("CREATEDEVICE adapter=%u type=%d behavior=%x bb=%ux%u fmt=%u count=%u swap=%u windowed=%d autods=%d dsfmt=%u flags=%x hz=%u interval=%x -> %s\n", a, t, bf, pp->BackBufferWidth, pp->BackBufferHeight, pp->BackBufferFormat, pp->BackBufferCount, pp->SwapEffect, pp->Windowed, pp->EnableAutoDepthStencil, pp->AutoDepthStencilFormat, pp->Flags, pp->FullScreen_RefreshRateInHz, pp->PresentationInterval, r == 0 ? "ok" : "fail");
    if (r == 0 && o && *o) {
        void **vt = *(void ***)*o;
        if (!orig_dev[0]) memcpy(orig_dev, vt, sizeof(void *) * 110);
        struct { int i; void *h; } hk[] = {
            {57, h_SetRenderState}, {67, h_SetTSS}, {69, h_SetSS}, {65, h_SetTexture}, {44, h_SetTransform}, {46, h_MultiplyTransform}, {49, h_SetMaterial}, {51, h_SetLight},
            {53, h_LightEnable}, {47, h_SetViewport}, {89, h_SetFVF}, {87, h_SetVertexDeclaration}, {100, h_SetStreamSource}, {102, h_SetStreamSourceFreq}, {104, h_SetIndices},
            {92, h_SetVertexShader}, {107, h_SetPixelShader}, {94, h_SetVSCF}, {109, h_SetPSCF}, {96, h_SetVSCI}, {98, h_SetVSCB}, {55, h_SetClipPlane}, {75, h_SetScissorRect},
            {37, h_SetRenderTarget}, {39, h_SetDepthStencilSurface}, {43, h_Clear}, {41, h_BeginScene}, {17, h_Present}, {16, h_Reset}, {59, h_CreateStateBlock}, {60, h_BeginStateBlock},
            {81, h_DrawPrimitive}, {82, h_DrawIndexedPrimitive}, {83, h_DrawPrimitiveUP}, {84, h_DrawIndexedPrimitiveUP}, {23, h_CreateTexture}, {26, h_CreateVertexBuffer},
            {27, h_CreateIndexBuffer}, {28, h_CreateRT}, {29, h_CreateDS}, {31, h_UpdateTexture}, {30, h_UpdateSurface}, {91, h_CreateVS}, {106, h_CreatePS}, {86, h_CreateVD} };
        for (unsigned i = 0; i < sizeof hk / sizeof *hk; i++) { void *dummy; if (vt[hk[i].i] != hk[i].h) vt_patch(vt, hk[i].i, hk[i].h, &dummy); }
        dump_caps(d3d, *o);
        fflush(lg);
    }
    return r;
}


/* ---- virtual clock: the game is frame locked but seeds and animates from the clock; with vclock=1 the MAIN thread sees a clock that advances only by
 * its own calls (step_us per time call) and by its Sleep()s, so two runs of the same script see the same times.  Other threads see the unmodified value. */
static DWORD main_tid; static LONGLONG qpf;
static int is_main(void) { return GetCurrentThreadId() == main_tid; }
/* vsnap=1 (N1): when the main thread reads the clock and the race phase changed since its last read, the clock jumps to the next whole second:
 * times stored after a phase change (race start ticks, countdown camera) do not depend on how many clock calls the loading code made before it */
/* pin=<phase>:<orig va>:<value> (repeatable, max NPIN): at the same moment (first main-thread clock read after the phase changed to <phase>) the dword at
 * va + sh_delta is set to value: counters that ran during the menus (their count depends on real-time key timing) start the race equal in every run */
/* srand=<phase>:<va>:<seed>: at that moment the game's own CRT srand (absolute va: 0x5d365e in MFL.exe, `_srand` of the link map) is called on the
 * main thread: the CRT rand() state lives in the per-thread data (heap), menus consume it frame by frame, loading uses it (course setup) */
static DWORD cfg_srand_ph, cfg_srand_va, cfg_srand_seed;
#define NPIN 8192
static int npin; static DWORD vs_phase = 0xFFFFFFFF; static struct { DWORD ph, va, v; } pins[NPIN];
static LONGLONG vnow(void) {
    if (!is_main()) return vt_us;
    if ((cfg_vsnap || npin || cfg_srand_va) &&!IsBadReadPtr((void *)(uintptr_t)cfg_phase_addr, 4)) { DWORD ph = *(DWORD *)(uintptr_t)cfg_phase_addr; if (ph != vs_phase) {
        vs_phase = ph; if (cfg_vsnap) vt_us = (vt_us / 1000000 + 1) * 1000000;
        for (int i = 0; i < npin; i++) if (pins[i].ph == ph) *(DWORD *)(uintptr_t)(pins[i].va + cfg_sh_delta) = pins[i].v;
        if (cfg_srand_va && ph == cfg_srand_ph) ((void (__cdecl *)(unsigned))(uintptr_t)cfg_srand_va)(cfg_srand_seed); } }
    return vt_us += cfg_step_us;
}
static BOOL WINAPI v_QPC(LARGE_INTEGER *c) { c->QuadPart = vnow() * qpf / 1000000; return TRUE; }
static DWORD WINAPI v_GetTickCount(void) { return (DWORD)(vnow() / 1000) + 100000; }
static DWORD WINAPI v_timeGetTime(void) { return (DWORD)(vnow() / 1000) + 100000; }
static void WINAPI v_GetSystemTimeAsFileTime(FILETIME *ft) { ULONGLONG t = 132000000000000000ull + (ULONGLONG)vnow() * 10; ft->dwLowDateTime = (DWORD)t; ft->dwHighDateTime = (DWORD)(t >> 32); }
/* alock=1 (V6, harness only, default off): the other threads of the exe (RWA audio / stream workers) run in lockstep with the main thread.
 * A non-main Sleep, or a timed wait that would block, parks the thread until the main thread releases it: at each Present and at each main-thread
 * Sleep / wait, the parked threads are released one at a time (registration order) and the main thread waits until each one parks again
 * (500 ms cap, counted in ato).  Infinite or wait-all waits count as parked while they block.  Wine's own threads (dsound mixer) are not covered. */
#define NTH 32
static int cfg_alock, ato; static CRITICAL_SECTION acs;
static struct { DWORD tid; volatile LONG st; HANDLE ev; } th[NTH];     /* st: 0 running, 1 parked (released by main), 2 blocked in a real wait */
static volatile LONG nth;
static int th_slot(void) {
    DWORD t = GetCurrentThreadId(); int i;
    for (i = 0; i < nth; i++) if (th[i].tid == t) return i;
    EnterCriticalSection(&acs); i = nth; if (i < NTH) { th[i].ev = CreateEventA(NULL, FALSE, FALSE, NULL); th[i].tid = t; nth = i + 1; } LeaveCriticalSection(&acs);
    return i < NTH ? i : -1;
}
static void alock_step(int settle) {
    for (int i = 0; i < nth; i++) {
        if (InterlockedCompareExchange(&th[i].st, 0, 1) != 1) continue;
        SetEvent(th[i].ev); DWORD t0 = GetTickCount();
        while (th[i].st == 0) { if (GetTickCount() - t0 > 500) { ato++; if (lg) { L("ALOCK TIMEOUT frame=%ld thread=%d\n", (long)npres, i); fflush(lg); } break; } Sleep(0); }   /* W13 hx7: every event, active or not (statecmp / scen_judge: UNJUDGED) */
    }
    if (settle) { DWORD t0 = GetTickCount(); for (int i = 0; i < nth; i++) while (th[i].st == 0 && GetTickCount() - t0 < 50) Sleep(0); }   /* threads woken by another one */
}
static DWORD await(DWORD n, const HANDLE *hs, BOOL all, DWORD ms) {    /* non-main thread, alock */
    int i = th_slot(); DWORD r = n ? WaitForMultipleObjects(n, hs, all, 0) : WAIT_TIMEOUT;
    if (i < 0) { if (!n) { Sleep(ms); return 0; } return r != WAIT_TIMEOUT ? r : WaitForMultipleObjects(n, hs, all, ms); }   /* registry full: unchanged */
    if (r != WAIT_TIMEOUT || (n && ms == 0)) return r;
    if (cfg_apoll) {    /* apoll=1 (lane v_w10): never block in the kernel; the wait is re-checked only when main releases the thread, so a wake-up happens at a step point, not when the signal lands */
        for (;;) {
            th[i].st = 1; WaitForSingleObject(th[i].ev, INFINITE); th[i].st = 0;
            r = n ? WaitForMultipleObjects(n, hs, all, 0) : WAIT_TIMEOUT;
            if (r != WAIT_TIMEOUT || ms != INFINITE) return r;
        }
    }
    if (all || ms == INFINITE || n >= MAXIMUM_WAIT_OBJECTS) { th[i].st = 2; r = WaitForMultipleObjects(n, hs, all, ms); th[i].st = 0; return r; }
    HANDLE h2[MAXIMUM_WAIT_OBJECTS]; memcpy(h2, hs, n * sizeof *hs); h2[n] = th[i].ev;
    th[i].st = 1; r = WaitForMultipleObjects(n + 1, h2, FALSE, INFINITE);
    if (r != WAIT_OBJECT_0 + n && InterlockedCompareExchange(&th[i].st, 0, 1) != 1) WaitForSingleObject(th[i].ev, INFINITE);   /* main released it meanwhile: eat the event */
    th[i].st = 0;
    return r == WAIT_OBJECT_0 + n ? WAIT_TIMEOUT : r;
}
static DWORD mwait(DWORD n, const HANDLE *hs, BOOL all, DWORD ms) {    /* main thread, alock: the workers may be what it waits for */
    DWORD r = WaitForMultipleObjects(n, hs, all, 0);
    if (r != WAIT_TIMEOUT || ms == 0) return r;
    alock_step(0);
    if (ms != INFINITE) return WaitForMultipleObjects(n, hs, all, ms);
    while ((r = WaitForMultipleObjects(n, hs, all, 20)) == WAIT_TIMEOUT) alock_step(0);
    return r;
}
static DWORD WINAPI v_WaitForMultipleObjects(DWORD n, const HANDLE *hs, BOOL all, DWORD ms) { return !cfg_alock ? WaitForMultipleObjects(n, hs, all, ms) : is_main() ? mwait(n, hs, all, ms) : await(n, hs, all, ms); }
static DWORD WINAPI v_WaitForSingleObject(HANDLE h, DWORD ms) { return !cfg_alock ? WaitForSingleObject(h, ms) : v_WaitForMultipleObjects(1, &h, FALSE, ms); }
static void WINAPI v_Sleep(DWORD ms) { if (is_main()) { if (cfg_vsnap != 3) vt_us += (LONGLONG)ms * 1000; if (cfg_alock) alock_step(0); Sleep(cfg_pace ? ms : 0); } else if (cfg_alock) await(0, NULL, FALSE, ms); else Sleep(ms); }
/* vds=1 (V8, harness only, default off, use with alock=1): Wine's dsound mixer advances the play cursor in real time, so how far the RWA thread
 * streams per frame (voice queue, then everything downstream) differs run to run.  With vds the exe's DirectSoundCreate / DirectSoundCreate8
 * (DSOUND IAT, ordinal 1 / 11 or by name) are wrapped, IDirectSound::CreateSoundBuffer patches the buffer vtables and
 * IDirectSoundBuffer::GetCurrentPosition returns a virtual cursor: play = (Presents since the buffer's first read) x nAvgBytesPerSec / 60
 * + VDS_STEP_DIV-th of a second for every read with no Present since the previous read (a poll loop while loading cannot hang; per call, so
 * deterministic), plus SetCurrentPosition's offset, block aligned, mod buffer size; write = play + the real write lead at the first read.
 * Wine still mixes the real buffer (audio output unchanged apart from where the game writes). */
#define VDS_STEP_DIV 600
#define NDSB 128
typedef struct { WORD tag, ch; DWORD sps, bps; WORD align, bits, cb; } VWFX;
static struct { void **vt; void *o[24]; } dvt[16]; static int ndvt;
static struct { void *b; DWORD size, bps, align, lead; LONG base, last; ULONGLONG extra, pos0; } dsb[NDSB];
static void *orig_dsc, *orig_dsc8;
static void **dvt_orig(void *obj) { void **vt = *(void ***)obj; for (int k = 0; k < ndvt; k++) if (dvt[k].vt == vt) return dvt[k].o; return NULL; }
static void dvt_hook(void *obj, int i, void *fn) {     /* patch slot i of obj's vtable once; dvt_orig(obj)[i] = the original */
    EnterCriticalSection(&acs); void **vt = *(void ***)obj; int k;
    for (k = 0; k < ndvt && dvt[k].vt != vt; k++) ;
    if (k == ndvt && k < 16) { dvt[k].vt = vt; memcpy(dvt[k].o, vt, 6 * sizeof(void *)); ndvt++; }
    if (k < ndvt && vt[i] != fn) vt_patch(vt, i, fn, &dvt[k].o[i]);
    LeaveCriticalSection(&acs);
}
/* ---- framed menu input (HARN W15, cfg mkbd= + mstep= lines from race_script.sh FRAMED=1): the menu path is driven from here, frame by frame, instead of
 * by xdotool keys in wall time, so the number of game frames (and with vsnap=2 the game clock) from process start to loading is the same in every run.
 * mkbd=<va of the game's DirectInput keyboard device pointer> (g_app_770f70): its GetDeviceState(256) gets the injected key's 0x80 bit.
 * mstep=k:<dik>:<va>:<=|!><value>  press <dik> (MS_HOLD frames) until the dword at va equals / differs (re-press after MS_RETRY frames, MS_TRIES max);
 *                                   no press when it already holds after MS_SETTLE frames
 * mstep=w:0:<va>:<=|!><value>       wait (no key) until it holds
 * mstep=f:<count>:<va>:<value>      fill count dwords at va with value (the course unlock of race_script.sh), at that point of the path
 * Every step is keyed to game state read at Present; a step logs "MSTEP i op frame=<Present count> vt=<virtual clock us> tries=n". */
#define NMS 64
#define MS_SETTLE 2
#define MS_HOLD 2
#define MS_RETRY 20
#define MS_TRIES 40
static struct { char op, cmp; int a; DWORD va, v; } ms[NMS]; static int nms, ms_i, ms_st, ms_t, ms_tries; static DWORD cfg_mkbd; static volatile DWORD ms_key; static void *ms_dev;
/* rec=1 / play=<file> (replay file v1, lane replay_w17): the keyboard state the game reads (DirectInput GetDeviceState(256) of the mkbd
 * device), keyed by the Present count since process start.  Text file: "MFLREPLAY 1"; "C <cfg line>" = the determinism settings of the recording
 * run (clock, lockstep, sound pins, srand phase and seed, pin rows), read as cfg lines by a play run; "P <frame> <phase>" = race phase changes
 * (information); "K <frame> [<dik hex> ...]" = the keys down from that frame on (at most 15).  rec writes a K line whenever the set changes
 * (injected mstep keys included); play replaces the device state by the file's.  No file is held open in the game: rec writes its lines into
 * the proxy log with the prefix "RP " (run.sh REPLAY_REC cuts them out), play loads the file while the cfg is read.  One more open handle moves
 * the handle values the game stores (9 words of the state compare, measured).  Keyboard only: pads / mouse are not recorded (ponytail: add a
 * device tag to the K line when a pad recording is needed). */
#define NRK 32768
static struct { long fr; BYTE n, k[15]; } rk[NRK]; static int nrk;
static int cfg_aio, cfg_vst;
static void rp_step(BYTE *buf) {
    static BYTE keys[256]; static int cur, started; int i, ch = 0;
    if (cfg_play[0]) {
        for (; cur < nrk && rk[cur].fr <= npres; cur++) { memset(keys, 0, 256); for (i = 0; i < rk[cur].n; i++) keys[rk[cur].k[i]] = 0x80; }
        memcpy(buf, keys, 256); return;
    }
    if (!lg) return;
    if (!started) {
        started = ch = 1;
        L("RP MFLREPLAY 1\nRP C vclock=%d\nRP C step_us=%d\nRP C vsnap=%d\nRP C anchor_phase=%d\nRP C alock=%d\nRP C vds=%d\nRP C vdssnap=%d\nRP C apoll=%d\nRP C aio=%d\nRP C vst=%d\n", cfg_vclock, cfg_step_us, cfg_vsnap, cfg_anchor_phase, cfg_alock, cfg_vds, cfg_vdssnap, cfg_apoll, cfg_aio, cfg_vst);
        if (cfg_srand_va) L("RP C srand=%u:0:%u\n", (unsigned)cfg_srand_ph, (unsigned)cfg_srand_seed);      /* va 0: the play run's own cfg names its _srand */
        for (i = 0; i < npin; i++) L("RP C pin=%u:%x:%x\n", (unsigned)pins[i].ph, (unsigned)pins[i].va, (unsigned)pins[i].v);
    }
    for (i = 0; i < 256; i++) ch |= (buf[i] ^ keys[i]) & 0x80;
    if (!ch) return;
    L("RP K %ld", (long)npres);
    for (i = 0; i < 256; i++) { keys[i] = buf[i] & 0x80; if (keys[i]) L(" %x", i); }
    L("\n"); fflush(lg);
}
static HRESULT WINAPI h_di_GetDeviceState(void *dev, DWORD n, void *buf) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, DWORD, void *))dvt_orig(dev)[9])(dev, n, buf);
    if (dev == ms_dev && n == 256 && ms_key) { if (r < 0) { memset(buf, 0, 256); r = 0; } ((BYTE *)buf)[ms_key] |= 0x80; }
    if (dev == ms_dev && n == 256 && cfg_play[0]) { r = 0; rp_step(buf); }
    else if (dev == ms_dev && n == 256 && cfg_rec[0] && r >= 0) rp_step(buf);
    return r;
}
static void mstep_frame(void) {
    if (!ms_dev && cfg_mkbd && !IsBadReadPtr((void *)(uintptr_t)cfg_mkbd, 4) && *(void **)(uintptr_t)cfg_mkbd) { ms_dev = *(void **)(uintptr_t)cfg_mkbd; dvt_hook(ms_dev, 9, h_di_GetDeviceState); L("MSTEP kbd %p\n", ms_dev); }
    if (ms_i >= nms) return;
    int i = ms_i, met = 0; DWORD *p = (DWORD *)(uintptr_t)ms[i].va;
    if (ms[i].op == 'f') { for (int k = 0; k < ms[i].a; k++) p[k] = ms[i].v; met = 1; }
    else if (!IsBadReadPtr(p, 4)) met = ms[i].cmp == '=' ? *p == ms[i].v : *p != ms[i].v;
    if (ms[i].op == 'k' && !ms_dev) return;      /* no keyboard yet: nothing can be pressed */
    if (ms_st == 0 && ms[i].op == 'k' && ++ms_t < MS_SETTLE) return;
    if (ms_st == 1) { if (++ms_t >= MS_HOLD) { ms_key = 0; ms_st = 2; ms_t = 0; } return; }
    if (met) { L("MSTEP %d %c frame=%d vt=%lld tries=%d\n", i, ms[i].op, (int)npres, (long long)vt_us, ms_tries); fflush(lg); ms_i++; ms_st = ms_t = ms_tries = 0; return; }
    if (ms[i].op != 'k' || (ms_st == 2 && ++ms_t < MS_RETRY)) return;
    if (++ms_tries > MS_TRIES) { L("MSTEP %d FAIL frame=%d\n", i, (int)npres); fflush(lg); ms_i = nms; ms_key = 0; return; }
    ms_key = (DWORD)ms[i].a; ms_st = 1; ms_t = 0;
}
static int dsb_slot(void *b, int fresh) {    /* the buffer's cursor state, set up at its first read; fresh: just (re)created at this address, forget it */
    EnterCriticalSection(&acs); int i, f = -1;
    for (i = 0; i < NDSB; i++) { if (dsb[i].b == b) break; if (!dsb[i].b && f < 0) f = i; }
    if (fresh) { if (i < NDSB) dsb[i].b = NULL; LeaveCriticalSection(&acs); return -1; }
    if (i == NDSB) i = f;
    if (i >= 0 && dsb[i].b != b) {
        void **o = dvt_orig(b); DWORD caps[5] = {sizeof caps}, w = 0, p = 0, wr = 0; VWFX fx = {0};
        ((HRESULT (WINAPI *)(void *, DWORD *))o[3])(b, caps);
        ((HRESULT (WINAPI *)(void *, VWFX *, DWORD, DWORD *))o[5])(b, &fx, sizeof fx, &w);
        ((HRESULT (WINAPI *)(void *, DWORD *, DWORD *))o[4])(b, &p, &wr);
        memset(&dsb[i], 0, sizeof dsb[i]); dsb[i].b = b; dsb[i].size = caps[2] ? caps[2] : 1; dsb[i].bps = fx.bps; dsb[i].align = fx.align ? fx.align : 1;
        dsb[i].lead = caps[2] ? (wr + caps[2] - p) % caps[2] : 0; dsb[i].base = npres; dsb[i].last = -1; nvds_buf++;
        if (lg) L("VDS buffer %d size=%u bps=%u align=%u lead=%u\n", i, dsb[i].size, dsb[i].bps, dsb[i].align, dsb[i].lead);
    }
    LeaveCriticalSection(&acs); return i;
}
static HRESULT WINAPI h_dsb_GetCurrentPosition(void *b, DWORD *play, DWORD *write) {
    int i = dsb_slot(b, 0);
    if (i < 0) return ((HRESULT (WINAPI *)(void *, DWORD *, DWORD *))dvt_orig(b)[4])(b, play, write);
    LONG n = npres; InterlockedIncrement(&nvds_read);
    if (dsb[i].last == n) { dsb[i].extra += dsb[i].bps / VDS_STEP_DIV; InterlockedIncrement(&nvds_step); }
    dsb[i].last = n;
    ULONGLONG a = dsb[i].pos0 + (ULONGLONG)(n - dsb[i].base) * dsb[i].bps / 60 + dsb[i].extra;
    DWORD p = (DWORD)(a % dsb[i].size); p -= p % dsb[i].align;
    if (play) *play = p;
    if (write) *write = (p + dsb[i].lead) % dsb[i].size;
    return 0;
}
/* vdssnap=1 (lane v2_w11 pin 1): called by the main thread's Present with the race phase; on a phase change every set-up virtual cursor
 * moves forward to the next whole buffer, counted from this Present, so the cursor's phase against the RWA mixer's 20 ms block grid no longer
 * depends on the number of menu / loading frames before it. */
/* vds=1, ours (ASM W11, TS Q4): the linked exe has no DirectSound; its audio device is recomp/audio/mfl_audio_null.c, a thread that pulls the RWA mixer
 * once per period of QueryPerformanceCounter time, fetched with GetProcAddress(kernel32) past the import table (so the game's vclock does not speed it up).
 * With vds the exe's GetProcAddress hands out dev_QPC instead for that name: the device clock is (Presents since its first read) x 1/60 s + 1/VDS_STEP_DIV s
 * per read without a Present between (a poll loop cannot hang), the same rule as the virtual DirectSound cursor; vdssnap moves it to the next whole 160 ms
 * (= the original's 2-period buffer) counted from its first value at every race-phase change. */
static struct { int init; LONG base, last; LONGLONG v0, pos0, extra; } dev;
static LONGLONG dev_now(LONG n) { return dev.pos0 + (LONGLONG)(n - dev.base) * qpf / 60 + dev.extra; }
static BOOL WINAPI dev_QPC(LARGE_INTEGER *c) {
    EnterCriticalSection(&acs); LONG n = npres;
    if (!dev.init) { dev.init = 1; dev.base = n; dev.last = -1; dev.pos0 = dev.v0 = qpf * 1000; if (lg) L("VDS device clock: ours (mfl_audio_null via GetProcAddress)\n"); }
    if (dev.last == n) { dev.extra += qpf / VDS_STEP_DIV; InterlockedIncrement(&nvds_step); }
    dev.last = n; InterlockedIncrement(&nvds_read);
    c->QuadPart = dev_now(n); LeaveCriticalSection(&acs); return TRUE;
}
static FARPROC WINAPI v_GetProcAddress(HMODULE m, LPCSTR name) {
    if (cfg_vds && (uintptr_t)name > 0xffff && !strcmp(name, "QueryPerformanceCounter")) return (FARPROC)dev_QPC;
    return GetProcAddress(m, name);
}
static void vds_snap(DWORD ph) {
    static DWORD last_ph = 0xFFFFFFFF;
    if (ph == last_ph) return;
    last_ph = ph;
    EnterCriticalSection(&acs); LONG n = npres;
    if (dev.init) { LONGLONG g = qpf * 160 / 1000, a = dev_now(n); dev.pos0 = dev.v0 + ((a - dev.v0) / g + 1) * g; dev.base = n; dev.extra = 0; dev.last = -1; InterlockedIncrement(&nvds_snap); }
    for (int i = 0; i < NDSB; i++) if (dsb[i].b) {
        ULONGLONG a = dsb[i].pos0 + (ULONGLONG)(n - dsb[i].base) * dsb[i].bps / 60 + dsb[i].extra;
        dsb[i].pos0 = (a / dsb[i].size + 1) * dsb[i].size; dsb[i].base = n; dsb[i].extra = 0; dsb[i].last = -1; InterlockedIncrement(&nvds_snap);
    }
    LeaveCriticalSection(&acs);
}
static HRESULT WINAPI h_dsb_SetCurrentPosition(void *b, DWORD pos) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, DWORD))dvt_orig(b)[13])(b, pos);
    int i = dsb_slot(b, 0);
    if (r == 0 && i >= 0) { dsb[i].pos0 = pos; dsb[i].base = npres; dsb[i].extra = 0; }
    return r;
}
/* vst=1 (lane v_w10 attempt 3, default 0, with vds): IDirectSoundBuffer::GetStatus answers from the game's own Play / Stop calls
 * (PLAYING, + LOOPING for DSBPLAY_LOOPING; never BUFFERLOST), not from Wine's mixer state; counters for Play / Stop / Lock / GetStatus */
#ifndef VST_DEF
#define VST_DEF 0
#endif
static int cfg_vst = VST_DEF;
static struct { void *b; int play, loop; } vst[NDSB];
static int vst_of(void *b, int fresh) {
    EnterCriticalSection(&acs); int i, f = -1;
    for (i = 0; i < NDSB; i++) { if (vst[i].b == b) break; if (!vst[i].b && f < 0) f = i; }
    if (i == NDSB) i = f;
    if (i >= 0 && (fresh || vst[i].b != b)) { vst[i].b = b; vst[i].play = vst[i].loop = 0; }
    LeaveCriticalSection(&acs); return i;
}
static HRESULT WINAPI h_dsb_GetStatus(void *b, DWORD *st) {
    int i = vst_of(b, 0); InterlockedIncrement(&nst_get);
    if (i < 0 || !st) return ((HRESULT (WINAPI *)(void *, DWORD *))dvt_orig(b)[9])(b, st);
    *st = vst[i].play ? (1 | (vst[i].loop ? 4 : 0)) : 0;     /* DSBSTATUS_PLAYING / LOOPING */
    return 0;
}
static HRESULT WINAPI h_dsb_Play(void *b, DWORD r1, DWORD prio, DWORD fl) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, DWORD, DWORD, DWORD))dvt_orig(b)[12])(b, r1, prio, fl);
    int i = vst_of(b, 0); InterlockedIncrement(&nst_play);
    if (r == 0 && i >= 0) { vst[i].play = 1; vst[i].loop = fl & 1; }
    return r;
}
static HRESULT WINAPI h_dsb_Stop(void *b) {
    HRESULT r = ((HRESULT (WINAPI *)(void *))dvt_orig(b)[18])(b);
    int i = vst_of(b, 0); InterlockedIncrement(&nst_stop);
    if (r == 0 && i >= 0) vst[i].play = 0;
    return r;
}
static HRESULT WINAPI h_dsb_Lock(void *b, DWORD ofs, DWORD n, void **p1, DWORD *n1, void **p2, DWORD *n2, DWORD fl) {
    InterlockedIncrement(&nst_lock);
    if (fl & 1) { DWORD pw = 0; h_dsb_GetCurrentPosition(b, NULL, &pw); ofs = pw; fl &= ~1u; InterlockedIncrement(&nst_lockwc); }  /* DSBLOCK_FROMWRITECURSOR: the virtual write cursor */
    return ((HRESULT (WINAPI *)(void *, DWORD, DWORD, void **, DWORD *, void **, DWORD *, DWORD))dvt_orig(b)[11])(b, ofs, n, p1, n1, p2, n2, fl);
}
static HRESULT WINAPI h_ds_CreateSoundBuffer(void *ds, const void *desc, void **o, void *outer) {
    HRESULT r = ((HRESULT (WINAPI *)(void *, const void *, void **, void *))dvt_orig(ds)[3])(ds, desc, o, outer);
    if (r == 0 && o && *o) {
        dvt_hook(*o, 4, h_dsb_GetCurrentPosition); dvt_hook(*o, 13, h_dsb_SetCurrentPosition); dsb_slot(*o, 1);
        if (cfg_vst) { dvt_hook(*o, 9, h_dsb_GetStatus); dvt_hook(*o, 12, h_dsb_Play); dvt_hook(*o, 18, h_dsb_Stop); dvt_hook(*o, 11, h_dsb_Lock); vst_of(*o, 1); }
    }
    return r;
}
static HRESULT WINAPI v_DirectSoundCreate(const GUID *g, void **o, void *outer) {
    HRESULT r = ((HRESULT (WINAPI *)(const GUID *, void **, void *))orig_dsc)(g, o, outer);
    if (r == 0 && o && *o) dvt_hook(*o, 3, h_ds_CreateSoundBuffer);
    return r;
}
static HRESULT WINAPI v_DirectSoundCreate8(const GUID *g, void **o, void *outer) {
    HRESULT r = ((HRESULT (WINAPI *)(const GUID *, void **, void *))orig_dsc8)(g, o, outer);
    if (r == 0 && o && *o) dvt_hook(*o, 3, h_ds_CreateSoundBuffer);
    return r;
}
/* aio=1 (lane v_w10 attempt 2, default 0): an overlapped ReadFile that goes pending is waited for here, then still reported as pending
 * (FALSE + ERROR_IO_PENDING): the game's own path is unchanged, but the completion (event, Internal) is in place before the caller looks at it,
 * so HasOverlappedIoCompleted polls and timed waits no longer depend on when Wine finishes the read */
#ifndef AIO_DEF
#define AIO_DEF 0
#endif
static int cfg_aio = AIO_DEF;
static BOOL WINAPI v_ReadFile(HANDLE h, void *buf, DWORD n, DWORD *got, OVERLAPPED *ov) {
    BOOL r = ReadFile(h, buf, n, got, ov); InterlockedIncrement(&nrd);
    if (!r && ov && cfg_aio && GetLastError() == ERROR_IO_PENDING) {
        InterlockedIncrement(&npend);
        while (ov->Internal == 0x103) Sleep(0);      /* STATUS_PENDING */
        if (ov->hEvent) { WaitForSingleObject(ov->hEvent, INFINITE); SetEvent(ov->hEvent); }   /* event in place; re-set so an auto-reset event is not eaten */
        SetLastError(ERROR_IO_PENDING);
    }
    return r;
}
static void iat_set(IMAGE_THUNK_DATA *ft, void *fn, void **save) {
    DWORD old; VirtualProtect(&ft->u1.Function, 4, PAGE_READWRITE, &old); if (save) *save = (void *)(uintptr_t)ft->u1.Function; ft->u1.Function = (DWORD)(uintptr_t)fn; VirtualProtect(&ft->u1.Function, 4, old, &old);
}
static void patch_iat(HMODULE m) {
    unsigned char *b = (unsigned char *)m; IMAGE_DOS_HEADER *dh = (void *)b; IMAGE_NT_HEADERS *nt = (void *)(b + dh->e_lfanew);
    IMAGE_DATA_DIRECTORY *dd = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    struct { const char *name; void *fn; } hk[] = { {"QueryPerformanceCounter", v_QPC}, {"GetTickCount", v_GetTickCount}, {"timeGetTime", v_timeGetTime}, {"GetSystemTimeAsFileTime", v_GetSystemTimeAsFileTime}, {"Sleep", v_Sleep},
                                                    {"WaitForSingleObject", v_WaitForSingleObject}, {"WaitForMultipleObjects", v_WaitForMultipleObjects}, {"ReadFile", v_ReadFile}, {"GetProcAddress", v_GetProcAddress} };
    for (IMAGE_IMPORT_DESCRIPTOR *d = (void *)(b + dd->VirtualAddress); dd->VirtualAddress && d->Name; d++) {
        IMAGE_THUNK_DATA *ot = (void *)(b + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk)), *ft = (void *)(b + d->FirstThunk);
        int ds = cfg_vds && !_stricmp((const char *)(b + d->Name), "dsound.dll");
        for (; ot->u1.AddressOfData; ot++, ft++) {
            if (ot->u1.Ordinal & 0x80000000) {
                if (ds && (ot->u1.Ordinal & 0xffff) == 1) iat_set(ft, v_DirectSoundCreate, &orig_dsc);
                if (ds && (ot->u1.Ordinal & 0xffff) == 11) iat_set(ft, v_DirectSoundCreate8, &orig_dsc8);
                continue;
            }
            const char *n = (const char *)((IMAGE_IMPORT_BY_NAME *)(b + ot->u1.AddressOfData))->Name;
            if (ds && !strcmp(n, "DirectSoundCreate")) iat_set(ft, v_DirectSoundCreate, &orig_dsc);
            if (ds && !strcmp(n, "DirectSoundCreate8")) iat_set(ft, v_DirectSoundCreate8, &orig_dsc8);
            for (unsigned i = 0; i < sizeof hk / sizeof *hk; i++) if (!strcmp(n, hk[i].name)) iat_set(ft, hk[i].fn, NULL);
        }
    }
}

/* ---- audiorng=1 (TEST ONLY, control runs only, default off; output/determinism_2026-11-16.md): the audio/gameplay random split in the original.
 * Every `audiorng_site=<va>:<target>` row is a `call rel32` in MFL.exe's sound code to random_range (0x481fd0) or rp_random_int_range (0x4c45c0)
 * (agents/rep_w12/audiosites.py -> tools/statehash/data/audio_rng_sites.txt).  In memory only, the call is pointed at a thunk that swaps
 * RpRandom's state (plugin data at [0x7d2128] + [0x7da6b8] = {table, ptrA, ptrB, end}, table of 31 words) with the sound stream, runs the
 * ORIGINAL wrapper, and swaps back.  The sound stream is recomp/tier1/mfl_audio_rng.h, the same code ours uses under
 * MFL_FIX_DETERMINISTIC_AUDIO_RNG, so the split control and the flagged ours draw the same numbers.  MFL.exe on disk is never changed. */
#include "../../recomp/tier1/mfl_audio_rng.h"
#define NARS 256
static int cfg_audiorng, n_ars; static DWORD ars_va[NARS], ars_to[NARS];
static MflAudioRng ar_sound, ar_game;
DWORD ar_to_rr = 0x481fd0, ar_to_ri = 0x4c45c0;
static DWORD *ar_data(void) { return (DWORD *)(uintptr_t)(*(DWORD *)0x7d2128 + *(DWORD *)0x7da6b8); }
static void ar_save(MflAudioRng *g, DWORD *d) { memcpy(g->t, (void *)(uintptr_t)d[0], sizeof g->t); g->a = (int)(d[1] - d[0]) / 4; g->b = (int)(d[2] - d[0]) / 4; }
static void ar_load(const MflAudioRng *g, DWORD *d) { memcpy((void *)(uintptr_t)d[0], g->t, sizeof g->t); d[1] = d[0] + 4 * g->a; d[2] = d[0] + 4 * g->b; }
void ar_in(void) { DWORD *d = ar_data(); if (!ar_sound.seeded) mfl_audio_rng_seed(&ar_sound, MFL_AUDIO_RNG_SEED); ar_save(&ar_game, d); ar_load(&ar_sound, d); }
void ar_out(void) { DWORD *d = ar_data(); ar_save(&ar_sound, d); ar_load(&ar_game, d); }
/* ar_in / ar_out are integer code: the wrapper's float result stays in st(0); eax / edx are kept around ar_out.  No global slot: both wrappers
   take exactly two dword arguments (cdecl), so the thunk pushes copies of them and calls the original; its own return address stays on the stack.
   All 102 sites are reached from the main loop only (agents/rep_w12/callroots.py: no caller in the RWA thread code; roots entry and the
   course_storm_cb1 callback), as the shared RpRandom state itself requires. */
#define AR_THUNK(name, target) __asm__(".globl _" #name "\n_" #name ":\n call _ar_in\n pushl 8(%esp)\n pushl 8(%esp)\n call *_" #target "\n addl $8, %esp\n" \
                                      " pushl %eax\n pushl %edx\n call _ar_out\n popl %edx\n popl %eax\n ret\n")
AR_THUNK(ar_thunk_rr, ar_to_rr);
AR_THUNK(ar_thunk_ri, ar_to_ri);
extern char ar_thunk_rr[], ar_thunk_ri[];
static int ar_ok, ar_total, ar_bad1 = -1;     /* patched sites, first refused site (logged in Direct3DCreate9: the log is not open yet in DllMain) */
#ifdef AUDIORNG_BUILTIN      /* lane test build (agents/rep_w12/d3d9_ar.dll): audiorng on with the site list compiled in, so the tree's run.sh can use it via D3DP_DLL */
#include AUDIORNG_BUILTIN
#endif
static void ar_patch(void) {
#ifdef AUDIORNG_BUILTIN
    if (!n_ars) for (unsigned i = 0; i < sizeof ar_builtin / sizeof *ar_builtin && n_ars < NARS; i++, n_ars++) { ars_va[n_ars] = ar_builtin[i][0]; ars_to[n_ars] = ar_builtin[i][1]; }
#endif
    ar_total = n_ars;
    for (int i = 0; i < n_ars; i++) {
        unsigned char *p = (unsigned char *)(uintptr_t)ars_va[i]; DWORD old;
        char *th = ars_to[i] == 0x481fd0 ? ar_thunk_rr : ars_to[i] == 0x4c45c0 ? ar_thunk_ri : NULL;
        if (!th || IsBadReadPtr(p, 5) || p[0] != 0xe8 || ars_va[i] + 5 + *(int *)(p + 1) != ars_to[i]) { if (ar_bad1 < 0) ar_bad1 = (int)ars_va[i]; continue; }
        VirtualProtect(p, 5, PAGE_EXECUTE_READWRITE, &old); *(int *)(p + 1) = (int)((uintptr_t)th - (ars_va[i] + 5)); VirtualProtect(p, 5, old, &old); ar_ok++;
    }
}

/* ---- entry point -------------------------------------------------------------------------------------------------------------- */
static void cfg_line(char *ln, char *outbuf, int inc) {      /* inc: a "C " line of a replay file (play=) */
    {
        char *e = strchr(ln, '='); if (!e) return; *e++ = 0; e[strcspn(e, "\r\n")] = 0;
        if (inc && (!strcmp(ln, "out") || !strcmp(ln, "play") || !strcmp(ln, "rec"))) return;
        if (!strcmp(ln, "out")) strncpy(outbuf, e, 500);
        else if (!strcmp(ln, "play")) {      /* the file's C lines are settings: a cfg line after play= overrides them */
            strncpy(cfg_play, e, 259); FILE *p = fopen(e, "r"); char l2[512]; nrk = 0;
            while (p && fgets(l2, sizeof l2, p)) {
                if (l2[0] == 'C' && l2[1] == ' ') cfg_line(l2 + 2, outbuf, 1);
                else if (l2[0] == 'K' && nrk < NRK) {
                    char *q = l2 + 1, *x; rk[nrk].fr = strtol(q, &q, 10); rk[nrk].n = 0;
                    for (;;) { unsigned long d = strtoul(q, &x, 16); if (x == q) break; if (rk[nrk].n < 15) rk[nrk].k[rk[nrk].n++] = (BYTE)d; q = x; }
                    nrk++;
                }
            }
            if (p) fclose(p); else cfg_play[0] = 0;
        }
        else if (!strcmp(ln, "rec")) strncpy(cfg_rec, e, 259);
        else if (!strcmp(ln, "pace")) cfg_pace = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "sh_seed")) strncpy(cfg_seed, e, 259);
        else if (!strcmp(ln, "sh_seed_skip")) strncpy(cfg_seed_skip, e, 259);
        else if (!strcmp(ln, "sh_seed_k")) cfg_seed_k = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "sh_seed_from")) cfg_seed_from = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "sh_seed_range") && nsr < NSR && sscanf(e, "%x:%x", &sr[nsr][0], &sr[nsr][1]) == 2) nsr++;
        else if (!strcmp(ln, "start_phase")) cfg_start_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "phase_addr")) cfg_phase_addr = (u32)strtoul(e, 0, 0);
        else if (!strcmp(ln, "racef_addr")) cfg_racef_addr = (u32)strtoul(e, 0, 0);
        else if (!strcmp(ln, "stop_racef")) cfg_stop_racef = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vdump")) cfg_vdump = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "max_frames")) cfg_max_frames = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vclock")) cfg_vclock = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "anchor_phase")) cfg_anchor_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "stop_phase")) cfg_stop_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "step_us")) cfg_step_us = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "rwtap")) strncpy(cfg_rwtap, e, 259);
        else if (!strcmp(ln, "rwtap_out")) strncpy(cfg_rwtap_out, e, 259);
        else if (!strcmp(ln, "rwtap_delta")) cfg_rwtap_delta = (unsigned)strtoul(e, 0, 16);
        else if (!strcmp(ln, "geom")) cfg_geom = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "trace")) cfg_trace = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "statehash")) cfg_sh = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vsnap")) cfg_vsnap = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "alock")) cfg_alock = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vds")) cfg_vds = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vdssnap")) cfg_vdssnap = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "apoll")) cfg_apoll = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "aio")) cfg_aio = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vst")) cfg_vst = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "srand")) { unsigned va = 0; sscanf(e, "%u:%x:%u", &cfg_srand_ph, &va, &cfg_srand_seed); if (va) cfg_srand_va = va; }      /* va 0 (replay file): keep this run's _srand */
        else if (!strcmp(ln, "pin") && npin < NPIN && sscanf(e, "%u:%x:%x", &pins[npin].ph, &pins[npin].va, &pins[npin].v) == 3) npin++;
        else if (!strcmp(ln, "mkbd")) cfg_mkbd = (DWORD)strtoul(e, 0, 0);
        else if (!strcmp(ln, "mstep") && nms < NMS && e[0] && e[1] == ':') {      /* see mstep_frame */
            char *q = e + 2; ms[nms].op = e[0]; ms[nms].a = (int)strtol(q, &q, 0); ms[nms].va = (DWORD)strtoul(q + (*q == ':'), &q, 0);
            q += *q == ':'; ms[nms].cmp = '='; if (*q == '=' || *q == '!') ms[nms].cmp = *q++; ms[nms].v = (DWORD)strtoul(q, 0, 0); nms++; }
        else if (!strcmp(ln, "sh_base")) cfg_sh_base = (u32)strtoul(e, 0, 16);
        else if (!strcmp(ln, "sh_end")) cfg_sh_end = (u32)strtoul(e, 0, 16);
        else if (!strcmp(ln, "sh_delta")) cfg_sh_delta = (u32)strtoul(e, 0, 16);
        else if (!strcmp(ln, "sh_out")) strncpy(cfg_sh_out, e, 259);
        else if (!strcmp(ln, "sh_poke")) sscanf(e, "%d:%x:%x", &cfg_poke_frame, &cfg_poke_va, &cfg_poke_xor);
        else if (!strcmp(ln, "audiorng")) cfg_audiorng = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "audiorng_site") && n_ars < NARS && sscanf(e, "%x:%x", &ars_va[n_ars], &ars_to[n_ars]) == 2) n_ars++;
    }
}
static void read_cfg(const char **outp, char *outbuf) {
    FILE *f = fopen("d3dtrace.cfg", "r"); char ln[512];
    strcpy(outbuf, "d3dtrace.log"); npin = 0; n_ars = 0; nms = 0; nsr = 0;
    while (f && fgets(ln, sizeof ln, f)) cfg_line(ln, outbuf, 0);
    if (f) fclose(f);
    *outp = outbuf;
}
__declspec(dllexport) IDirect3D9 *WINAPI Direct3DCreate9(UINT ver) {
    static HMODULE real; static IDirect3D9 *(WINAPI *real_create)(UINT);
    if (!real) {
        char p[MAX_PATH]; GetSystemDirectoryA(p, sizeof p); strcat(p, "\\d3d9.dll");
        real = LoadLibraryA(p);
        if (!real) return NULL;
        real_create = (IDirect3D9 *(WINAPI *)(UINT))GetProcAddress(real, "Direct3DCreate9");
        InitializeCriticalSection(&cs);
        char ob[512]; const char *out; read_cfg(&out, ob);
        lg = fopen(out, "wb"); if (!lg) return NULL;
        setvbuf(lg, NULL, _IOFBF, 1 << 20);
        L("TRACE d3dtrace 1 sdk=%u start_phase=%d stop_racef=%d geom=%d trace=%d\n", ver, cfg_start_phase, cfg_stop_racef, cfg_geom, cfg_trace);
        if (cfg_audiorng) L("AUDIORNG patched %d of %d sites%s\n", ar_ok, ar_total, ar_bad1 >= 0 ? " (some refused)" : "");
        if (cfg_audiorng && ar_bad1 >= 0) L("AUDIORNG first refused site %x\n", (unsigned)ar_bad1);
        if (!cfg_start_phase && !cfg_stop_racef && !cfg_stop_phase) { active = 1; }
        if (cfg_rwtap[0]) {
            HMODULE tm = LoadLibraryA("mfltap.dll"); int (*ti)(const char *, const char *, unsigned, volatile int *, volatile int *, volatile int *) = tm ? (void *)GetProcAddress(tm, "rwtap_init") : NULL;
            L("RWTAP hooks=%d\n", ti ? ti(cfg_rwtap, cfg_rwtap_out, cfg_rwtap_delta, &active, &ndraws, &nframes) : -2);
        }
    }
    void *d3d = (void *)real_create(ver);
    if (d3d) {
        void **vt = *(void ***)d3d; void *dummy;
        if (!orig_d3d[0]) memcpy(orig_d3d, vt, sizeof(void *) * 17);
        if (vt[16] != (void *)h_d3d_CreateDevice) { vt_patch(vt, 16, h_d3d_CreateDevice, &dummy); vt_patch(vt, 10, h_d3d_CheckDeviceFormat, &dummy); }
    }
    return d3d;
}
BOOL WINAPI DllMain(HINSTANCE h, DWORD why, void *r) {
    if (why == DLL_PROCESS_ATTACH) {
        char ob[512]; const char *o; read_cfg(&o, ob);       /* cfg is re-read in Direct3DCreate9 for the log file; the clock must be installed before the game starts */
        InitializeCriticalSection(&acs);
#ifdef AUDIORNG_BUILTIN
        cfg_audiorng = 1;
#endif
        if (cfg_audiorng) ar_patch();
        if (cfg_vclock) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); qpf = f.QuadPart; main_tid = GetCurrentThreadId(); patch_iat(GetModuleHandleA(NULL)); }
    }
    if (why == DLL_PROCESS_DETACH && lg) fflush(lg);
    return TRUE;
}
