/* rw_tristrip_check.c: bit-exact check of the librw DLL's tristrip kernels (tools/delink/rw_tristrip.txt, librw-mfl capi/tristrip.cpp:
   TriStripMeshGenerate, _rpTriListMeshGenerate, TriStripStripTris, TriStripJoin, TriStripBinEntryArrayCreate, TriStripAddEdge,
   TriStripMarkTriUsed, TriStripFollow) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.
   Made from rw_pixfmt_check.c (same image mapping: MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000).
   Harness heap: RwEngineInstance 0x7d2128 points at a harness engine whose malloc / free / free-list alloc / free (+0x108 / +0x10c / +0x118 /
   +0x11c) bump-allocate from one arena (each block + 16 slack bytes filled with a pattern of the case seed and its offset) and log every call
   with its arguments; RwFreeListCreate 0x4e90b0 / RwFreeListDestroy 0x4e92b0 / RwErrorSet 0x4ea160 are patched to harness loggers.
   _rpMeshHeaderCreate, _rpMeshHeaderForAllMeshes, qsort and _rwerror run as original code on both sides (the DLL reaches them through
   mflrw_tristrip_bind).  The mesh module offset 0x947778 points at a harness block in the engine (serial number, strip free list).
   Each side rebuilds the case from the same seed (inputs and setup objects live in the arena, setup runs the ORIGINAL code), then calls
   its function; compared: the whole used arena incl. slack, the engine block, the call log, the return value.
   Inputs: grid patches (consistent / flipped winding, holes, shuffled), random triangles over small vertex pools, fans; 1..6 materials
   (texture / raster format / alpha / index words drawn so that sort keys tie); vertex base up to 60000.  No degenerate triangles (a
   repeated vertex): the original's tri-list reorder loops forever on them.
   usage: rw_tristrip_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (restored after the call); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_MESH, K_LIST, K_STRIP, K_JOIN, K_BIN, K_EDGE, K_MARK, K_FOLLOW };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_TriStripMeshGenerate", 0x5021e0, K_MESH}, {"rw37__rpTriListMeshGenerate", 0x5038f0, K_LIST}, {"rw37_TriStripStripTris", 0x5027a0, K_STRIP},
  {"rw37_TriStripJoin", 0x503670, K_JOIN}, {"rw37_TriStripBinEntryArrayCreate", 0x5030a0, K_BIN}, {"rw37_TriStripAddEdge", 0x5031c0, K_EDGE},
  {"rw37_TriStripMarkTriUsed", 0x503260, K_MARK}, {"rw37_TriStripFollow", 0x503380, K_FOLLOW}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F2)(u32, u32);
typedef u32 (__cdecl *F3)(u32, u32, u32);
typedef u32 (__cdecl *F4)(u32, u32, u32, u32);
typedef u32 (__cdecl *F5)(u32, u32, u32, u32, u32);
typedef u32 (__cdecl *F6)(u32, u32, u32, u32, u32, u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *, void *, void *, void *);

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

/* ---- harness heap + call log */
#define HSZ (16u << 20)
#define LGMAX (1u << 21)
static u8 *heap, *save0; static u32 hp, nalloc, failAt, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static u8 eng[0x400], eng0[0x400], engs[0x400];
#define MODOFF 0x300
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static void *__cdecl hmalloc(u32 n, u32 hint) { logw(0xa1100000u); logw(n); logw(hint); if (++nalloc == failAt) { logw(0); return 0; } { void *p = halloc(n); logw((u32)(uintptr_t)p); return p; } }
static void __cdecl hfree(void *p) { logw(0xa1200000u); logw((u32)(uintptr_t)p); }
static void *__cdecl hflalloc(u32 *fl, u32 hint) { logw(0xa1300000u); logw((u32)(uintptr_t)fl); logw(hint); if (++nalloc == failAt) { logw(0); return 0; } { void *p = halloc(fl[1]); logw((u32)(uintptr_t)p); return p; } }
static void *__cdecl hflfree(void *fl, void *p) { logw(0xa1400000u); logw((u32)(uintptr_t)fl); logw((u32)(uintptr_t)p); return fl; }
static void *__cdecl hflcreate(u32 size, u32 n, u32 align, u32 hint) { u32 *fl = halloc(8); fl[0] = 0xf1f1f1f1u; fl[1] = size; logw(0xa1500000u); logw(size); logw(n); logw(align); logw(hint); logw((u32)(uintptr_t)fl); return fl; }
static void __cdecl hfldestroy(void *fl) { logw(0xa1600000u); logw((u32)(uintptr_t)fl); }
static void *__cdecl herr(int32_t *e) { logw(0xa1700000u); logw(e[0]); logw(e[1]); return e; }

/* ---- inputs */
static u32 nverts_base;
static u16 vtx(u32 v) { return (u16)(nverts_base + v); }
/* RpBuildMeshTriangle: v[3], pad, material, matIndex, texIndex, rasIndex, pipeIndex */
static u8 *mats[8]; static int nmats;
static void gen_materials(void) {
  int k; nmats = 1 + rnd() % 6;
  for (k = 0; k < nmats; k++) {
    u8 *m = halloc(0x20), *tex, *ras;
    if (rnd() % 8 == 0) *(u32 *)m = 0;
    else { tex = halloc(0x10); *(u32 *)m = (u32)(uintptr_t)tex; ras = halloc(0x30); *(u32 *)tex = (u32)(uintptr_t)ras; ras[0x23] = (u8)(rnd() & 0xf0) | (u8)(rnd() % 7); }      /* a texture always has a raster (the original does not test it) */
    m[7] = rnd() % 4 ? 0xff : (u8)rnd();
    mats[k] = m;
  }
}
static void set_tri(u8 *t, u16 a, u16 b, u16 c) {
  u32 r = rnd() % 16;
  *(u16 *)t = a; *(u16 *)(t + 2) = b; *(u16 *)(t + 4) = c;
  *(u32 *)(t + 8) = r == 0 ? 0 : (u32)(uintptr_t)mats[(rnd() % (nmats * 2)) % nmats];
  *(u16 *)(t + 0xc) = rnd() % 3; *(u16 *)(t + 0xe) = rnd() % 3; *(u16 *)(t + 0x10) = rnd() % 3; *(u16 *)(t + 0x12) = rnd() % 3;
}
/* n triangles into tris (room for max); returns the count */
static u32 gen_tris(u8 *tris, u32 max) {
  u32 n = 0, parts = 1 + rnd() % 3, p;
  nverts_base = rnd() % 8 == 0 ? rnd() % 60000 : rnd() % 4 == 0 ? rnd() % 300 : 0;
  for (p = 0; p < parts && n < max; p++) {
    u32 mode = rnd() % 4, base = rnd() % 40;
    if (mode <= 1) {          /* grid patch */
      u32 w = 1 + rnd() % 10, h = 1 + rnd() % 8, x, y, flipall = rnd() % 2, holes = rnd() % 3;
      for (y = 0; y < h; y++) for (x = 0; x < w; x++) {
        u16 a = vtx(base + y * (w + 1) + x), b = vtx(base + y * (w + 1) + x + 1), c = vtx(base + (y + 1) * (w + 1) + x), d = vtx(base + (y + 1) * (w + 1) + x + 1);
        int fl = flipall ^ (rnd() % 16 == 0);
        if (holes && rnd() % 6 == 0) continue;
        if (n < max) { if (fl) set_tri(tris + 20 * n, a, b, c); else set_tri(tris + 20 * n, a, c, b); n++; }
        if (n < max) { if (fl) set_tri(tris + 20 * n, c, b, d); else set_tri(tris + 20 * n, c, d, b); n++; }
      }
    } else if (mode == 2) {   /* random triangles over a small pool */
      u32 pool = 3 + rnd() % 30, k, cnt = 1 + rnd() % 40;
      for (k = 0; k < cnt && n < max; k++) { u32 a = rnd() % pool, b, c; do b = rnd() % pool; while (b == a); do c = rnd() % pool; while (c == a || c == b); set_tri(tris + 20 * n, vtx(base + a), vtx(base + b), vtx(base + c)); n++; }
    } else {                  /* fan */
      u32 cnt = 1 + rnd() % 12, k;
      for (k = 0; k < cnt && n < max; k++) { set_tri(tris + 20 * n, vtx(base), vtx(base + 1 + k), vtx(base + 2 + k)); n++; }
    }
  }
  if (n > 1 && rnd() % 2) { u32 k; for (k = n - 1; k > 0; k--) { u32 j = rnd() % (k + 1); u8 t[20]; memcpy(t, tris + 20 * k, 20); memcpy(tris + 20 * k, tris + 20 * j, 20); memcpy(tris + 20 * j, t, 20); } }
  if (n == 0) { set_tri(tris, vtx(0), vtx(1), vtx(2)); n = 1; }
  return n;
}
#define MAXT 420

/* the setup of StripTris (bins from the adjacency counts), harness side */
static void fill_bins(u32 *entries, u32 n, u32 *bins) {
  u32 i; for (i = 0; i < 4; i++) bins[i] = 0;
  for (i = 0; i < n; i++) { u8 *t = (u8 *)(uintptr_t)entries[i]; u32 a = t[0x20]; *(u32 *)(t + 0x10) = bins[a]; if (bins[a]) *(u32 *)((uintptr_t)bins[a] + 0x14) = (u32)(uintptr_t)t; bins[a] = (u32)(uintptr_t)t; *(u32 *)(t + 0x14) = 0; }
}
static u32 strip_new(u32 count, u32 cap, const u16 *v) {
  u32 *s = halloc(16), k; u16 *ix = halloc(cap * 4);
  s[0] = (u32)(uintptr_t)ix; s[1] = count; s[2] = cap; s[3] = 0; for (k = 0; k < count; k++) ix[k] = v[k];
  return (u32)(uintptr_t)s;
}

static u8 *swapA, *swapB;     /* negative fixture: the two input words */
static void swapw(void) { u16 t; if (!swapA) return; t = *(u16 *)swapA; *(u16 *)swapA = *(u16 *)swapB; *(u16 *)swapB = t; }

/* build case `seed` for entry kind and run fn; returns the result word */
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, n, k, *bm, *list, *fls, *head, *bins, ents, *E; u8 *tris; int (*orig_bin)(u32, u32, u32, u32) = (void *)0x5030a0; void (*orig_mark)(u32, u32, u32) = (void *)0x503260;
  rs = seed; hseed = seed * 2654435761u; hp = 0; nalloc = 0; failAt = 0; ln[side] = 0; swapA = swapB = 0;
  memcpy(eng, eng0, sizeof eng);
  *(u16 *)(eng + MODOFF) = (u16)rnd(); *(u32 *)(eng + MODOFF + 4) = 0;
  gen_materials();
  tris = halloc(MAXT * 20); n = gen_tris(tris, rnd() % 6 == 0 ? MAXT : rnd() % 4 == 0 ? 120 : 30);
  swapA = tris; swapB = tris + 2;
  switch (kind) {
  case K_MESH: case K_LIST: {
    u32 a1 = rnd() % 2, a2 = rnd() % 2;
    bm = halloc(12); bm[0] = n; bm[1] = n; bm[2] = (u32)(uintptr_t)tris;
    if (rnd() % 10 == 0) failAt = 1 + rnd() % (kind == K_LIST ? 4 : 1);
    cov[0] = failAt != 0; cov[1] = n > 100;
    if (neg && isdll) swapw();
    r = kind == K_MESH ? ((F3)fn)((u32)(uintptr_t)bm, a1, a2) : ((F2)fn)((u32)(uintptr_t)bm, a1);
    if (neg && isdll) swapw();
    break; }
  case K_STRIP: {
    u32 tryAll = rnd() % 2;
    if (rnd() % 20 == 0) n = 0;
    *(u32 *)(eng + MODOFF + 4) = (u32)(uintptr_t)hflcreate(0x10, 5, 4, 0x10502);
    list = halloc(4); *list = rnd() % 4 ? 0 : strip_new(3, 3, (u16 *)tris);
    cov[0] = tryAll; cov[1] = n > 100;
    if (neg && isdll) swapw();
    r = ((F4)fn)((u32)(uintptr_t)tris, n, (u32)(uintptr_t)list, tryAll);
    if (neg && isdll) swapw();
    break; }
  case K_JOIN: {
    u32 ns = rnd() % 7, flag = rnd() % 2, pool = 2 + rnd() % 6;
    *(u32 *)(eng + MODOFF + 4) = (u32)(uintptr_t)hflcreate(0x10, 5, 4, 0x10502);
    list = halloc(4); *list = 0;
    for (k = 0; k < ns; k++) { u16 v[16]; u32 c = 2 + rnd() % 14, j, *s; for (j = 0; j < c; j++) v[j] = (u16)(rnd() % pool); s = hflalloc((u32 *)(uintptr_t)*(u32 *)(eng + MODOFF + 4), 0x30502); s[0] = (u32)(uintptr_t)halloc(c * 2); s[1] = c; s[2] = c; memcpy((void *)(uintptr_t)s[0], v, c * 2); s[3] = *list; *list = (u32)(uintptr_t)s; }
    if (ns) { u32 *s = (u32 *)(uintptr_t)*list; swapA = (u8 *)(uintptr_t)s[0]; swapB = swapA + 2; } else swapA = 0;
    cov[0] = ns == 0; cov[1] = flag;
    if (neg && isdll) swapw();
    r = ((F2)fn)((u32)(uintptr_t)list, flag);
    if (neg && isdll) swapw();
    break; }
  case K_BIN:
    fls = halloc(8); head = halloc(4); *head = 0;
    cov[0] = n > 100; cov[1] = 0;
    if (neg && isdll) swapw();
    r = ((F4)fn)(n, (u32)(uintptr_t)fls, (u32)(uintptr_t)head, (u32)(uintptr_t)tris);
    if (neg && isdll) swapw();
    break;
  case K_EDGE: case K_MARK: case K_FOLLOW: {
    fls = halloc(8); head = halloc(4); *head = 0; bins = halloc(16);
    ents = orig_bin(n, (u32)(uintptr_t)fls, (u32)(uintptr_t)head, (u32)(uintptr_t)tris); E = (u32 *)(uintptr_t)ents;
    if (kind == K_EDGE) {
      u32 i = rnd() % n, t = rnd() % n, e = rnd() % 3; u16 *v = (u16 *)(tris + 20 * t); u32 a = v[e], b = v[(e + 1) % 3], m = rnd() % 5; u32 args[2];
      if (m < 3) { args[0] = b; args[1] = a; } else if (m == 3) { args[0] = a; args[1] = b; } else { args[0] = vtx(rnd() % 40); args[1] = vtx(rnd() % 40); }
      args[0] |= rnd() << 16; args[1] |= rnd() << 16;      /* garbage upper halves as in the original's pushes */
      cov[0] = m < 3; cov[1] = 0;
      if (neg && isdll) { u32 t2 = args[0]; args[0] = args[1]; args[1] = t2; }
      r = ((F6)fn)(fls[1], (u32)(uintptr_t)head, args[0], args[1], i, ents);
      break;
    }
    fill_bins(E, n, bins);
    for (k = rnd() % (n + 1); k; k--) { u8 *t = (u8 *)(uintptr_t)E[rnd() % n]; if (!*(u32 *)(t + 0x18)) orig_mark((u32)(uintptr_t)t, (u32)(uintptr_t)bins, 4); }
    for (k = 0; k < n; k++) { u8 *t = (u8 *)(uintptr_t)E[k]; *(u32 *)(t + 0x1c) = rnd() % 3 ? *(u32 *)(t + 0x18) : rnd() % 2; }
    { u32 tries = 0, ti; u8 *t; int pass = rnd() % 2 ? 4 + rnd() % 3 : rnd() % 4;
      do { ti = rnd() % n; t = (u8 *)(uintptr_t)E[ti]; } while (*(u32 *)(t + 0x18) && ++tries < 64);
      if (*(u32 *)(t + 0x18)) pass = rnd() % 4;
      swapA = t + 4; swapB = t + 8;       /* edge[0] <-> edge[1] (u16 halves of both pointers swapped would break them: swap whole words) */
      cov[0] = pass >= 4; cov[1] = 0;
      if (kind == K_MARK) {
        if (neg && isdll) { u32 w = *(u32 *)swapA; *(u32 *)swapA = *(u32 *)swapB; *(u32 *)swapB = w; }
        ((F3)fn)((u32)(uintptr_t)t, (u32)(uintptr_t)bins, (u32)pass); r = 0;
        if (neg && isdll) { u32 w = *(u32 *)swapA; *(u32 *)swapA = *(u32 *)swapB; *(u32 *)swapB = w; }
      } else {
        u32 s = rnd() % 3, cap = 2 * n + 2, st, edge; u16 *v = (u16 *)(tris + 20 * *(u32 *)t), sv[3]; u32 mode = rnd() % 4;
        if (pass < 4) for (k = 0; k < n; k++) *(u32 *)((uintptr_t)E[k] + 0x1c) = *(u32 *)((uintptr_t)E[k] + 0x18);
        orig_mark((u32)(uintptr_t)t, (u32)(uintptr_t)bins, (u32)pass);
        sv[0] = v[s]; sv[1] = v[(s + 1) % 3]; sv[2] = v[(s + 2) % 3];
        if (mode == 0) { u16 bv[2]; bv[0] = sv[1]; bv[1] = sv[0]; st = strip_new(2, cap, bv); edge = *(u32 *)(t + 4 + 4 * s); }
        else if (mode == 3 && rnd() % 4 == 0) { st = strip_new(3, cap, sv); edge = 0; }
        else { st = strip_new(3, cap, sv); edge = *(u32 *)(t + 4 + 4 * ((s + 1) % 3)); }
        swapA = (u8 *)(uintptr_t)*(u32 *)(uintptr_t)st; swapB = swapA + 2;   /* the strip's first two indices */
        cov[1] = mode == 0;
        if (neg && isdll) swapw();
        r = ((F5)fn)(st, edge, (u32)(uintptr_t)bins, (u32)(uintptr_t)tris, (u32)pass);
        if (neg && isdll) swapw();
      }
    }
    break; }
  }
  return r;
}

/* zero the stack below the caller (the region the callee's frame uses) */
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
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 40u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ); lg[0] = malloc(4 * LGMAX); lg[1] = malloc(4 * LGMAX);
  if (!heap || !save0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  jmp_patch(0x4e90b0, (void *)hflcreate); jmp_patch(0x4e92b0, (void *)hfldestroy); jmp_patch(0x4ea160, (void *)herr);
  memset(eng0, 0xee, sizeof eng0);
  *(void **)(eng0 + 0x108) = (void *)hmalloc; *(void **)(eng0 + 0x10c) = (void *)hfree; *(void **)(eng0 + 0x118) = (void *)hflalloc; *(void **)(eng0 + 0x11c) = (void *)hflfree;
  W(0x7d2128) = (u32)(uintptr_t)eng; W(0x947778) = MODOFF;
  bind = (Bind)GetProcAddress(m, "mflrw_tristrip_bind");
  if (!bind) { printf("mflrw_tristrip_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x947778, (void *)0x4e90b0, (void *)0x4e92b0, (void *)0x500ba0, (void *)0x500fb0, (void *)0x5d38b0, (void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[4] = {0}; u32 hp0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        ret[side] = run_case(kind, side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16 <= HSZ ? hp + 16 : hp); memcpy(engs, eng, sizeof eng); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (ret[0] == ret[1] && hp0 == hp && ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * (ln[0] < LGMAX ? ln[0] : LGMAX)) && !memcmp(save0, heap, hp + 16) && !memcmp(engs, eng, sizeof eng)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap %u / %u, log %u / %u", T[fi].name, i, ret[0], ret[1], hp0, hp, ln[0], ln[1]);
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
