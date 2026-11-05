/* rw_colltree_check.c: bit-exact check of the librw DLL's colltree kernels (tools/delink/rw_colltree.txt, librw-mfl capi/rwcolltree.cpp:
   BuildTriListCollTree, _rpCollTreeCreate, _rpCollTreeBuild, BuildTreeDestroy, BuildTreeGenerate, GetClipStats, DivisionScore,
   FuzzyExtentScore, SortPolygons, BuildPolySectorCreate, BuildPlaneSectorCreate, BuildDataDestroy, BuildTreeCountLeafNodes) against the
   ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_tristrip_check.c (same image mapping and harness heap:
   RwEngineInstance 0x7d2128 -> harness engine, malloc / free bump-allocate from one arena and log every call; RwErrorSet 0x4ea160 -> logger).
   RwBBoxInitialize / RwBBoxAddPoint, qsort and _rwerror run as original code on both sides (the DLL reaches them through mflrw_colltree_bind).
   x87 control word 0x7f (the game's) on even cases, 0x27f on odd ones.  Each side rebuilds the case from the same seed (inputs and setup
   objects in the arena, setup by the ORIGINAL code or harness C), then calls its function; compared: the used arena incl. slack, the call
   log, the return value (the 80-bit st0 for DivisionScore / FuzzyExtentScore) and the control word.
   Inputs: grids / boxes / terrain / random clouds of vertices (coordinates -100..100, 1 in 8 cases scaled by 2^+-40..100, repeated values,
   flat sets), triangles over them (random, neighbouring, duplicate), 1 in 10 cases with a failing allocation.
   usage: rw_colltree_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (restored after the call); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_TRILIST, K_BUILD, K_CREATE, K_TDESTROY, K_BTG, K_CLIP, K_DIV, K_FUZZ, K_SORT, K_POLYS, K_PLANES, K_DDESTROY, K_COUNT };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_BuildTriListCollTree", 0x54da80, K_TRILIST}, {"rw37__rpCollTreeCreate", 0x54dbc0, K_CREATE}, {"rw37__rpCollTreeBuild", 0x54e1c0, K_BUILD},
  {"rw37_BuildTreeDestroy", 0x54e470, K_TDESTROY}, {"rw37_BuildTreeGenerate", 0x54e4c0, K_BTG}, {"rw37_GetClipStats", 0x54ea50, K_CLIP},
  {"rw37_DivisionScore", 0x54ebf0, K_DIV}, {"rw37_FuzzyExtentScore", 0x54ecb0, K_FUZZ}, {"rw37_SortPolygons", 0x54ed10, K_SORT},
  {"rw37_BuildPolySectorCreate", 0x54ed50, K_POLYS}, {"rw37_BuildPlaneSectorCreate", 0x54ed80, K_PLANES}, {"rw37_BuildDataDestroy", 0x54edb0, K_DDESTROY},
  {"rw37_BuildTreeCountLeafNodes", 0x54ee10, K_COUNT}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef u32 (__cdecl *F2)(u32, u32);
typedef u32 (__cdecl *F3)(u32, u32, u32);
typedef u32 (__cdecl *F4)(u32, u32, u32, u32);
typedef u32 (__cdecl *F5)(u32, u32, u32, u32, u32);
typedef u32 (__cdecl *F7)(u32, u32, u32, u32, u32, u32, u32);
typedef long double (__cdecl *L2)(u32, u32);
typedef long double (__cdecl *L3)(u32, u32, u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
#define LO 0x4b0000u
#define HI 0x94b000u
#define W(a) (*(u32 *)(uintptr_t)(a))
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
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }
static void cw_set(unsigned short cw) { __asm__ volatile("fldcw %0" : : "m"(cw)); }
static unsigned short cw_now(void) { unsigned short cw; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw; }

/* ---- harness heap + call log */
#define HSZ (16u << 20)
#define LGMAX (1u << 20)
static u8 *heap, *save0; static u32 hp, nalloc, failAt, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static u8 eng[0x400], eng0[0x400];
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static void *__cdecl hmalloc(u32 n, u32 hint) { logw(0xa1100000u); logw(n); logw(hint); if (++nalloc == failAt || n > (4u << 20)) { logw(0); return 0; } { void *p = halloc(n); logw((u32)(uintptr_t)p); return p; } }
static void __cdecl hfree(void *p) { logw(0xa1200000u); logw((u32)(uintptr_t)p); }
static void *__cdecl herr(int32_t *e) { logw(0xa1700000u); logw(e[0]); logw(e[1]); return e; }
static void __cdecl hcallback(void *geom, void *tree, u16 *map, void *data) { logw(0xa1800000u); logw((u32)(uintptr_t)geom); logw((u32)(uintptr_t)tree); logw((u32)(uintptr_t)map); logw((u32)(uintptr_t)data); }

/* ---- inputs */
static float scl;
static float coord(void) {
  u32 m = rnd() % 16;
  if (m == 0) return 0.0f;
  if (m == 1) return (float)(int)(rnd() % 5) * scl;                 /* repeated values */
  if (m == 2 && rnd() % 64 == 0) { static const u32 sp[4] = {0x7f800000u, 0xff800000u, 0x7fc00000u, 0x7fa00001u}; float f; memcpy(&f, &sp[rnd() % 4], 4); return f; }      /* inf, -inf, qNaN, sNaN */
  return ((float)(int)(rnd() % 20001) - 10000.0f) * 0.01f * scl;
}
/* nv vertices (12 bytes) into v; returns the count */
static u32 gen_verts(float *v, u32 max) {
  u32 n = 0, mode = rnd() % 4, k;
  scl = rnd() % 8 == 0 ? ldexpf(1.0f, (int)(rnd() % 141) - 40) : 1.0f;
  if (mode == 0) {            /* grid / terrain */
    u32 w = 2 + rnd() % 8, h = 2 + rnd() % 8, x, y; float hgt = rnd() % 2 ? 0.0f : 1.0f;
    for (y = 0; y < h && n < max; y++) for (x = 0; x < w && n < max; x++) { v[3 * n] = (float)x * scl; v[3 * n + 1] = hgt * coord() * 0.1f; v[3 * n + 2] = (float)y * scl; n++; }
  } else if (mode == 1) {     /* box corners + cloud */
    for (k = 0; k < 8 && n < max; k++, n++) { v[3 * n] = (k & 1 ? 1.0f : -1.0f) * 10.0f * scl; v[3 * n + 1] = (k & 2 ? 1.0f : -1.0f) * 5.0f * scl; v[3 * n + 2] = (k & 4 ? 1.0f : -1.0f) * 2.0f * scl; }
    for (k = rnd() % 30; k && n < max; k--, n++) { v[3 * n] = coord(); v[3 * n + 1] = coord(); v[3 * n + 2] = coord(); }
  } else {                    /* cloud, sometimes flat */
    u32 flat = rnd() % 4; u32 c = 3 + rnd() % 60;
    for (k = 0; k < c && n < max; k++, n++) { v[3 * n] = coord(); v[3 * n + 1] = flat == 0 ? 1.0f : coord(); v[3 * n + 2] = flat == 1 ? 0.0f : coord(); }
  }
  if (n < 3) { for (; n < 3; n++) { v[3 * n] = (float)n; v[3 * n + 1] = 0; v[3 * n + 2] = (float)(n * n); } }
  return n;
}
/* RpTriangle {u16 v[3], matIndex} */
static u32 gen_tris(u16 *t, u32 max, u32 nv) {
  u32 n = rnd() % 8 == 0 ? max : 1 + rnd() % (max < 60 ? max : 60), k;
  for (k = 0; k < n; k++) {
    u32 a = rnd() % nv, b = rnd() % 3 ? (a + 1 + rnd() % 3) % nv : rnd() % nv, c = rnd() % 3 ? (a + nv - 1 - rnd() % 3) % nv : rnd() % nv;
    if (k && rnd() % 10 == 0) { memcpy(t + 4 * k, t + 4 * (rnd() % k), 8); continue; }
    t[4 * k] = (u16)a; t[4 * k + 1] = (u16)b; t[4 * k + 2] = (u16)c; t[4 * k + 3] = (u16)(rnd() % 4);
  }
  return n;
}
#define MAXV 160
#define MAXT 300

/* harness copy of _rpCollTreeBuild's setup (build data + arrays), bbox by the original RwBBox functions */
static u32 *mkbd(u32 nv, float *v, u32 nt, u16 *t) {
  u32 *bd = halloc(0x3c), i, *vi, *po; void (*binit)(u32, u32) = (void *)0x4d1990, (*badd)(u32, u32) = (void *)0x4d19d0;
  vi = halloc(nv * 12); po = halloc(nt * 8);
  bd[0] = nv; bd[1] = (u32)(uintptr_t)vi; bd[2] = (u32)(uintptr_t)po; bd[3] = (u32)(uintptr_t)t; bd[4] = rnd() % 8 == 0 ? 30 + rnd() % 4 : rnd() % 5;
  binit((u32)(uintptr_t)(bd + 5), (u32)(uintptr_t)v);
  for (i = 0; i < nv; i++) { badd((u32)(uintptr_t)(bd + 5), (u32)(uintptr_t)(v + 3 * i)); vi[3 * i] = (u32)(uintptr_t)(v + 3 * i); }
  for (i = 0; i < nt; i++) po[2 * i] = (u32)(uintptr_t)(t + 4 * i);
  bd[11] = nt; bd[12] = 0; bd[13] = 0; bd[14] = (u32)(uintptr_t)halloc(nv * 4);
  return bd;
}

static u8 *swapA, *swapB; static int swapn;
static void swapw(void) { u8 t[4]; if (!swapA) return; memcpy(t, swapA, swapn); memcpy(swapA, swapB, swapn); memcpy(swapB, t, swapn); }

/* build case `seed` for entry kind and run fn; the result word(s) into ret[0..2] */
static void run_case(int kind, void *fn, int isdll, int neg, u32 seed, u32 *ret, int *cov) {
  u32 nv, nt, *bd; float *v; u16 *t; u32 (*orig_btg)(u32) = (void *)0x54e4c0;
  rs = seed; hseed = seed * 2654435761u; hp = 0; nalloc = 0; failAt = 0; ln[side] = 0; swapA = swapB = 0; swapn = 4; ret[0] = ret[1] = ret[2] = 0;
  memcpy(eng, eng0, sizeof eng);
  v = halloc(MAXV * 12); nv = gen_verts(v, MAXV);
  t = halloc(MAXT * 8); nt = gen_tris(t, MAXT, nv);
  swapA = (u8 *)v; swapB = (u8 *)(v + 1);
  cov[0] = nt >= 4; cov[1] = 0;
  switch (kind) {
  case K_TRILIST: case K_BUILD: {
    u32 *p = 0, *geom = halloc(16), *map = 0; u16 *out = t; u32 m = rnd() % 4;
    if (rnd() % 10 == 0) failAt = 1 + rnd() % 6;
    if (kind == K_TRILIST) { if (m) { p = halloc(12); p[0] = m == 1 ? 0 : 1 | (rnd() & ~1u); p[1] = m == 3 ? 0 : (u32)(uintptr_t)hcallback; p[2] = rnd(); } if (rnd() % 4 == 0) { out = halloc(nt * 8); memcpy(out, t, nt * 8); } }
    else if (rnd() % 2) map = halloc(nt * 2);
    cov[1] = failAt != 0;
    if (neg && isdll) swapw();
    ret[0] = kind == K_TRILIST ? ((F7)fn)((u32)(uintptr_t)geom, nv, (u32)(uintptr_t)v, nt, (u32)(uintptr_t)t, (u32)(uintptr_t)out, (u32)(uintptr_t)p)
                               : ((F5)fn)(nv, (u32)(uintptr_t)v, nt, (u32)(uintptr_t)t, (u32)(uintptr_t)map);
    if (neg && isdll) swapw();
    break; }
  case K_CREATE: {
    u32 np = rnd() % 3 ? rnd() % 300 : rnd(), ns = rnd() % 3 ? rnd() % 200 : rnd(), fl = rnd() % 2 ? rnd() & 1 : rnd(); float *bb = halloc(24); int k;
    for (k = 0; k < 6; k++) bb[k] = coord();
    if ((np & 0xffff) > 2000) np &= 0x7ff; if ((ns & 0xffff) > 2000) ns &= 0x7ff;
    if (rnd() % 10 == 0) failAt = 1;
    cov[1] = fl & 1; swapA = (u8 *)bb; swapB = (u8 *)(bb + 3);
    if (neg && isdll) { u32 x = np; np = ns; ns = x; }
    ret[0] = ((F4)fn)(np, ns, (u32)(uintptr_t)bb, fl);
    break; }
  case K_TDESTROY: case K_COUNT: case K_BTG: case K_CLIP: case K_DIV: case K_SORT: case K_DDESTROY: {
    bd = mkbd(nv, v, nt, t);
    if (kind == K_BTG) {
      if (rnd() % 10 == 0) failAt = 1 + rnd() % 8;
      cov[1] = bd[4] >= 30;
      if (neg && isdll) swapw();
      ret[0] = ((F1)fn)((u32)(uintptr_t)bd);
      if (neg && isdll) swapw();
    } else if (kind == K_TDESTROY || kind == K_COUNT) {
      u32 root = rnd() % 16 ? orig_btg((u32)(uintptr_t)bd) : 0;
      cov[1] = root && (int32_t)W(root) >= 0;
      u32 keep = 0, plane = root && (int32_t)W(root) >= 0;
      swapA = plane ? (u8 *)(uintptr_t)(root + 4) : 0; swapB = (u8 *)(uintptr_t)(root + 8);
      if (neg && isdll && plane) { if (kind == K_COUNT) { keep = W(root + 8); W(root + 8) = 0; } else swapw(); }     /* count: right subtree cut; destroy: left <-> right */
      if (kind == K_COUNT) ret[0] = ((F1)fn)(root); else ((F1)fn)(root);      /* BuildTreeDestroy is void: eax is not compared */
      if (neg && isdll && plane) { if (kind == K_COUNT) W(root + 8) = keep; else swapw(); }
    } else if (kind == K_DDESTROY) {
      if (rnd() % 4 == 0) bd[1 + rnd() % 2] = 0; if (rnd() % 4 == 0) bd[14] = 0;
      swapA = (u8 *)(bd + 1); swapB = (u8 *)(bd + 2);
      if (rnd() % 16 == 0) bd = 0;
      if (neg && isdll) swapw();
      ((F1)fn)((u32)(uintptr_t)bd);
      if (neg && isdll) swapw();
    } else {
      u32 i, axis = 4 * (rnd() % 3), *po = (u32 *)(uintptr_t)bd[2], *vi = (u32 *)(uintptr_t)bd[1], first = rnd() % 3 ? 0 : rnd() % nt;
      float value = rnd() % 2 ? v[3 * (rnd() % nv) + axis / 4] : coord();
      bd[12] = first; bd[11] = nt - first - (nt - first > 1 ? rnd() % (nt - first) : 0);
      for (i = 0; i < nv; i++) { float d = v[3 * i + axis / 4] - value; vi[3 * i + 1] = rnd() % 16 == 0 ? rnd() % 4 : (d <= 0) | (d >= 0) << 1; }
      for (i = 0; i < nt; i++) po[2 * i + 1] = rnd() % 3 ? 1 + rnd() % 2 : rnd();
      if (kind == K_CLIP) {
        u32 *st = halloc(16);
        cov[1] = bd[11] > 8;
        swapA = (u8 *)(po + 2 * first); swapB = (u8 *)(po + 2 * (first + bd[11] - 1)); if (bd[11] < 2) swapA = 0;     /* the first and last polygon's triangles */
        if (neg && isdll) swapw();
        ((F4)fn)((u32)(uintptr_t)bd, axis, *(u32 *)&value, (u32)(uintptr_t)st);
        if (neg && isdll) swapw();
      } else if (kind == K_SORT) {
        swapA = (u8 *)(po + 2 * first + 1); swapB = (u8 *)(po + 2 * first + 3); if (bd[11] < 2) swapA = 0;
        if (neg && isdll) swapw();
        ((F1)fn)((u32)(uintptr_t)bd);
        if (neg && isdll) swapw();
      } else {      /* K_DIV */
        u32 *st = halloc(16); long double r; float lv = coord(), rv = coord();
        st[0] = rnd() % 8 == 0 ? rnd() : 1 + rnd() % 300; st[1] = rnd() % 8 == 0 ? rnd() % 4 : 1 + rnd() % 300; memcpy(st + 2, &lv, 4); memcpy(st + 3, &rv, 4);
        if (rnd() % 8 == 0) { float *bb = (float *)(bd + 5); int k; for (k = 0; k < 6; k++) bb[k] = coord(); }
        swapA = (u8 *)st; swapB = (u8 *)(st + 1); cov[1] = st[0] > 300;
        if (neg && isdll) swapw();
        r = ((L3)fn)((u32)(uintptr_t)bd, (u32)(uintptr_t)st, axis);
        if (neg && isdll) swapw();
        memcpy(ret, &r, 10);
      }
    }
    break; }
  case K_FUZZ: {
    float a = coord(), b = rnd() % 4 ? coord() : a * (float)(rnd() % 5) * 0.25f; long double r;
    if (rnd() % 8 == 0) b = 0.0f;
    if (neg && isdll) { float x = a; a = b; b = x; }
    r = ((L2)fn)(*(u32 *)&a, *(u32 *)&b);
    memcpy(ret, &r, 10); cov[1] = b == 0.0f;
    break; }
  case K_POLYS: case K_PLANES: {
    u32 a = rnd() % 3 ? rnd() % 300 : rnd(), b = rnd(), c = rnd();
    if (rnd() % 10 == 0) failAt = 1;
    if (neg && isdll) { u32 x = b; b = c; c = x; }
    ret[0] = kind == K_POLYS ? ((F2)fn)(b, c) : ((F3)fn)(a, b, c);
    cov[1] = failAt != 0;
    break; }
  }
  ret[2] ^= (u32)cw_now() << 16;
}

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[8192]; int k; for (k = 0; k < 8192; k++) b[k] = 0; }
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  u32 *sp = (u32 *)(uintptr_t)e->ContextRecord->Esp; int k;
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx; stack", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  for (k = 0; k < 24; k++) printf(" %08x", sp[k]);
  printf("\n"); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 60u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ); lg[0] = malloc(4 * LGMAX); lg[1] = malloc(4 * LGMAX);
  if (!heap || !save0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  jmp_patch(0x4ea160, (void *)herr);
  memset(eng0, 0xee, sizeof eng0);
  *(void **)(eng0 + 0x108) = (void *)hmalloc; *(void **)(eng0 + 0x10c) = (void *)hfree;
  W(0x7d2128) = (u32)(uintptr_t)eng;
  bind = (Bind)GetProcAddress(m, "mflrw_colltree_bind");
  if (!bind) { printf("mflrw_colltree_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x4d1990, (void *)0x4d19d0, (void *)0x5d38b0, (void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[2] = {0}; u32 hp0 = 0, ret[2][3];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        cw_set(i & 1 ? 0x27f : 0x7f);
        run_case(kind, side ? orig : dll, !side, neg, seed, ret[side], c);
        cw_set(0x27f);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (!memcmp(ret[0], ret[1], sizeof ret[0]) && hp0 == hp && ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * (ln[0] < LGMAX ? ln[0] : LGMAX)) && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x %08x %08x orig %08x %08x %08x, heap %u / %u, log %u / %u", T[fi].name, i, ret[0][0], ret[0][1], ret[0][2], ret[1][0], ret[1][1], ret[1][2], hp0, hp, ln[0], ln[1]);
        for (k = 0; k < (int)(ln[0] < ln[1] ? ln[0] : ln[1]) && k < (int)LGMAX; k++) if (lg[0][k] != lg[1][k]) { printf(", first log diff at %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k + 4 <= (int)(hp0 < hp ? hp0 : hp) + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first heap diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
