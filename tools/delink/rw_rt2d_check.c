/* rw_rt2d_check.c: bit-exact check of the librw DLL's rt2d kernels (tools/delink/rw_rt2d.txt, librw-mfl capi/rwrt2d.cpp: _rt2dCTMGet, Rt2dPath
   Empty / Moveto / Lineto / Close / Destroy / GetBBox, _rt2dFlatSubPathExtendBBox, Rt2dBrushCreate) against the ORIGINAL code of MFL.exe, both run
   natively (Wine) on the same CPU.  Image mapping as rw_rwmath_check.c (this exe at 0x400000, MFL.exe's sections copied into a bss block that spans
   0x4b0000..0x94b000, so the originals run in place).  As in the game link, the original's calls into g0 / g1 (_rwSqrt, _rwInvSqrt, RwMatrixMultiply /
   Translate / Scale / Transform / Rotate / OrthoNormalize / Optimize / Invert) are pointed at the DLL's exports (jmp patch); RwFrameGetLTM 0x4d0ff0 is
   jmp-patched to a harness function (frame + 0x50).  The SList functions, Rt2dPathLock and Rt2dBrushSetUV stay original and both sides call them (the
   DLL through mflrw_rt2d_bind).  RwEngineInstance 0x7d2128 points at a harness engine whose malloc / free / realloc / free list functions (+0x108 ..
   +0x11c) are one deterministic bump heap inside the object arena; every call is logged into the arena.
   usage: rw_rt2d_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg|fld]
   Per entry N finite cases + N/5 special cases (inf / NaN / random bits in the float inputs; reported, not counted).  x87 CW 0x7f.  Compared per case:
   the arena (objects, heap, allocator log, heap top), Rt2dGlobals 0x948060..0x948270, the return value, the control word, and (all cases)
   ebx / esi / edi / ebp + x87 TOP / tag word (call trampoline as in rw_rwmath_check.c; REGDIFF).
   neg = negative fixture: the DLL side gets an input changed (x <-> y of the point / first node / view window, bbox x0 <-> x1, the sub path chain cut
   behind the head, the brush free list swapped for the path one; restored after the call): DIFF on every entry.
   fld = planted fixture: an unbalanced fld1 in front of the DLL entry: REGDIFF on every entry. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_GET, K_EMPTY, K_MOVE, K_LINE, K_CLOSE, K_EXT, K_BBOX, K_DESTROY, K_BRUSH };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rt2dCTMGet", 0x563410, K_GET}, {"rw37_Rt2dPathEmpty", 0x563570, K_EMPTY}, {"rw37_Rt2dPathMoveto", 0x563600, K_MOVE},
  {"rw37_Rt2dPathLineto", 0x5636c0, K_LINE}, {"rw37_Rt2dPathClose", 0x5639a0, K_CLOSE}, {"rw37__rt2dFlatSubPathExtendBBox", 0x564220, K_EXT},
  {"rw37_Rt2dPathGetBBox", 0x5642c0, K_BBOX}, {"rw37_Rt2dPathDestroy", 0x564450, K_DESTROY}, {"rw37_Rt2dBrushCreate", 0x5649f0, K_BRUSH}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef void (__cdecl *Bind)(void *engine, void *globals, void *newEntry, void *slistDestroy, void *pathLock, void *brushSetUV, void *texDestroy,
                             void *sqrtFn, void *invSqrtFn, void *mul, void *scale, void *translate, void *invert, void *frameLTM);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float spf(float f) {
  u32 r;
  if (!special || rnd() % 8) return f;
  r = rnd() % 4; return bitsf(r == 0 ? 0x7f800000 : r == 1 ? 0xff800000 : r == 2 ? 0x7fc00000 : rnd());
}
static u32 fw(float f) { u32 w; memcpy(&w, &f, 4); return w; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
#define LO 0x4b0000u
#define HI 0x94b000u
static unsigned char mfl_area[0x700000] __attribute__((aligned(4096)));
static int load_image(const char *path) {
  FILE *f = fopen(path, "rb"); unsigned char *d; long sz; IMAGE_NT_HEADERS32 *nt; IMAGE_SECTION_HEADER *s; int i; DWORD old;
  if (!f) return 0; fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET); d = malloc(sz); fread(d, 1, sz, f); fclose(f);
  if ((uintptr_t)mfl_area > LO || (uintptr_t)mfl_area + sizeof mfl_area < HI) { printf("bss block at %p does not cover %x..%x\n", mfl_area, LO, HI); return 0; }
  if (!VirtualProtect((void *)LO, HI - LO, PAGE_EXECUTE_READWRITE, &old)) { printf("VirtualProtect failed %lu\n", GetLastError()); return 0; }
  nt = (IMAGE_NT_HEADERS32 *)(d + ((IMAGE_DOS_HEADER *)d)->e_lfanew); s = IMAGE_FIRST_SECTION(nt);
  for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
    u32 va = 0x400000 + s[i].VirtualAddress, n = s[i].SizeOfRawData < s[i].Misc.VirtualSize || !s[i].Misc.VirtualSize ? s[i].SizeOfRawData : s[i].Misc.VirtualSize, k;
    for (k = 0; k < n; k++) if (va + k >= LO && va + k < HI) ((unsigned char *)(uintptr_t)(va + k))[0] = d[s[i].PointerToRawData + k];
  }
  return 1;
}
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }

/* call trampoline (rw_rwmath_check.c): g_fn(g_args[0..3]) with ebx / esi / edi / ebp = g_sent; afterwards g_out, g_ret, g_env (fnstenv, then fninit + CW) */
u32 g_fn, g_fn2, g_args[4], g_ret, g_sent[4], g_out[4], g_savesp; u16 g_env[14];
u32 tramp(void); extern char plant_fld[];
__asm__(".text\n.globl _tramp\n_tramp:\n"
        " pushl %ebx\n pushl %esi\n pushl %edi\n pushl %ebp\n movl %esp, _g_savesp\n"
        " pushl _g_args+12\n pushl _g_args+8\n pushl _g_args+4\n pushl _g_args\n"
        " movl _g_sent, %ebx\n movl _g_sent+4, %esi\n movl _g_sent+8, %edi\n movl _g_sent+12, %ebp\n"
        " call *_g_fn\n"
        " movl %ebx, _g_out\n movl %esi, _g_out+4\n movl %edi, _g_out+8\n movl %ebp, _g_out+12\n movl %eax, _g_ret\n"
        " fnstenv _g_env\n fninit\n fldcw _g_env\n"
        " movl _g_savesp, %esp\n popl %ebp\n popl %edi\n popl %esi\n popl %ebx\n ret\n"
        ".globl _plant_fld\n_plant_fld:\n fld1\n jmp *_g_fn2\n");
static u32 call4(void *fn, u32 a, u32 b, u32 c, u32 d) { g_fn = (u32)(uintptr_t)fn; g_args[0] = a; g_args[1] = b; g_args[2] = c; g_args[3] = d; return tramp(); }
#define P(x) ((u32)(uintptr_t)(x))

/* ---- arena: allocator log at the start, then one bump heap (setup objects and the calls' allocations) */
#define ASZ (1u << 20)
#define HEAP0 0x1000u
static u8 *arena, *snap, *after0;
static u32 *hp;            /* heap top offset, lives in the arena (compared) */
static u32 *logn;          /* log entries, in the arena */
static void hlog(u32 op, u32 a, u32 b) { u32 *l = (u32 *)(arena + 8); if (*logn < 300) { l[3 * *logn] = op; l[3 * *logn + 1] = a; l[3 * *logn + 2] = b; } (*logn)++; }
static u32 aoff(const void *p) { return p == 0 ? 0 : (const u8 *)p >= arena && (const u8 *)p < arena + ASZ ? (u32)((const u8 *)p - arena) : 0xbad00000u ^ (u32)(uintptr_t)p; }
static void *hraw(u32 n) {      /* block: size word in front, contents a fixed pattern of the offset */
  u8 *p; u32 k, o;
  *hp = (*hp + 15) & ~15u; o = *hp + 16;
  if (o + n + 16 > ASZ) { printf("arena full\n"); exit(2); }
  p = arena + o; ((u32 *)p)[-1] = n; *hp = o + n;
  for (k = 0; k < n; k++) p[k] = (u8)((o + k) * 0x9du >> 3);
  return p;
}
static void *__cdecl e_malloc(u32 n, u32 hint) { void *p = hraw(n); hlog(1, n, aoff(p) ^ hint << 8); return p; }
static void __cdecl e_free(void *p) { hlog(2, aoff(p), 0); }
static void *__cdecl e_realloc(void *p, u32 n, u32 hint) { u8 *q = hraw(n); if (p) memcpy(q, p, ((u32 *)p)[-1] < n ? ((u32 *)p)[-1] : n); hlog(3, aoff(p), aoff(q) ^ n << 8 ^ hint); return q; }
static void *__cdecl e_calloc(u32 n, u32 m, u32 hint) { u8 *p = hraw(n * m); memset(p, 0, n * m); hlog(4, n * m, aoff(p) ^ hint); return p; }
static void *__cdecl e_flalloc(void *fl, u32 hint) { void *p = hraw(*(u32 *)fl); hlog(5, aoff(fl), aoff(p) ^ hint << 8); return p; }
static void __cdecl e_flfree(void *fl, void *p) { hlog(6, aoff(fl), aoff(p)); }
static void *__cdecl frame_ltm(u8 *frame) { return frame + 0x50; }
static void *allocr(u32 n) { u8 *p = hraw(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }

#define GLO 0x948060u
#define GSZ 0x210u
#define G(off) ((u8 *)(uintptr_t)(GLO + (off)))
static u8 gsnap[GSZ], gafter0[GSZ];
static u8 *eng, *flpath, *flbrush;

static void rmatrix(float *m) {      /* rotation about z * scale + pos (2D CTM style), sometimes random */
  int a; u32 kind = rnd() % 6;
  if (kind == 0) { for (a = 0; a < 16; a++) m[a] = spf(uf(-2, 2)); }
  else {
    float t = uf(-3.2f, 3.2f), s = rnd() % 3 ? 1 : uf(0.2f, 3);
    memset(m, 0, 64); m[0] = spf(s * (float)cos(t)); m[1] = spf(s * (float)sin(t)); m[4] = -m[1]; m[5] = m[0]; m[10] = 1;
    m[12] = spf(uf(-300, 300)); m[13] = spf(uf(-300, 300)); m[14] = rnd() % 2 ? 0 : uf(-2, 2);
  }
  ((u32 *)m)[3] = rnd() % 4 ? 3 : rnd() % 2 ? 0x20003 : rnd();
}
static float coord(void) { return spf(rnd() % 16 ? uf(-640, 640) : (float)(int)uf(-640, 640)); }
static void rnode(float *nd, int type) { ((u32 *)nd)[0] = type; nd[1] = coord(); nd[2] = coord(); nd[3] = spf(uf(-1, 1)); nd[4] = spf(uf(-1, 1)); nd[5] = spf(uf(0, 2000)); }
/* sub path: locked (SList {entries, n, alloc, 0x18}) or an array of n nodes; flat, closed random */
static u32 *mkpath(int n, int locked) {
  u32 *p = hraw(0x1c); int k; float *e;
  u32 al = n + (rnd() % 3 ? rnd() % 4 : 0);
  if (al == 0) al = 1;
  e = hraw(0x18 * al);
  for (k = 0; k < n; k++) rnode(e + 6 * k, k == 0 ? 0 : 1);
  if (locked) { u32 *l = hraw(16); l[0] = P(e); l[1] = n; l[2] = al; l[3] = 0x18; p[0] = P(l); p[1] = 0xffffffffu; }
  else { p[0] = P(e); p[1] = n; }
  p[2] = rnd() % 2; p[3] = 1; ((float *)p)[4] = uf(0, 2); p[5] = 0; p[6] = P(p);
  return p;
}
static u32 *mkchain(int subs, int nmin, int nmax, int lockall) {      /* head + subs sub paths; curr = the last */
  u32 *h = mkpath(nmin + rnd() % (nmax - nmin + 1), 1), *c = h; int k;
  for (k = 0; k < subs; k++) { u32 *s = mkpath(nmin + rnd() % (nmax - nmin + 1), lockall || rnd() % 2); c[5] = P(s); c = s; }
  h[6] = P(c);
  return h;
}
static u32 *firstnode(u32 *p) { return p[1] == 0xffffffffu ? (u32 *)(uintptr_t)((u32 *)(uintptr_t)p[0])[0] : (u32 *)(uintptr_t)p[0]; }
static void swap32(void *a, void *b) { u32 t; memcpy(&t, a, 4); memcpy(a, b, 4); memcpy(b, &t, 4); }

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[4096]; int k; for (k = 0; k < 4096; k++) b[k] = 0; }
static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"), fld = argc > 5 && !strcmp(argv[5], "fld");
  const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 40u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ + 64); snap = malloc(ASZ); after0 = malloc(ASZ); if (!arena || !snap || !after0) { printf("no memory\n"); return 2; }
  arena = (u8 *)(((uintptr_t)arena + 15) & ~(uintptr_t)15); hp = (u32 *)arena; logn = (u32 *)(arena + 4);
  { static const char *g[][2] = {{"_rwInvSqrt", (char *)0x4d5470}, {"_rwSqrt", (char *)0x4d5410}, {"RwMatrixMultiply", (char *)0x4d3cd0}, {"RwMatrixTranslate", (char *)0x4d4870},
      {"RwMatrixScale", (char *)0x4d46e0}, {"RwMatrixTransform", (char *)0x4d49c0}, {"RwMatrixRotate", (char *)0x4d43f0}, {"RwMatrixOrthoNormalize", (char *)0x4d3d50},
      {"RwMatrixOptimize", (char *)0x4d3c00}, {"RwMatrixInvert", (char *)0x4d4490}};
    int k; for (k = 0; k < 10; k++) { void *p = (void *)GetProcAddress(m, g[k][0]); if (!p) { printf("%s MISSING export (g0/g1)\n", g[k][0]); return 1; } jmp_patch((u32)(uintptr_t)g[k][1], p); } }
  jmp_patch(0x4d0ff0, (void *)frame_ltm);
  eng = calloc(1, 0x200); W(0x7d2128) = (u32)(uintptr_t)eng;
  *(void **)(eng + 0x108) = (void *)e_malloc; *(void **)(eng + 0x10c) = (void *)e_free; *(void **)(eng + 0x110) = (void *)e_realloc;
  *(void **)(eng + 0x114) = (void *)e_calloc; *(void **)(eng + 0x118) = (void *)e_flalloc; *(void **)(eng + 0x11c) = (void *)e_flfree;
  bind = (Bind)GetProcAddress(m, "mflrw_rt2d_bind");
  if (!bind) { printf("mflrw_rt2d_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)GLO, (void *)0x5d5e90, (void *)0x5d6030, (void *)0x564510, (void *)0x564d90, (void *)0x4d5bf0, (void *)0x4d5410, (void *)0x4d5470,
       (void *)0x4d3cd0, (void *)0x4d46e0, (void *)0x4d4870, (void *)0x4d4490, (void *)0x4d0ff0);
  cw_set(0x7f);
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: one input changed on the DLL side" : fld ? "  PLANTED FIXTURE: unbalanced fld1 in front of the DLL entry" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, badreg = 0, kind = T[fi].kind; long cov[4] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u32 a0 = 0, a1 = 0, a2 = 0, ret[2], x87[2], regbad = 0, top[2], setup; void *na = 0, *nb = 0; u32 *cut = 0; int brushswap = 0;
      cur_fi = fi; cur_case = i; special = i >= N;
      *hp = HEAP0; *logn = 0;
      flpath = hraw(4); *(u32 *)flpath = 0x1c; flbrush = hraw(4); *(u32 *)flbrush = 0x80;
      memset(G(0), 0, GSZ);
      *(u32 *)G(0x00) = P(flbrush); *(u32 *)G(0x24) = P(flpath); *(u32 *)G(0x28) = P(mkpath(rnd() % 3, 1));
      switch (kind) {
      case K_GET: {
        u8 *cam = allocr(0x80), *fr = allocr(0x90); int top_ = rnd() % 32;
        for (k = 0; k < 32; k++) if (k <= top_ || rnd() % 2) { float *mm = hraw(64); rmatrix(mm); *(u32 *)G(0x84 + 4 * k) = P(mm); }
        *(u32 *)G(0x104) = top_; rmatrix((float *)G(0x44)); rmatrix((float *)G(0x118));
        *(float *)G(0x108) = spf(uf(0.2f, 4)); *(float *)G(0x10c) = spf(rnd() % 4 ? uf(0.2f, 4) : *(float *)G(0x108));
        *(float *)G(0x1e0) = spf(rnd() % 3 ? 1 : uf(0.1f, 10)); *(u32 *)G(0x1d8) = rnd() % 4 == 0;
        *(u32 *)(cam + 4) = P(fr); *(u32 *)(cam + 0x14) = rnd() % 3 ? 1 : 2; rmatrix((float *)(fr + 0x50));
        *(u32 *)G(0x20c) = P(cam); cov[*(u32 *)G(0x1d8) * 2 + (*(u32 *)(cam + 0x14) == 2)]++;
        na = G(0x108); nb = G(0x10c); break; }
      case K_EMPTY: case K_DESTROY: {
        u32 *h = mkchain(rnd() % 4, 0, 4, 0); a0 = P(h); cov[h[5] != 0]++;
        if (kind == K_DESTROY && rnd() % 8 == 0) { a0 = 0; cov[2]++; }
        cut = h; break; }
      case K_MOVE: case K_LINE: {
        int nmin = kind == K_LINE ? 1 : 0; u32 *h = mkchain(rnd() % 3, nmin, 4, 1), *c = (u32 *)(uintptr_t)h[6], *l = (u32 *)(uintptr_t)c[0];
        a0 = P(h); a1 = fw(coord()); a2 = fw(coord()); cov[l[1] == 0 ? 0 : l[1] == l[2] ? 1 : 2]++;      /* empty / full (grows) / room */
        break; }
      case K_CLOSE: {
        u32 *h = mkchain(rnd() % 3, 1, 5, 1), *c = (u32 *)(uintptr_t)h[6], *l = (u32 *)(uintptr_t)c[0], *e = (u32 *)(uintptr_t)l[0];
        if (rnd() % 3 == 0) { e[6 * (l[1] - 1) + 1] = e[1]; e[6 * (l[1] - 1) + 2] = e[2]; }      /* last node on the first: no closing line */
        cov[e[6 * (l[1] - 1) + 1] == e[1] && e[6 * (l[1] - 1) + 2] == e[2]]++; cov[2 + (l[1] == l[2])]++;
        a0 = P(h); na = e + 1; nb = e + 2; break; }
      case K_EXT: {
        u32 *p = mkpath(rnd() % 7, rnd() % 2); float *b = allocr(16);
        for (k = 0; k < 4; k++) b[k] = spf(uf(-700, 700));
        a0 = P(p); a1 = P(b); cov[p[1] == 0xffffffffu]++; na = b; nb = b + 2; break; }
      case K_BBOX: {
        u32 *h = mkchain(rnd() % 4, 1, 5, 0); float *b = allocr(16);
        a0 = P(h); a1 = P(b); cov[h[5] != 0]++; na = firstnode(h) + 1; nb = firstnode(h) + 2; break; }
      case K_BRUSH: brushswap = 1; cov[0]++; break;
      }
      setup = *hp;
      memcpy(snap, arena, setup); memcpy(gsnap, G(0), GSZ);
      for (side = 0; side < 2; side++) {      /* both sides run on the same bytes (restored before each) */
        void *fn = side ? orig : dll; u32 r = 0, cutnext = 0;
        memcpy(arena, snap, setup); memcpy(G(0), gsnap, GSZ);
        if (neg && !side) {
          if (kind == K_MOVE || kind == K_LINE) swap32(&a1, &a2);
          else if (na) swap32(na, nb);
          else if (cut) { cutnext = cut[5]; cut[5] = 0; }
          else if (brushswap) swap32(G(0x00), G(0x24));
        }
        cw_set(0x7f);
        stackwipe();
        if (fld && !side) { g_fn2 = (u32)(uintptr_t)fn; fn = plant_fld; }
        for (k = 0; k < 4; k++) g_sent[k] = 0x5e000000u ^ (u32)i * 0x9e3779b1u ^ (u32)k << 28 ^ (u32)side << 20;
        r = call4(fn, a0, a1, a2, 0);
        for (k = 0; k < 4; k++) regbad |= g_out[k] != g_sent[k];
        x87[side] = (u32)(g_env[2] >> 11 & 7) << 16 | g_env[4];
        ret[side] = aoff((void *)(uintptr_t)r) ^ ((u32)cw_now() ^ 0x7f) << 24; cw_set(0x7f);      /* arena offset (or the global's address), CW change */
        if (neg && !side) {
          if (kind == K_MOVE || kind == K_LINE) swap32(&a1, &a2);
          else if (na) swap32(na, nb);
          else if (cut) cut[5] = cutnext;
          else if (brushswap) swap32(G(0x00), G(0x24));
        }
        top[side] = *hp;
        if (!side) { memcpy(after0, arena, *hp); memcpy(gafter0, G(0), GSZ); }
      }
      if (regbad || x87[0] != x87[1]) {
        if (++badreg <= 3) printf("  REGDIFF %s case %d: callee-saved %s, x87 TOP/tag dll %05x orig %05x\n", T[fi].name, i, regbad ? "CHANGED" : "kept", x87[0], x87[1]);
      }
      if (ret[0] == ret[1] && top[0] == top[1] && !memcmp(after0, arena, top[0]) && !memcmp(gafter0, G(0), GSZ)) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap top %x / %x", T[fi].name, i, ret[0], ret[1], top[0], top[1]);
        for (k = 0; k < (int)(top[0] < top[1] ? top[0] : top[1]); k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        for (k = 0; k < (int)GSZ; k += 4) if (*(u32 *)(gafter0 + k) != *(u32 *)G(k)) { printf(", first global diff at 0x%x: dll %08x orig %08x", GLO + k, *(u32 *)(gafter0 + k), *(u32 *)G(k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld; %d of %d special cases differ; regs/x87 %s", cov[0], cov[1], cov[2], cov[3], badsp, N / 5, badreg ? "REGDIFF" : "EQUAL");
    if (badreg) printf(" (%d cases)", badreg);
    printf("\n"); bad_total += bad != 0 || badreg != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
