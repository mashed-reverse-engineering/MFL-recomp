/* rwtap: logs calls into the RenderWare 3.7 layer of the (original or standalone) MFL.exe.  Loaded by the d3dtrace proxy (cfg `rwtap=<funcs.txt>`),
 * hooks every function of funcs.txt by an inline detour (displaced prologue + jmp back), logs the 8 stack dwords at entry (cdecl args, hex) and the
 * pointed-to data named by the spec.  Return values are not captured.  Format: see README.md. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
static FILE *lg; static int active_(void); static CRITICAL_SECTION cs; static volatile int *p_active, *p_ndraws, *p_nframes;
static unsigned seq, ncall; static char names[128][48]; static char specs[128][96]; static unsigned nargs[128];
static void dump(const u8 *p, unsigned n) {
    if (n > 65536) n = 65536;
    if (IsBadReadPtr(p, n)) { fputs("!", lg); return; }
    for (unsigned i = 0; i < n; i++) fprintf(lg, "%02x", p[i]);
}
static unsigned g_delta; static unsigned atomic_idx = ~0u;                   /* hook index of the D3D9 default render callback (0x4faeb0): atomics get dumped (G / M / A / X records) */
static unsigned retstk[512]; static int rsp; static u8 *exit_stub;
static unsigned seen_g[8192][2], seen_m[8192][2];  /* (pointer, content hash) of the geometries / materials already written */
static unsigned fnv(const u8 *p, unsigned n, unsigned h) { while (n--) h = (h ^ *p++) * 16777619u; return h; }
static int rd(const void *p, unsigned n) { return p && !IsBadReadPtr(p, n); }
static int fresh(unsigned (*t)[2], unsigned ptr, unsigned h) {   /* 1 if (ptr, h) is new; records it */
    unsigned i = (ptr >> 3) & 8191;
    for (unsigned k = 0; k < 8192; k++, i = (i + 1) & 8191) {
        if (!t[i][0]) { t[i][0] = ptr; t[i][1] = h; return 1; }
        if (t[i][0] == ptr) { if (t[i][1] == h) return 0; t[i][1] = h; return 1; }
    }
    return 0;
}
static void hexn(const void *p, unsigned n) { const u8 *b = p; if (!rd(b, n)) { fputs("!", lg); return; } for (unsigned i = 0; i < n; i++) fprintf(lg, "%02x", b[i]); }
static void dump_material(unsigned mp) {   /* RpMaterial: texture 0, color 4, surfaceProps 0xc; RwTexture: raster 0, name 0x10, filterAddressing 0x50; RwRaster: width 0xc, height 0x10 */
    const u8 *m = (const u8 *)(uintptr_t)mp; if (!rd(m, 0x1c)) return;
    unsigned tex = *(const unsigned *)m, h = fnv(m, 0x1c, 2166136261u);
    if (rd((void *)(uintptr_t)tex, 0x58)) h = fnv((const u8 *)(uintptr_t)tex + 0x10, 0x44, h);
    if (!fresh(seen_m, mp, h)) return;
    fprintf(lg, "M %x col=", mp); hexn(m + 4, 4); fputs(" sp=", lg); hexn(m + 0xc, 12);
    if (rd((void *)(uintptr_t)tex, 0x58)) {
        const u8 *t = (const u8 *)(uintptr_t)tex; const u8 *r = (const u8 *)(uintptr_t)*(const unsigned *)t; char nm[33] = ""; memcpy(nm, t + 0x10, 32);
        fprintf(lg, " tex=%s fa=%x rw=%d rh=%d", nm[0] ? nm : "-", *(const unsigned *)(t + 0x50), rd(r, 0x14) ? *(const int *)(r + 0xc) : 0, rd(r, 0x14) ? *(const int *)(r + 0x10) : 0);
    } else fputs(" tex=-", lg);
    fputc('\n', lg);
}
/* RpGeometry (3.7): flags 8, numTriangles 0x10, numVertices 0x14, numMorphTargets 0x18, numTexCoordSets 0x1c, matList{materials 0x20, num 0x24}, triangles 0x2c, preLitLum 0x30,
 * texCoords[8] 0x34, mesh 0x54, morphTarget 0x5c (RpMorphTarget 28 B: sphere 4, verts 0x14, normals 0x18); RpMeshHeader 16 B + RpMesh{indices, numIndices, material} 12 B each */
static void dump_geometry(unsigned gp) {
    const u8 *g = (const u8 *)(uintptr_t)gp; if (!rd(g, 0x60)) return;
    unsigned fl = *(const unsigned *)(g + 8); int nt = *(const int *)(g + 0x10), nv = *(const int *)(g + 0x14), nm = *(const int *)(g + 0x18), ntc = *(const int *)(g + 0x1c), nmat = *(const int *)(g + 0x24);
    const unsigned *ml = *(const unsigned **)(g + 0x20); const u8 *tri = *(const u8 **)(g + 0x2c), *lit = *(const u8 **)(g + 0x30), *mh = *(const u8 **)(g + 0x54), *mt = *(const u8 **)(g + 0x5c);
    if (nt < 0 || nv < 0 || nt > 100000 || nv > 100000 || nmat < 0 || nmat > 256 || ntc < 0 || ntc > 8) return;
    unsigned h = fnv(g + 8, 0x18, 2166136261u); h = fnv(g + 0x20, 0x44, h);
    if (tri && rd(tri, nt * 8)) h = fnv(tri, nt * 8, h);
    if (mt && rd(mt, 28) && nv) { const u8 *v = *(const u8 **)(mt + 0x14); h = fnv(mt + 4, 16, h); if (rd(v, nv * 12)) h = fnv(v, nv * 12, h); }
    if (lit && nv && rd(lit, nv * 4)) h = fnv(lit, nv * 4, h);
    for (int s = 0; s < ntc && s < 2; s++) { const u8 *tc = *(const u8 **)(g + 0x34 + 4 * s); if (tc && nv && rd(tc, nv * 8)) h = fnv(tc, nv * 8, h); }
    if (!fresh(seen_g, gp, h)) return;
    for (int i = 0; i < nmat && rd(ml, nmat * 4); i++) dump_material(ml[i]);
    if (rd(mh, 16)) for (unsigned i = 0, n = *(const unsigned short *)(mh + 4); i < n && i < 1024; i++) { const u8 *me = mh + 16 + *(const unsigned *)(mh + 12) + 12 * i; if (rd(me, 12)) dump_material(*(const unsigned *)(me + 8)); }
    fprintf(lg, "G %x flags=%x nt=%d nv=%d nm=%d ntc=%d nmat=%d", gp, fl, nt, nv, nm, ntc, nmat);
    fputs(" mats=", lg); for (int i = 0; i < nmat && rd(ml, nmat * 4); i++) fprintf(lg, i ? ",%x" : "%x", ml[i]);
    fputs(" tri=", lg); if (tri && nt) hexn(tri, nt * 8); else fputs("-", lg);
    fputs(" lit=", lg); if (lit && nv) hexn(lit, nv * 4); else fputs("-", lg);
    for (int s = 0; s < ntc && s < 2; s++) { const u8 *tc = *(const u8 **)(g + 0x34 + 4 * s); fprintf(lg, " tc%d=", s); if (tc && nv) hexn(tc, nv * 8); else fputs("-", lg); }
    fputs(" sph=", lg); if (rd(mt, 28)) hexn(mt + 4, 16); else fputs("-", lg);
    fputs(" verts=", lg); if (rd(mt, 28) && nv && *(const u8 **)(mt + 0x14)) hexn(*(const u8 **)(mt + 0x14), nv * 12); else fputs("-", lg);
    fputs(" norm=", lg); if (rd(mt, 28) && nv && *(const u8 **)(mt + 0x18)) hexn(*(const u8 **)(mt + 0x18), nv * 12); else fputs("-", lg);
    fputs(" mesh=", lg);
    if (rd(mh, 16)) {
        unsigned n = *(const unsigned short *)(mh + 4); fprintf(lg, "%x:%u", *(const unsigned *)mh, n);
        for (unsigned i = 0; i < n && i < 1024; i++) { const u8 *me = mh + 16 + *(const unsigned *)(mh + 12) + 12 * i; if (!rd(me, 12)) break;
            unsigned ni = *(const unsigned *)(me + 4); fprintf(lg, ";%x:%u:", *(const unsigned *)(me + 8), ni); hexn(*(const void **)me, ni * 2); }
    } else fputs("-", lg);
    fputc('\n', lg);
}
static void exit_hook(unsigned *slot) { *slot = rsp > 0 ? retstk[--rsp] : 0; if (active_()) { EnterCriticalSection(&cs); fprintf(lg, "X %u d=%d f=%d\n", seq++, *p_ndraws, *p_nframes); LeaveCriticalSection(&cs); } }
/* object of type 1 = RpAtomic: RpGeometry at +0x18, RwFrame at +4 (ltm at +0x50), flags +2.  type 2 (world sector) is counted by the plain C line only. */
static void on_atomic(const unsigned *a, unsigned *ra) {
    if ((a[2] & 0xff) != 1) return;
    const u8 *at = (const u8 *)(uintptr_t)a[1]; if (!rd(at, 0x70)) return;
    unsigned fr = *(const unsigned *)(at + 4), gp = *(const unsigned *)(at + 0x18);
    dump_geometry(gp);
    fprintf(lg, "A %u d=%d f=%d atom=%x geo=%x fl=%x hdr=%x", seq++, *p_ndraws, *p_nframes, a[1], gp, at[2], a[0]);
    fputs(" ltm=", lg); if (rd((void *)(uintptr_t)fr, 0x90)) hexn((const u8 *)(uintptr_t)fr + 0x50, 64); else fputs("!", lg);
    /* RwResEntry 0x18 B + RxD3D9ResEntryHeader (numMeshes +4, instance array at +0x40 of the header, 36 B each: numIndex, minVert, material, vertexAlpha, shader, baseIndex, numVertices, startIndex, numPrims) */
    const u8 *re = (const u8 *)(uintptr_t)a[0]; fputs(" inst=", lg);
    if (rd(re, 0x60)) { unsigned nmesh = *(const unsigned *)(re + 0x18 + 4); fprintf(lg, "%x:%x:", *(const unsigned *)(re + 0x18 + 0xc), nmesh); if (nmesh < 1024) hexn(re + 0x58, nmesh * 36); } else fputs("!", lg);
    fputs(" bs=", lg); hexn(at + 0x1c, 16);   /* RpAtomic boundingSphere (object space) */
    fputs(" amb=", lg); hexn((const void *)(uintptr_t)(0x62e738 + g_delta), 12);   /* ambient light sum of the lighting callback (0x4ff4c0), used by RwD3D9SetSurfaceProperties */
    fputc('\n', lg);
    if (rsp < 512) { retstk[rsp++] = *ra; *ra = (unsigned)(uintptr_t)exit_stub; }
}
/* called by the stub with the idx and a pointer at the return address; args follow it */
static void __cdecl on_call(unsigned idx, unsigned *ra) {
    if (!*p_active) return;
    const unsigned *a = ra + 1; EnterCriticalSection(&cs);
    fprintf(lg, "C %u %s d=%d f=%d a=", seq++, names[idx], *p_ndraws, *p_nframes);
    for (unsigned i = 0; i < nargs[idx]; i++) fprintf(lg, i ? ",%x" : "%x", a[i]);
    char sp[96]; strcpy(sp, specs[idx]);
    for (char *t = strtok(sp, " "); t; t = strtok(NULL, " ")) {
        int k = atoi(t + 1); const char *c = strchr(t, ':') + 1; unsigned n;
        if (*c == 'n') { int m = atoi(c + 1); n = a[m] * (unsigned)atoi(strchr(c, '*') + 1); } else n = (unsigned)atoi(c);
        fprintf(lg, " p%d=", k); dump((const u8 *)(uintptr_t)a[k], n);
    }
    fputc('\n', lg);
    if (idx == atomic_idx) on_atomic(a, ra);
    LeaveCriticalSection(&cs);
}
static int active_(void) { return *p_active; }
static u8 *make_exit_stub(void) {   /* push 0 (slot for the original return address); pushfd; pushad; lea eax,[esp+36]; push eax; call exit_hook; add esp,4; popad; popfd; ret */
    u8 *s = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE), *q = s; if (!s) return NULL;
    *q++ = 0x6a; *q++ = 0; *q++ = 0x9c; *q++ = 0x60; *q++ = 0x8d; *q++ = 0x44; *q++ = 0x24; *q++ = 0x24; *q++ = 0x50;
    *q++ = 0xb8; void *f = (void *)exit_hook; memcpy(q, &f, 4); q += 4; *q++ = 0xff; *q++ = 0xd0; *q++ = 0x83; *q++ = 0xc4; *q++ = 0x04; *q++ = 0x61; *q++ = 0x9d; *q++ = 0xc3;
    return s;
}
static int hook(unsigned idx, u8 *fn, unsigned len) {
    u8 *s = VirtualAlloc(NULL, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); if (!s) return 0; u8 *q = s;
    *q++ = 0x9c; *q++ = 0x60;                       /* pushfd; pushad */
    *q++ = 0x8d; *q++ = 0x44; *q++ = 0x24; *q++ = 0x24;   /* lea eax,[esp+36] = return address of the hooked call */
    *q++ = 0x50; *q++ = 0x68; memcpy(q, &idx, 4); q += 4;  /* push eax; push idx */
    *q++ = 0xb8; void *f = (void *)on_call; memcpy(q, &f, 4); q += 4; *q++ = 0xff; *q++ = 0xd0;   /* mov eax,on_call; call eax */
    *q++ = 0x83; *q++ = 0xc4; *q++ = 0x08;          /* add esp,8 */
    *q++ = 0x61; *q++ = 0x9d;                       /* popad; popfd */
    memcpy(q, fn, len); q += len;                   /* displaced instructions */
    *q++ = 0xe9; int rel = (int)((u8 *)fn + len - (q + 4)); memcpy(q, &rel, 4); q += 4;
    DWORD old; if (!VirtualProtect(fn, len, PAGE_EXECUTE_READWRITE, &old)) return 0;
    fn[0] = 0xe9; rel = (int)(s - (fn + 5)); memcpy(fn + 1, &rel, 4); for (unsigned i = 5; i < len; i++) fn[i] = 0x90;
    VirtualProtect(fn, len, old, &old); FlushInstructionCache(GetCurrentProcess(), fn, len); return 1;
}
/* funcs: funcs.txt path; delta: hex offset added to every VA (standalone link); returns number of hooks */
__declspec(dllexport) int rwtap_init(const char *funcs, const char *out, unsigned delta, volatile int *active, volatile int *ndraws, volatile int *nframes) {
    g_delta = delta; InitializeCriticalSection(&cs); p_active = active; p_ndraws = ndraws; p_nframes = nframes;
    lg = fopen(out, "wb"); FILE *f = fopen(funcs, "r"); if (!lg || !f) return -1;
    setvbuf(lg, NULL, _IOFBF, 1 << 20); fprintf(lg, "RWTAP 2 delta=%x\n", delta);
    char ln[256]; int n = 0;
    while (fgets(ln, sizeof ln, f) && ncall < 128) {
        if (ln[0] == '#') continue; char nm[64], sp[200] = ""; unsigned va, len, na_;
        char *e = ln + strcspn(ln, "\r\n"); *e = 0;
        int k = sscanf(ln, "%47s %x %u %u %95[^\n]", nm, &va, &len, &na_, sp); if (k < 4) continue; nargs[ncall] = na_ > 8 ? 8 : na_;
        strcpy(names[ncall], nm); if (!strcmp(nm, "rxD3D9DefaultRenderCallback")) { atomic_idx = ncall; if (!exit_stub) exit_stub = make_exit_stub(); } strcpy(specs[ncall], k > 4 ? sp : "");
        if (hook(ncall, (u8 *)(uintptr_t)(va + delta), len)) { fprintf(lg, "H %u %s %x\n", ncall, nm, va + delta); n++; }
        ncall++;
    }
    fclose(f); fflush(lg); return n;
}
BOOL WINAPI DllMain(HINSTANCE h, DWORD r, void *x) { return TRUE; }
