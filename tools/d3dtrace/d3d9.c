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
 *   max_frames=<n>        stop after n logged Presents (0 = unlimited)
 *   geom=1                hash vertex/index buffer contents at Unlock (default 1)
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
static int cfg_start_phase, cfg_stop_racef, cfg_max_frames, cfg_geom = 1, cfg_vclock = 1, cfg_step_us = 20, cfg_stop_phase;
static u32 cfg_phase_addr = 0x652f68, cfg_racef_addr;
static int active, nframes, ndraws, nunhooked;
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
    if (on && !active) { active = 1; dump_state(); }
    else if (!on && active) { active = 0; L("INACTIVE\n"); fflush(lg); }
}

/* ---- device hooks ------------------------------------------------------------------------------------------------------------ */
#define ACT if (active)
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
static HRESULT WINAPI h_Present(void *d, const RECT *a, const RECT *b, HWND w, const RGNDATA *c) {
    HRESULT r = C(HRESULT (WINAPI *)(void *, const RECT *, const RECT *, HWND, const RGNDATA *), 17, d, a, b, w, c);
    DWORD ph = 0, rf = 0;
    if (!IsBadReadPtr((void *)(uintptr_t)cfg_phase_addr, 4)) ph = *(DWORD *)(uintptr_t)cfg_phase_addr;
    if (cfg_racef_addr && !IsBadReadPtr((void *)(uintptr_t)cfg_racef_addr, 4)) rf = *(DWORD *)(uintptr_t)cfg_racef_addr;
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
static HRESULT WINAPI h_DrawPrimitive(void *d, D3DPRIMITIVETYPE t, UINT sv, UINT pc) { ACT { L("D %d DP %d %u %u vh=%016llx\n", ndraws, t, sv, pc, (unsigned long long)vhash(sv, vcount(t, pc))); ndraws++; } return C(HRESULT (WINAPI *)(void *, D3DPRIMITIVETYPE, UINT, UINT), 81, d, t, sv, pc); }
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
static LONGLONG vnow(void) { if (is_main()) vt_us += cfg_step_us; return vt_us; }
static BOOL WINAPI v_QPC(LARGE_INTEGER *c) { c->QuadPart = vnow() * qpf / 1000000; return TRUE; }
static DWORD WINAPI v_GetTickCount(void) { return (DWORD)(vnow() / 1000) + 100000; }
static DWORD WINAPI v_timeGetTime(void) { return (DWORD)(vnow() / 1000) + 100000; }
static void WINAPI v_GetSystemTimeAsFileTime(FILETIME *ft) { ULONGLONG t = 132000000000000000ull + (ULONGLONG)vnow() * 10; ft->dwLowDateTime = (DWORD)t; ft->dwHighDateTime = (DWORD)(t >> 32); }
static void WINAPI v_Sleep(DWORD ms) { if (is_main()) { vt_us += (LONGLONG)ms * 1000; Sleep(0); } else Sleep(ms); }
static void patch_iat(HMODULE m) {
    unsigned char *b = (unsigned char *)m; IMAGE_DOS_HEADER *dh = (void *)b; IMAGE_NT_HEADERS *nt = (void *)(b + dh->e_lfanew);
    IMAGE_DATA_DIRECTORY *dd = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    struct { const char *name; void *fn; } hk[] = { {"QueryPerformanceCounter", v_QPC}, {"GetTickCount", v_GetTickCount}, {"timeGetTime", v_timeGetTime}, {"GetSystemTimeAsFileTime", v_GetSystemTimeAsFileTime}, {"Sleep", v_Sleep} };
    for (IMAGE_IMPORT_DESCRIPTOR *d = (void *)(b + dd->VirtualAddress); dd->VirtualAddress && d->Name; d++) {
        IMAGE_THUNK_DATA *ot = (void *)(b + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk)), *ft = (void *)(b + d->FirstThunk);
        for (; ot->u1.AddressOfData; ot++, ft++) {
            if (ot->u1.Ordinal & 0x80000000) continue;
            const char *n = (const char *)((IMAGE_IMPORT_BY_NAME *)(b + ot->u1.AddressOfData))->Name;
            for (unsigned i = 0; i < sizeof hk / sizeof *hk; i++) if (!strcmp(n, hk[i].name)) { DWORD old; VirtualProtect(&ft->u1.Function, 4, PAGE_READWRITE, &old); ft->u1.Function = (DWORD)(uintptr_t)hk[i].fn; VirtualProtect(&ft->u1.Function, 4, old, &old); }
        }
    }
}

/* ---- entry point -------------------------------------------------------------------------------------------------------------- */
static void read_cfg(const char **outp, char *outbuf) {
    FILE *f = fopen("d3dtrace.cfg", "r"); char ln[512];
    strcpy(outbuf, "d3dtrace.log");
    while (f && fgets(ln, sizeof ln, f)) {
        char *e = strchr(ln, '='); if (!e) continue; *e++ = 0; e[strcspn(e, "\r\n")] = 0;
        if (!strcmp(ln, "out")) strncpy(outbuf, e, 500);
        else if (!strcmp(ln, "start_phase")) cfg_start_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "phase_addr")) cfg_phase_addr = (u32)strtoul(e, 0, 0);
        else if (!strcmp(ln, "racef_addr")) cfg_racef_addr = (u32)strtoul(e, 0, 0);
        else if (!strcmp(ln, "stop_racef")) cfg_stop_racef = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "max_frames")) cfg_max_frames = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "vclock")) cfg_vclock = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "anchor_phase")) cfg_anchor_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "stop_phase")) cfg_stop_phase = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "step_us")) cfg_step_us = (int)strtol(e, 0, 0);
        else if (!strcmp(ln, "geom")) cfg_geom = (int)strtol(e, 0, 0);
    }
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
        L("TRACE d3dtrace 1 sdk=%u start_phase=%d stop_racef=%d geom=%d\n", ver, cfg_start_phase, cfg_stop_racef, cfg_geom);
        if (!cfg_start_phase && !cfg_stop_racef && !cfg_stop_phase) { active = 1; }
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
        if (cfg_vclock) { LARGE_INTEGER f; QueryPerformanceFrequency(&f); qpf = f.QuadPart; main_tid = GetCurrentThreadId(); patch_iat(GetModuleHandleA(NULL)); }
    }
    if (why == DLL_PROCESS_DETACH && lg) fflush(lg);
    return TRUE;
}
