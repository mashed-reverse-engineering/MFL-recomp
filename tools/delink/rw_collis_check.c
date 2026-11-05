/* rw_collis_check.c: bit-exact check of the librw DLL's collision kernels (tools/delink/rw_collis.txt, librw-mfl capi/collis.cpp) against the ORIGINAL code of
   MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_g2_check.c (same image mapping: this exe is linked at 0x400000 and MFL.exe's sections are copied
   into a bss block that spans 0x4b0000..0x94b000, so the original functions run in place with their internal calls: _rwInvSqrt 0x4d5470,
   RtIntersectionSphereTriangle 0x557a80, RtIntersectionBBoxTriangle 0x558220).
   usage: rw_collis_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name] [neg]
   Per entry point N finite cases (one query each on random worlds / geometries: plane trees, sectors with and without collision trees, kd trees with and
   without a map, single-leaf trees, degenerate triangles; lines incl. axis-parallel, zero-length and aimed through triangles from both sides; spheres and
   boxes around triangles) + N/5 special cases (random bits; reported, not counted).  x87 CW 0x7f (the game's), MXCSR 0x9fc0.
   Compared per query: the callback sequence (intersection, sector, the RpCollisionTriangle's normal / point / index / vertex pointers, the distance bits,
   data), early exit after the k-th callback (callbacks return NULL there), the world bytes after the query, the control word.  Exit 1 on any finite DIFF.
   neg = negative fixture (negtest.sh): the DLL's line entries get the line with start and end swapped (a kernel that tests the segment the other way
   round, i.e. the back faces); the check must report DIFF for the two line entry points. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
#define SOFF 0x40      /* collision plugin data offset in a world sector (the exe's _rpCollisionWorldSectorDataOffset 0x7da6f4 is set to it) */
#define GOFF 0x70      /* ... in a geometry (_rpCollisionGeometryDataOffset 0x7da6f0) */
typedef void (__cdecl *WLine)(void *world, void *is, void *cb, void *data);
typedef void (__cdecl *WSph)(void *world, void *sphere, void *is, void *cb, void *data);
typedef void (__cdecl *GLine)(void *geom, void *line, void *is, void *cb, void *data);
typedef void (__cdecl *Bind)(int32_t *, int32_t *);
static const struct { const char *name; u32 va; int kind; } T[] = {      /* kind: 0 world line, 1 world box, 2 world sphere, 3 geometry line */
  {"rw37_WorldForAllTriIntsecLine", 0x549370, 0}, {"rw37_WorldForAllTriIntsecBBox", 0x549f10, 1},
  {"rw37_WorldForAllTriIntsecSphere", 0x54a4d0, 2}, {"rw37_GeometryForAllTriIntsecLine", 0x54adb0, 3}};
static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float spf(float f) {      /* in special mode a few values become inf / NaN / random bits */
  u32 r, b; if (!special || rnd() % 50) return f;
  r = rnd() % 4; b = r == 0 ? 0x7f800000 : r == 1 ? 0xff800000 : r == 2 ? 0x7fc00000 : rnd(); memcpy(&f, &b, 4); return f;
}
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static void *__cdecl hmalloc(size_t n, u32 hint) { (void)hint; return calloc(1, n + 64); }
static void __cdecl hfree(void *p) { free(p); }
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

/* ---- world / geometry arena (RW 3.7 layouts as the kernels read them) */
#define ASZ (1u << 22)
static u8 *arena, *snap;      /* heap (a static block could push mfl_area off 0x4b0000) */
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; memset(p, 0, n); an += n; return p; }
static float scale;      /* world size of the current world */
static float *alltv[4096]; static u16 *alltt[4096]; static int allnt[4096], nall;

/* kd tree: entries in leaf order (left first), splits in preorder (root = split 0) */
static u16 ent[512]; static int nent; static u8 *splits; static int nspl, deep, maxdepth;      /* deep: skewed splits, trees up to 30 levels (the stacks hold 33) */
static float cen(const float *v, const u16 *t, int a) { return v[3 * t[0] + a] + v[3 * t[1] + a] + v[3 * t[2] + a]; }
static int cmp_a; static const float *cmp_v; static const u16 *cmp_t;
static int cmpc(const void *x, const void *y) { float p = cen(cmp_v, cmp_t + 4 * *(const u16 *)x, cmp_a), q = cen(cmp_v, cmp_t + 4 * *(const u16 *)y, cmp_a); return p < q ? -1 : p > q; }
static void kd(u8 *node, u16 *ids, int n, const float *v, const u16 *t, int depth) {
  int s, a, k, i, j; float lv = -1e30f, rv = 1e30f;
  if (depth > maxdepth && depth < 99) maxdepth = depth;
  if (n < 4 || (n < 240 && (depth >= (deep ? 30 : 14) || rnd() % (deep ? 40 : 6) == 0))) { node[1] = (u8)n; *(u16 *)(node + 2) = (u16)nent; for (i = 0; i < n; i++) ent[nent++] = ids[i]; node[0] = (u8)(rnd() % 3 * 4); return; }
  s = nspl++; node[1] = 0xff; *(u16 *)(node + 2) = (u16)s;
  a = rnd() % 3; cmp_a = a; cmp_v = v; cmp_t = t; qsort(ids, n, 2, cmpc);
  k = deep && depth < 22 && rnd() % 2 ? 1 + (int)(rnd() % 3) : n / 2 + (int)(rnd() % 3) - 1; if (k < 1) k = 1; if (k > n - 1) k = n - 1;
  for (i = 0; i < k; i++) for (j = 0; j < 3; j++) { float c = v[3 * t[4 * ids[i] + j] + a]; if (c > lv) lv = c; }
  for (i = k; i < n; i++) for (j = 0; j < 3; j++) { float c = v[3 * t[4 * ids[i] + j] + a]; if (c < rv) rv = c; }
  if (rnd() % 8 == 0) lv += uf(-0.1f, 0.1f) * scale; if (rnd() % 8 == 0) rv += uf(-0.1f, 0.1f) * scale;
  splits[16 * s] = (u8)(a * 4 | 1); *(float *)(splits + 16 * s + 4) = lv;
  splits[16 * s + 8] = (u8)(a * 4); *(float *)(splits + 16 * s + 12) = rv;
  kd(splits + 16 * s, ids, k, v, t, depth + 1);
  kd(splits + 16 * s + 8, ids + k, n - k, v, t, depth + 1);
}
/* a collision tree for nt triangles (u16 vi[3], u16 mat) over v; map or (with *tris reordered) no map; NULL sometimes */
static u8 *mktree(u16 **tris, int nt, const float *v, int nv) {
  u8 *tr, root[8]; u16 ids[512]; int i, usemap = rnd() % 2;
  if (rnd() % 8 == 0) return NULL;
  splits = alloc(16 * 512); nspl = 0; nent = 0;
  for (i = 0; i < nt; i++) ids[i] = (u16)i;
  if (nt == 0) { root[1] = 0; *(u16 *)(root + 2) = 0; } else kd(root, ids, nt, v, *tris, nt < 4 ? 99 : 0);
  tr = alloc(0x28);
  { float lo[3] = {1e30f, 1e30f, 1e30f}, hi[3] = {-1e30f, -1e30f, -1e30f}; int a;
    for (i = 0; i < nv; i++) for (a = 0; a < 3; a++) { if (v[3 * i + a] < lo[a]) lo[a] = v[3 * i + a]; if (v[3 * i + a] > hi[a]) hi[a] = v[3 * i + a]; }
    if (nv == 0) for (a = 0; a < 3; a++) { lo[a] = 0; hi[a] = 0; }
    for (a = 0; a < 3; a++) { *(float *)(tr + 4 + 4 * a) = hi[a] + (rnd() % 4 == 0 ? uf(-0.05f, 0.05f) * scale : 0); *(float *)(tr + 0x10 + 4 * a) = lo[a] + (rnd() % 4 == 0 ? uf(-0.05f, 0.05f) * scale : 0); } }
  *(u16 *)(tr + 0x1c) = (u16)nt; *(u16 *)(tr + 0x1e) = (u16)nspl;
  *(u8 **)(tr + 0x20) = nspl ? splits : NULL;
  if (nspl == 0 && nt > 0 && root[1] != 0xff) { /* single leaf: index 0, contents numEntries */ }
  if (usemap) { u16 *m = alloc(2 * (nt + 1)); for (i = 0; i < nent; i++) m[i] = ent[i]; *(u16 **)(tr + 0x24) = m; *(u32 *)tr = 1; }
  else { u16 *nt2 = alloc(8 * (nt + 1)); for (i = 0; i < nent; i++) memcpy(nt2 + 4 * i, *tris + 4 * ent[i], 8); *tris = nt2; }
  return tr;
}
/* vertices + triangles in a box */
static void mkmesh(const float *lo, const float *hi, float **pv, int *pnv, u16 **pt, int *pnt) {
  int nv = 3 + rnd() % 40, nt = rnd() % 70, i, a; float *v; u16 *t;
  if (rnd() % 10 == 0) nt = rnd() % 4;
  if (deep && rnd() % 3 == 0) { nv = 20 + rnd() % 200; nt = 100 + rnd() % 300; }
  v = alloc(12 * nv); t = alloc(8 * (nt + 1));
  for (i = 0; i < nv; i++) for (a = 0; a < 3; a++) v[3 * i + a] = spf(uf(lo[a], hi[a]) + (rnd() % 8 == 0 ? uf(-0.1f, 0.1f) * scale : 0));
  if (rnd() % 6 == 0 && nv > 3) { int b = rnd() % (nv - 1); for (a = 0; a < 3; a++) v[3 * (b + 1) + a] = v[3 * b + a] + (rnd() % 2 ? 0 : uf(-1e-4f, 1e-4f)); }      /* near-duplicate vertex */
  for (i = 0; i < nt; i++) { int k; for (k = 0; k < 3; k++) t[4 * i + k] = (u16)(rnd() % nv); t[4 * i + 3] = (u16)(rnd() % 7);
    if (rnd() % 5 == 0) { int c = rnd() % nv; t[4 * i] = (u16)c; t[4 * i + 1] = (u16)((c + 1) % nv); t[4 * i + 2] = (u16)((c + 2) % nv); } }
  *pv = v; *pnv = nv; *pt = t; *pnt = nt;
}
static int sectors;
static u8 *mkplane(float *lo, float *hi, int depth) {
  if (depth >= 7 || rnd() % 4 == 0 || sectors > 60) {      /* world sector: type < 0, triangles +4, vertices +8, collision tree at +SOFF */
    u8 *s = alloc(SOFF + 8); float *v; u16 *t; int nv, nt;
    *(int32_t *)s = -1 - (int)(rnd() % 3); sectors++;
    mkmesh(lo, hi, &v, &nv, &t, &nt);
    *(u8 **)(s + SOFF) = mktree(&t, nt, v, nv);
    *(u16 **)(s + 4) = t; *(float **)(s + 8) = v;
    if (nall < 4095) { alltv[nall] = v; alltt[nall] = t; allnt[nall] = nt; nall++; }
    return s;
  } else {      /* plane sector {type, value, left, right, leftValue, rightValue} */
    u8 *p = alloc(0x18); int a = rnd() % 3; float m = uf(lo[a], hi[a]), lv = m + uf(0, 0.1f) * (hi[a] - lo[a]), rv = m - uf(0, 0.1f) * (hi[a] - lo[a]), save;
    if (rnd() % 10 == 0) { float x = lv; lv = rv; rv = x; }
    *(int32_t *)p = a * 4; *(float *)(p + 4) = m; *(float *)(p + 0x10) = spf(lv); *(float *)(p + 0x14) = spf(rv);
    save = hi[a]; hi[a] = lv; *(u8 **)(p + 8) = mkplane(lo, hi, depth + 1); hi[a] = save;
    save = lo[a]; lo[a] = rv; *(u8 **)(p + 0xc) = mkplane(lo, hi, depth + 1); lo[a] = save;
    return p;
  }
}
static u8 *world, *geom; static float glo[3], ghi[3];
static void mkworld(void) {
  float lo[3], hi[3]; int a; float *v; u16 *t; int nv, nt;
  an = 0; sectors = 0; nall = 0; deep = rnd() % 3 == 0;
  scale = (float)ldexp(1.0, (int)(rnd() % 14) - 3);
  for (a = 0; a < 3; a++) { lo[a] = uf(-1, 0) * scale; hi[a] = lo[a] + uf(0.2f, 2) * scale; glo[a] = lo[a]; ghi[a] = hi[a]; }
  world = alloc(0x40); *(u8 **)(world + 0x1c) = mkplane(lo, hi, 0);
  geom = alloc(GOFF + 8);      /* geometry: numTriangles +0x10, triangles +0x2c, morphTarget +0x5c (vertices +0x14), tree at +GOFF */
  mkmesh(lo, hi, &v, &nv, &t, &nt);
  *(u8 **)(geom + GOFF) = mktree(&t, nt, v, nv);
  *(int32_t *)(geom + 0x10) = nt; *(u16 **)(geom + 0x2c) = t;
  { u8 *mt = alloc(0x20); *(float **)(mt + 0x14) = v; *(u8 **)(geom + 0x5c) = mt; }
  alltv[nall] = v; alltt[nall] = t; allnt[nall] = nt; nall++;
}
/* a point on a random triangle of the world (barycentric, may leave it a little) */
static void tripoint(float *p, int geo) {
  int k = geo ? nall - 1 : (int)(rnd() % (nall > 1 ? nall - 1 : 1)), a; float *v = alltv[k]; u16 *t = alltt[k]; int nt = allnt[k];
  if (!t || nt == 0) { for (a = 0; a < 3; a++) p[a] = uf(glo[a], ghi[a]); return; }
  { int i = rnd() % nt; float u = uf(-0.1f, 1.0f), w = uf(-0.1f, 1.0f - u); u16 *q = t + 4 * i;
    if (rnd() % 3 == 0) { static const float e[] = {0, 0.5f, 1}; u = e[rnd() % 3]; w = u == 1 ? 0 : e[rnd() % 2]; }      /* vertices / edges: ties */
    for (a = 0; a < 3; a++) p[a] = v[3 * q[0] + a] + u * (v[3 * q[1] + a] - v[3 * q[0] + a]) + w * (v[3 * q[2] + a] - v[3 * q[0] + a]); }
}

/* ---- callback log */
static u32 *lg[2]; static int ln[2], side, stopat, ncall, overflow;
static void logw(u32 x) { if (ln[side] < (1 << 16)) lg[side][ln[side]++] = x; else overflow = 1; }
static u32 fbits(float f) { u32 b; memcpy(&b, &f, 4); return b; }
static void *__cdecl wcb(void *is, void *sector, void *tri, float dist, void *data) {
  u32 *t = tri; int i;
  logw((u32)(uintptr_t)is); logw(ncall); logw((u32)(uintptr_t)sector); for (i = 0; i < 10; i++) logw(t[i]); logw(fbits(dist)); logw((u32)(uintptr_t)data); logw(cw_now());
  return ++ncall == stopat ? NULL : tri;
}
static void *__cdecl gcb(void *is, void *tri, float dist, void *data) { return wcb(is, (void *)0x5ec7, tri, dist, data); }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; unsigned char *eng; Bind bind;
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); lg[0] = malloc(4 << 16); lg[1] = malloc(4 << 16); if (!arena || !snap || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  *(unsigned char *)0x4ea160 = 0xc3; memcpy((void *)0x53eac0, "\x31\xc0\xc3", 3);      /* RwError helpers of the error paths */
  eng = calloc(1, 0x10000); W(0x7d2128) = (u32)(uintptr_t)eng; W((u32)(uintptr_t)eng + 0x108) = (u32)(uintptr_t)hmalloc; W((u32)(uintptr_t)eng + 0x10c) = (u32)(uintptr_t)hfree;
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  ((void (__cdecl *)(u32, u32, u32))0x4d5770)((u32)(uintptr_t)eng, 0x400, 0x18);      /* math ctor: the _rwInvSqrt table */
  W(0x7da6f4) = SOFF; W(0x7da6f0) = GOFF;
  bind = (Bind)GetProcAddress(m, "mflrw_collis_bind");
  if (!bind) { printf("mflrw_collis_bind MISSING export\n"); return 1; }
  bind((int32_t *)0x7da6f4, (int32_t *)0x7da6f0);
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: line entries get start/end swapped" : "");
  for (fi = 0; fi < 4; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0, badsp = 0, calls = 0, hitq = 0, stops = 0;
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-34s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u8 is[0x1c] __attribute__((aligned(4))), is2[0x1c]; float *f = (float *)is, sph[4]; int a, kind = T[fi].kind, ok, s;
      special = i >= N;
      if (i % 40 == 0) mkworld();
      memset(is, 0, sizeof is);
      if (kind == 0 || kind == 3) {      /* line: start f[0..2], end f[3..5] */
        int how = rnd() % 8;
        for (a = 0; a < 3; a++) { f[a] = uf(glo[a], ghi[a]) * 1.2f; f[3 + a] = uf(glo[a], ghi[a]) * 1.2f; }
        if (how <= 2) { float p[3]; tripoint(p, kind == 3); for (a = 0; a < 3; a++) { float d = (f[3 + a] - f[a]) * uf(0.2f, 1.0f); f[a] = p[a] - d * uf(0, 1); f[3 + a] = f[a] + d; } }
        if (how == 3) { int k = rnd() % 3; f[3 + k] = f[k]; if (rnd() % 2) f[3 + (k + 1) % 3] = f[(k + 1) % 3]; }      /* axis-parallel */
        if (how == 4) { float p[3]; tripoint(p, kind == 3); f[0] = f[3] = p[0]; f[2] = f[5] = p[2]; f[1] = p[1] + 3 * scale / 64; f[4] = f[1] - 6 * scale / 64; }      /* ground probe */
        if (how == 5 && rnd() % 4 == 0) for (a = 0; a < 3; a++) f[3 + a] = f[a];      /* zero length */
        for (a = 0; a < 6; a++) f[a] = spf(f[a]);
        *(int32_t *)(is + 0x18) = 1;
      } else if (kind == 1) {      /* box sup f[0..2], inf f[3..5] */
        float p[3]; tripoint(p, 0);
        for (a = 0; a < 3; a++) { float h = uf(0, 0.3f) * scale * (rnd() % 4 ? 0.2f : 1.0f); f[a] = p[a] + h * uf(0.2f, 1); f[3 + a] = p[a] - h * uf(0.2f, 1); if (rnd() % 20 == 0) { float x = f[a]; f[a] = f[3 + a]; f[3 + a] = x; } }
        if (rnd() % 3 == 0) for (a = 0; a < 6; a++) if (rnd() % 2) { float q[3]; tripoint(q, 0); f[a] = q[a % 3]; }      /* faces through vertices: ties with split values and tree boxes */
        for (a = 0; a < 6; a++) f[a] = spf(f[a]);
        *(int32_t *)(is + 0x18) = 4;
      } else {      /* sphere (the dispatcher passes &is->t.sphere, or the atomic's bounding sphere with is of type 5) */
        tripoint(f, 0); f[3] = uf(0, 0.25f) * scale * (rnd() % 3 ? 0.3f : 1.0f); if (rnd() % 50 == 0) f[3] = 0;
        for (a = 0; a < 4; a++) f[a] = spf(f[a]);
        *(int32_t *)(is + 0x18) = 3; memcpy(sph, f, 16);
      }
      (void)sph; memcpy(is2, is, sizeof is);
      memcpy(snap, arena, an);
      s = rnd() % 4 ? 0 : 1 + rnd() % 6;
      for (side = 0; side < 2; side++) {      /* both sides get the same intersection buffer (its address is logged), restored before each */
        void *fn = side ? orig : dll; u8 *isp = is;
        memcpy(is, is2, sizeof is);
        ln[side] = 0; ncall = 0; stopat = s; overflow = 0;
        cw_set(0x7f);
        if (kind == 0) {
          if (neg && !side) { u8 sw[0x1c]; memcpy(sw, isp, 0x1c); memcpy(sw, isp + 12, 12); memcpy(sw + 12, isp, 12); ((WLine)fn)(world, sw, wcb, (void *)0xda7a); }
          else ((WLine)fn)(world, isp, wcb, (void *)0xda7a);
        } else if (kind == 1) ((WLine)fn)(world, isp, wcb, (void *)0xda7a);
        else if (kind == 2) ((WSph)fn)(world, isp, isp, wcb, (void *)0xda7a);
        else {
          if (neg && !side) { float sw[6]; memcpy(sw, isp + 12, 12); memcpy(sw + 3, isp, 12); ((GLine)fn)(geom, sw, isp, gcb, (void *)0xda7a); }
          else ((GLine)fn)(geom, isp, isp, gcb, (void *)0xda7a);
        }
        logw(0xc0de0000u | cw_now()); cw_set(0x7f);
        if (memcmp(snap, arena, an)) { logw(0xbadbad); memcpy(arena, snap, an); }
        if (memcmp(is, is2, sizeof is)) logw(0xbad15);
        if (overflow) logw(0x0f10f1);
      }
      /* the neg fixture logs the swapped intersection pointer: compare the rest */
      ok = ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * ln[0]);
      if (neg && (kind == 0)) { int k; ok = ln[0] == ln[1]; for (k = 0; ok && k < ln[0]; k++) if (k % 16 != 0 && lg[0][k] != lg[1][k]) ok = 0; }
      if (!special) { int nc = (ln[1] - 1) / 16; calls += nc; hitq += nc > 0; stops += s && nc == s; }
      if (ok) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        int k; printf("  DIFF %s case %d: %d / %d log words (dll / orig)", T[fi].name, i, ln[0], ln[1]);
        for (k = 0; k < ln[0] && k < ln[1]; k++) if (lg[0][k] != lg[1][k]) { printf(", first at word %d (call %d field %d): dll %08x orig %08x", k, k / 16, k % 16, lg[0][k], lg[1][k]); break; }
        printf("  input"); for (k = 0; k < 7; k++) printf(" %08x", ((u32 *)is2)[k]); printf("\n");
      }
    }
    printf("%-34s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; %d callbacks in %d hit queries, %d early stops, kd depth <= %d; %d of %d special cases differ\n", calls, hitq, stops, maxdepth, badsp, N / 5); bad_total += bad != 0;
    maxdepth = 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
