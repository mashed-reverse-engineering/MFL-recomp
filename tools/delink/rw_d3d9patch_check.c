/* rw_d3d9patch_check.c: bit-exact check of the librw DLL's d3d9patch exports (group d3d9patch, librw-mfl capi/d3d9patch.cpp) against the ORIGINAL code of MFL.exe, both
   run natively (Wine).
   usage: rw_d3d9patch_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9patch_check.sh)
   Rows: see F[] below.  No device is involved.  _rpD3D9PatchInstanceAtomic: the 8 refiners / index generators are logging stand-ins (control point block contents, output
   offsets, never stack addresses); geometry, morph target, mesh header, meshes, instance block and index list are arena blocks.  rwD3D9PatchAtomicAllInOneNodeNode: every
   exe callee, the engine's malloc / free, the patch LOD callback and the node's render callback are logging stand-ins, vertex / index buffers are fake COM objects (Lock
   slot 11, Unlock slot 12, every other slot logs + aborts); its call of _rpD3D9PatchInstanceAtomic is a stand-in too (the original's call site 0x55b395 is patched, the
   DLL gets it through the bind table), so each row is judged alone.  Each call runs at x87 control word 0x7f, 0x27f or 0x37f.  "rev" runs the original first.  va:byte
   patches the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
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
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }
static void call_patch(u32 va, void *to) {      /* retarget an existing `call rel32` */
  if (*(u8 *)(uintptr_t)va != 0xe8) { printf("no call at %x\n", va); ExitProcess(5); }
  *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5));
}

#define ENGVAR 0x7d2128u
#define RESMOD 0x200u
#define PATCHOFFVAR 0x7da864u
#define IDX32VAR 0x7da890u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void lgwords(const void *p, u32 n) { u32 k; for (k = 0; k < n; k++) lg(((const u32 *)p)[k]); }
static void lghash(const void *p, u32 n) { u32 h = 2166136261u, k; for (k = 0; k < n; k++) h = (h ^ ((const u8 *)p)[k]) * 16777619u; lg(h); }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 22)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))
static void setcw(u32 cw) { __asm__ volatile("fldcw %0" :: "m"(cw)); }
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }
static u32 getcw(void) { u32 cw = 0; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw & 0xffff; }

/* ---- stand-ins of _rpD3D9PatchInstanceAtomic's callees: (out, stride, control block, res) and (indices, base, res) ---- */
static u8 *vbase; static u16 *ibase;
static void logvec(u32 tag, u32 out, u32 stride, const u32 *c, u32 res, u32 per) {
  u32 k, j; lg(tag); lg(out - P(vbase)); lg(stride); for (k = 0; k < 16; k++) for (j = 0; j < per; j++) lg(c[k * 4 + j]); lg(res);
}
static void __cdecl h_trivec(u32 out, u32 stride, const u32 *c, u32 res) { logvec(0xa55c050, out, stride, c, res, 3); }
static void __cdecl h_tricol(u32 out, u32 stride, const u32 *c, u32 res) { logvec(0xa55c3d0, out, stride, c, res, 4); }
static void __cdecl h_tritex(u32 out, u32 stride, const u32 *c, u32 res) { lg(0xa55ca30); lg(out - P(vbase)); lg(stride); lgwords(c, 6); lg(res); }
static void __cdecl h_quadvec(u32 out, u32 stride, const u32 *c, u32 res) { logvec(0xa55cb20, out, stride, c, res, 3); }
static void __cdecl h_quadcol(u32 out, u32 stride, const u32 *c, u32 res) { logvec(0xa55cea0, out, stride, c, res, 4); }
static void __cdecl h_quadtex(u32 out, u32 stride, const u32 *c, u32 res) { lg(0xa55d4e0); lg(out - P(vbase)); lg(stride); lgwords(c, 8); lg(res); }
static u32 triidx(u32 r) { return (r + 2) * r + (r & 1); }
static u32 quadidx(u32 r) { return (r + 1) * r * 2; }
static void genidx(u32 tag, u16 *io, u32 base, u32 res, u32 ni) {      /* first and last index of the strip get a value (the row copies the last one when it joins two strips) */
  lg(tag); lg((u32)(io - ibase)); lg(base); lg(res);
  if (ni) { io[0] = (u16)hrnd(); io[ni - 1] = (u16)hrnd(); }
}
static void __cdecl h_triidx(u16 *io, u32 base, u32 res) { genidx(0xa55d5d0, io, base, res, triidx(res)); }
static void __cdecl h_quadidx(u16 *io, u32 base, u32 res) { genidx(0xa55d6c0, io, base, res, quadidx(res)); }

/* ---- rwD3D9PatchAtomicAllInOneNodeNode: fake buffers and stand-ins ---- */
struct HBuf { void **vtbl; u32 id, kind, n; u8 *data; };      /* kind 1 = vertex buffer made by the stand-in, 2 = vertex buffer of a kept resource entry, 3 = index buffer */
static void *hb_vtbl[16];
static void hb_abort(int n) { printf("ABORT unfilled fake buffer slot %d\n", n); fflush(stdout); ExitProcess(6); }
#define HT(n) static long __stdcall ht_##n(void *self) { hb_abort(n); return 0; }
HT(0) HT(1) HT(2) HT(3) HT(4) HT(5) HT(6) HT(7) HT(8) HT(9) HT(10) HT(11) HT(12) HT(13) HT(14) HT(15)
static u32 nbuf; static int lockfail_ok;
static long __stdcall hb_lock(struct HBuf *b, u32 off, u32 size, void **pp, u32 flags) {
  long h; lg(0xb10c); lg(b->id); lg(off); lg(size); lg(flags);
  if (hrnd() % 6 == 0 && (b->kind != 1 || lockfail_ok)) h = (long)0x8876086c; else { *pp = b->data; h = hrnd() % 4 == 0; }
  lg((u32)h); return h;
}
static long __stdcall hb_unlock(struct HBuf *b) { lg(0xb10d); lg(b->id); if (b->kind == 3) lghash(b->data, b->n); return 0; }
static void init_vtbls(void) {
  static long (__stdcall *const t[16])(void *) = { ht_0, ht_1, ht_2, ht_3, ht_4, ht_5, ht_6, ht_7, ht_8, ht_9, ht_10, ht_11, ht_12, ht_13, ht_14, ht_15 };
  int i; for (i = 0; i < 16; i++) hb_vtbl[i] = (void *)t[i];
  hb_vtbl[11] = (void *)hb_lock; hb_vtbl[12] = (void *)hb_unlock;
}
static struct HBuf *mkbuf(u32 kind, u32 n) {
  struct HBuf *b = amalloc(sizeof *b); b->vtbl = hb_vtbl; b->kind = kind; b->id = 0xb0f00000u + (kind << 16) + nbuf++; b->n = n; b->data = amalloc(n + 16); memset(b->data, 0x22, n + 16); return b;
}
static u32 lodres; static u8 *premesh, *ltmbuf, *capsbuf, *lastre; static u16 *preidx; static u32 lastsize;
/* instanced mesh records {indices, numIndices, minVert, numVerts, material, mesh} as _rpD3D9PatchInstanceAtomic would leave them */
static void fill_im(u8 *im, u32 n) {
  u32 i; for (i = 0; i < n; i++) { u32 *e = (u32 *)(im + 0x18 * i); e[0] = P(preidx); e[1] = hrnd() % 8 == 0 ? 0 : 2 + hrnd() % 39; e[2] = hrnd() % 100; e[3] = hrnd() % 100; e[4] = hrnd(); e[5] = P(premesh); }
}
static u32 __cdecl h_lod(u32 atomic, u32 user) { lg(0xcb0010); lg(atomic); lg(user); return lodres; }
static void __cdecl h_freeres(u32 res) { lg(0xa4ea870); lg(res); }
/* engine malloc: the block starts with 24 sane instanced mesh records (a failed vertex buffer Lock leaves the block as it came) */
static u32 __cdecl h_malloc(u32 size, u32 hint) {
  u8 *p = amalloc((size > (1u << 20) ? (1u << 20) : size) + 0x300); lg(0xe0000108); lg(size); lg(hint); fill_im(p + 0x24, 24); return P(p);
}
static void __cdecl h_free(u32 p) { lg(0xe000010c); lg(p); lgwords((void *)(uintptr_t)(p + 4), 7); }
static u32 __cdecl h_createdecl(const u8 *el, u32 *out) {
  u32 n = 0; lg(0xa4db9e0); while (n < 20 && *(const u16 *)(el + 8 * n) != 0xff) n++; lg(n); lgwords(el, 2 * (n + 1));
  if (hrnd() % 8) *out = 0xdec10000u + n;      /* sometimes left unwritten: the row then stores what its slot held */
  return hrnd() % 2;
}
static u32 __cdecl h_createvb(u32 stride, u32 size, u32 *vb, u32 *off) {
  lg(0xa4ec6d0); lg(stride); lg(size);
  if (hrnd() % 6 == 0) { lg(0); return 0; }
  *vb = P(mkbuf(1, 64)); *off = hrnd() % 3 ? stride * (hrnd() % 5) : hrnd() % 0x1000; lg(*off); return 1 + hrnd() % 2;
}
static void __cdecl h_destroyvb(u32 stride, u32 size, u32 vb, u32 off) { lg(0xa4ec870); lg(stride); lg(size); lg(((struct HBuf *)(uintptr_t)vb)->id); lg(off); }
static u32 __cdecl h_patchinst(u32 inst, u32 geom, u32 res) {
  u32 r = hrnd() % 6 ? 1 + hrnd() % 2 : 0; lg(0xa55a750); lgwords((void *)(uintptr_t)(inst + 4), 7); lg(geom); lg(res); lg(r);
  if (r) { u32 k = hrnd() % 5, m = *(u32 *)(uintptr_t)(inst + 0x14); if (k > m) k = m; *(u32 *)(uintptr_t)(inst + 0x14) = k; fill_im((u8 *)(uintptr_t)(inst + 0x24), k); }
  return r;
}
static u32 __cdecl h_reinst(u32 inst, u32 geom, u32 res) { u32 r = hrnd() % 5 ? 1 + hrnd() % 2 : 0; lg(0xa55d760); lgwords((void *)(uintptr_t)(inst + 4), 7); lg(geom); lg(res); lg(r); return r; }
static u32 __cdecl h_allocres(u32 owner, u32 pentry, u32 size, u32 cb) {
  lg(0xa4ea960); lg(owner); lg(pentry); lg(size); lg(cb);
  lastsize = size; lastre = amalloc(0x18 + size + 16); { u32 k; for (k = 0; k < 0x18 + size + 16; k++) lastre[k] = (u8)hrnd(); }
  *(u32 *)(uintptr_t)pentry = P(lastre); return P(lastre);
}
static u32 __cdecl h_ib32(u32 n, u32 *out) { lg(0xa4db9b0); lg(n); if (hrnd() % 4 == 0) { lg(0); return 0; } *out = P(mkbuf(3, n * 4)); lg(1); return 1 + hrnd() % 2; }
static u32 __cdecl h_ib16(u32 n, u32 *out) { lg(0xa4db980); lg(n); if (hrnd() % 6 == 0) { lg(0); return 0; } *out = P(mkbuf(3, n * 2)); lg(1); return 1 + hrnd() % 2; }
static u32 __cdecl h_atlight(u32 atomic) { lg(0xa4ff4c0); lg(atomic); return hrnd(); }
static u8 *__cdecl h_framegetltm(u32 frame) { lg(0xa4d0ff0); lg(frame); return ltmbuf; }
static void __cdecl h_xformworld(u32 m) { lg(0xa4db600); lg(m); }
static void __cdecl h_getrs(u32 state, u32 *out) { u32 v = hrnd() % 3 ? 1 + hrnd() % 2 : 0; *out = v; lg(0xa4e3c50); lg(state); lg(v); }
static u8 *__cdecl h_getcaps(void) { lg(0xa4dbca0); return capsbuf; }
static void __cdecl h_setrs(u32 s, u32 v) { lg(0xa4e3c00); lg(s); lg(v); }
static u32 __cdecl h_rendercb(u32 res, u32 atomic, u32 type, u32 flags) { lg(0xcb0003); lg(res); lg(atomic); lg(type); lg(flags); return hrnd(); }

enum { PIA, NODE, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rpD3D9PatchInstanceAtomic", 0x55a750}, {"rw37_rwD3D9PatchAtomicAllInOneNodeNode", 0x55b060}};
typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF];

static void reset(u32 seed) {
  nlg = 0; ap = 0; nbuf = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng); W(0x947138) = RESMOD; W(PATCHOFFVAR) = 0; W(IDX32VAR) = 0;
}
/* the u of a texture coordinate of set 0: 0.0 = quad patch, anything else = tri patch (also -0.0 = quad, NaNs = tri) */
static u32 ubits(void) {
  static const u32 v[] = {0x3f800000, 0xbf000000, 0x80000000, 0x7fc00000, 0x7f800001, 0x00000001, 0xffc00000, 0x7f800000};
  return rnd() % 5 < 3 ? 0 : v[rnd() % 8];
}
/* a patch geometry: vertices, optional normals / prelit colours, 3 texture coordinate arrays, mesh header with 0-3 meshes of 16 indices per patch.  Returns the number
   of patches the row will walk (a mesh count that is no multiple of 16 walks one more block). */
static u8 *mkgeom(u32 maxPatches, u32 *npatches, u32 *nmeshes) {
  u8 *geom = rblock(0x60), *mt = rblock(0x20), *mh; u32 V = 24 + rnd() % 40, k, nm = rnd() % 8 == 0 ? 0 : 1 + rnd() % 3, off = (rnd() % 3) * 4, np = 0, m;
  *(u32 *)(geom + 0x5c) = P(mt); *(u32 *)(mt + 0x14) = P(rblock(V * 12)); *(u32 *)(mt + 0x18) = rnd() % 4 == 0 ? 0 : P(rblock(V * 12));
  *(u32 *)(geom + 0x30) = rnd() % 4 == 0 ? 0 : P(rblock(V * 4));
  *(u32 *)(geom + 0x1c) = rnd() % 5 == 0 ? 0 : rnd() % 16 == 0 ? (u32)-1 : 1 + rnd() % 3;
  for (k = 0; k < 8; k++) *(u32 *)(geom + 0x34 + 4 * k) = k < 3 ? P(rblock(V * 8)) : 0;
  for (k = 0; k < V; k++) ((u32 *)(uintptr_t)*(u32 *)(geom + 0x34))[k * 2] = ubits();
  mh = rblock(0x10 + off + nm * 0xc + 8); *(u16 *)(mh + 4) = (u16)nm; *(u32 *)(mh + 0xc) = off; *(u32 *)(geom + 0x54) = P(mh);
  for (m = 0; m < nm; m++) {
    u32 *mesh = (u32 *)(mh + 0x10 + off + m * 0xc), p = 1 + rnd() % 4, i, cnt; u16 *ix;
    if (np + p > maxPatches) p = maxPatches > np ? maxPatches - np : 0;
    if (p == 0 || rnd() % 8 == 0) cnt = 0, p = 0;
    else if (rnd() % 16 == 0) cnt = (u32)-16, p = 0;
    else cnt = p * 16 - (rnd() % 4 == 0 ? 8 : 0);
    ix = amalloc((p + 1) * 32); for (i = 0; i < (p + 1) * 16; i++) ix[i] = (u16)(rnd() % V);
    mesh[0] = P(ix); mesh[1] = cnt; mesh[2] = rnd(); np += p;
  }
  *npatches = np; *nmeshes = nm;
  return geom;
}
static u32 fbits(void) {      /* float bits: mostly values whose squares and sums hit the compared bounds exactly or nearly, sometimes any bit pattern */
  static const float v[] = {0.0f, 1.0f, -1.0f, 0.6f, 0.8f, 0.5f, 0.94868f, 1.0488f, 0.7f, 0.3f, 2.0f, 3.0f, 4.0f, -2.0f, 5.0f, 0.948683f, 1.048809f};
  union { float f; u32 u; } x; if (rnd() % 10 == 0) return rnd(); x.f = v[rnd() % 17]; return x.u;
}
/* a matrix row for the unit length test: 3 of 4 rows have a squared length near or inside [0.9, 1.1], so all three rows and both bounds are really compared (as in
   rw_noded3d9_check.c) */
static void unitrow(u32 *r) {
  static const float a[] = {1.0f, -1.0f, 0.94868f, 1.0488f, 0.948683f, 1.048809f, 0.96f, 1.02f, 1.003f, 0.92f, 1.07f, 0.95f, 1.05f};
  union { float f; u32 u; } x; u32 p = rnd() % 3;
  if (rnd() % 4 == 0) { r[0] = fbits(); r[1] = fbits(); r[2] = fbits(); return; }
  r[0] = r[1] = r[2] = 0;
  if (rnd() % 4 == 0) { x.f = 0.6f; r[p] = x.u; x.f = rnd() % 2 ? 0.8f : -0.8f; r[(p + 1) % 3] = x.u; }
  else { x.f = a[rnd() % 13]; r[p] = x.u; }
}
/* a resource entry of an instanced patch atomic: serial +0x18, vertex buffer / offset / stride +0x28.., vertex count +0x54; none, unlinked, or linked between two nodes */
static u32 *resPn[2], *resNn[2];
static u8 *mkres2(int w, u32 serial, u32 nverts) {
  u8 *res; u32 *Pn, *Nn;
  resPn[w] = resNn[w] = 0;
  if (rnd() % 4 == 0) return 0;
  res = rblock(0x60); *(u32 *)(res + 0x18) = rnd() % 3 ? serial : rnd() % 2 ? serial + 1 : serial | 0x10000; *(u32 *)(res + 0x54) = rnd() % 4 ? nverts : nverts + 1;
  *(u32 *)(res + 0x28) = P(mkbuf(2, 64)); *(u32 *)(res + 0x2c) = rnd() % 0x400; *(u32 *)(res + 0x30) = 0xc + rnd() % 0x30;
  if (rnd() % 3 == 0) { *(u32 *)res = 0; return res; }
  Pn = amalloc(8); Nn = amalloc(8); Pn[0] = P(res); Pn[1] = rnd(); Nn[0] = rnd(); Nn[1] = P(res); *(u32 *)res = P(Nn); *(u32 *)(res + 4) = P(Pn);
  resPn[w] = Pn; resNn[w] = Nn;
  return res;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 3, k, r = 0;
    lg(0x57e90000u + s); lg(op);
    ap = 0; lastre = 0;      /* every step starts a fresh arena: all results are logged below */
    switch (op) {
    default: {      /* rwD3D9PatchAtomicAllInOneNodeNode */
      static const int rr[] = {0, 1, 1, 2, 2, 3, 3, 5, 200, 200, -1, -2};
      static const u32 mf[] = {0, 0, 1, 0x20000, 0x20001, 0x10}, tcs[] = {0, 1, 2, 3, 8};
      u8 *node = rblock(0x20), *atomic = rblock(0x100), *pm = rblock(0x40), *geom, *mh, *r1, *r2, *prelit; u32 *cbs = (u32 *)rblock(16), *params = amalloc(8), *L;
      u32 np, nm, nq, nt, serial = rnd() % 4, nverts, q, t;
      lodres = (u32)rr[rnd() % 12]; nq = rnd() % ((int)lodres == 200 ? 3 : 4); nt = rnd() % 4;
      if (rnd() % 10 == 0) { lodres = 0; nq = 65535 + rnd() % 2; nt = 0; }      /* vertex count exactly at / one above the 16 bit index limit */
      q = (lodres + 1) * (lodres + 1); t = (u32)((int32_t)((lodres + 1) * (lodres + 2)) / 2); nverts = q * nq + t * nt;
      geom = mkgeom(12, &np, &nm); mh = (u8 *)(uintptr_t)*(u32 *)(geom + 0x54); prelit = (u8 *)(uintptr_t)*(u32 *)(geom + 0x30);
      *(u32 *)(node + 0x14) = P(cbs); cbs[3] = rnd() % 4 == 0 ? 0 : P(h_rendercb); params[0] = P(atomic); params[1] = rnd();
      W(PATCHOFFVAR) = 0x70; *(u32 *)(atomic + 0x70) = P(h_lod); *(u32 *)(atomic + 0x78) = P(pm); *(u32 *)(atomic + 0x18) = P(geom);
      *(u32 *)(pm + 0x34) = nt; *(u32 *)(pm + 0x38) = nq; *(u32 *)(pm + 0x2c) = (rnd() & 0xff00fff9u) | (rnd() % 2 ? 2 : 0) | (rnd() % 2 ? 4 : 0) | (tcs[rnd() % 5] << 16);
      *(u32 *)(geom + 8) = rnd(); *(u32 *)(geom + 0x14) = rnd() % 8 == 0 ? 0 : 1 + rnd() % 100; *(u32 *)(geom + 0x18) = rnd() % 2 ? 1 : 2;
      *(u16 *)(geom + 0xc) = (u16)(rnd() % 2 ? 0 : rnd()); *(u16 *)(mh + 6) = (u16)serial; mh[1] = (u8)((mh[1] & ~1u) | (rnd() % 5 == 0));
      /* a sane mesh for the instanced mesh records: its control point alphas are all 0xff in half of the cases (vertex alpha off) */
      preidx = amalloc(96); for (k = 0; k < 48; k++) preidx[k] = (u16)(rnd() % 24);
      premesh = amalloc(12); ((u32 *)premesh)[0] = P(preidx); ((u32 *)premesh)[1] = rnd() % 6 == 0 ? 0 : 1 + rnd() % 48; ((u32 *)premesh)[2] = rnd();
      if (prelit && rnd() % 2) for (k = 0; k < 24; k++) prelit[k * 4 + 3] = 0xff;
      r1 = mkres2(0, serial, nverts); r2 = mkres2(1, serial, nverts); *(u32 *)(geom + 0x58) = P(r1); *(u32 *)(atomic + 0x14) = P(r2);
      L = amalloc(8); { u32 *Fn = amalloc(8); L[0] = P(Fn); L[1] = rnd(); Fn[0] = rnd(); Fn[1] = P(L); EW(RESMOD + 0x24) = P(L); }
      { u32 *L2 = amalloc(8), *F2 = amalloc(8); L2[0] = P(F2); L2[1] = rnd(); F2[0] = rnd(); F2[1] = P(L2); EW(RESMOD + 0x28) = P(L2); }      /* decoy list beside the used list */
      ltmbuf = amalloc(0x40); for (k = 0; k < 16; k++) ((u32 *)ltmbuf)[k] = fbits();
      unitrow((u32 *)ltmbuf); unitrow((u32 *)(ltmbuf + 0x10)); unitrow((u32 *)(ltmbuf + 0x20)); *(u32 *)(ltmbuf + 0xc) = mf[rnd() % 6];
      capsbuf = rblock(0x40); *(u32 *)(capsbuf + 0x1c) = rnd() % 3 == 0 ? rnd() | 0x10000 : rnd() & ~0x10000u;
      W(IDX32VAR) = rnd() % 3 ? 1 + rnd() % 2 : 0; EW(0x108) = P(h_malloc); EW(0x10c) = P(h_free);
      lockfail_ok = (int)lodres >= 0 && nq + nt > 0;
      setcw(pickcw());
      r = ((G2)f[NODE])(P(node), P(params)); ncalls[NODE]++;
      lg(getcw()); setcw(0x27f);
      lg(r); lg(*(u16 *)(geom + 0xc)); lg(*(u32 *)(geom + 0x58)); lg(*(u32 *)(atomic + 0x14));
      if (r1) { lg(*(u32 *)r1); lg(*(u32 *)(r1 + 4)); } if (r2) { lg(*(u32 *)r2); lg(*(u32 *)(r2 + 4)); }
      lg(L[0]); lg(L[1]); lg(((u32 *)(uintptr_t)L[0])[0]); lg(((u32 *)(uintptr_t)L[0])[1]);
      for (k = 0; k < 2; k++) if (resPn[k]) { lg(resPn[k][0]); lg(resPn[k][1]); lg(resNn[k][0]); lg(resNn[k][1]); }
      if (lastre) lgwords(lastre, (0x18 + lastsize + 16) / 4);
      break; }
    case 0: {      /* _rpD3D9PatchInstanceAtomic */
      static const int rr[] = {0, 1, 1, 2, 2, 3, 3, 4, 5, 7, 16, 16, 31, 64, 100, 127, 128, 180, 181, 200, -1, -2, -3, 1, 2, 3, 4, 5, 6, 8};
      u32 res = (u32)rr[rnd() % 30], np, nm, capm, ni, isz, n; u8 *geom = mkgeom((int)res >= 64 ? 5 : 12, &np, &nm), *inst; u16 *ib;
      capm = rnd() % 6 == 0 && nm ? nm - 1 : nm + rnd() % 4;
      n = 0x24 + 0x18 * (nm + capm + 2); inst = rblock(n);
      ni = triidx(res) > quadidx(res) ? triidx(res) : quadidx(res); isz = (np * (ni + 2) + 16) * 2;
      ib = amalloc(isz); memset(ib, 0x11, isz); ibase = ib + 4;
      vbase = amalloc(16);
      *(u32 *)(inst + 4) = P(ibase); *(u32 *)(inst + 8) = (rnd() & 0xff00fff9u) | (rnd() % 2 ? 2 : 0) | (rnd() % 2 ? 4 : 0) | ((rnd() % 5) << 16);
      *(u32 *)(inst + 0xc) = 0xc + rnd() % 0x30; *(u32 *)(inst + 0x10) = P(vbase); *(u32 *)(inst + 0x14) = capm;
      setcw(pickcw());
      r = ((G3)f[PIA])(P(inst), P(geom), res); ncalls[PIA]++;
      lg(getcw()); setcw(0x27f);
      lg(r);
      for (k = 0; k < n / 4; k++) {      /* the instance block; pointers into the index list / mesh header as offsets */
        u32 v = ((u32 *)inst)[k];
        if (v >= P(ib) && v < P(ib) + isz) v = 0x1b000000u + (v - P(ib)); else if (v >= P(arena) && v < P(arena) + ARENA) v = 0xa0000000u + (v - P(arena));
        lg(v);
      }
      lghash(ib, isz);
      break; }
    }
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena, save_eng[sizeof eng];
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 30 ? i - 30 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  if (memcmp(save_eng, eng, sizeof eng)) { for (i = 0; i < sizeof eng && save_eng[i] == eng[i]; i++); printf("  seed %u: engine instance differs at +0x%x\n", seed, i); return 1; }
  if (memcmp(save_arena, arena, ARENA)) { for (i = 0; i < ARENA && save_arena[i] == arena[i]; i++); printf("  seed %u: arena differs at +0x%x\n", seed, i); return 1; }
  return 0;
}
static volatile u32 cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH %08lx at %p seed %u; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; double tot = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[32];
  /* table order = d3d9patch.cpp enum E */
  static const u32 t[] = {0x55c050, 0x55c3d0, 0x55ca30, 0x55d5d0, 0x55cb20, 0x55cea0, 0x55d4e0, 0x55d6c0,
                          PATCHOFFVAR, IDX32VAR, 0x947138,
                          0x4ea870, 0x4db9e0, 0x4ec6d0, 0x4ec870, 0x55a750, 0x55d760, 0x4ea960, 0x4fa950, 0x4db9b0, 0x4db980,
                          0x4ff4c0, 0x4d0ff0, 0x4db600, 0x4e3c50, 0x4dbca0, 0x4e3c00};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * 3000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9patch_bind");
  if (!bind) { printf("mflrw_d3d9patch_bind MISSING export\n"); return 1; }
  jmp_patch(0x55c050, (void *)h_trivec); jmp_patch(0x55c3d0, (void *)h_tricol); jmp_patch(0x55ca30, (void *)h_tritex); jmp_patch(0x55d5d0, (void *)h_triidx);
  jmp_patch(0x55cb20, (void *)h_quadvec); jmp_patch(0x55cea0, (void *)h_quadcol); jmp_patch(0x55d4e0, (void *)h_quadtex); jmp_patch(0x55d6c0, (void *)h_quadidx);
  jmp_patch(0x4ea870, (void *)h_freeres); jmp_patch(0x4db9e0, (void *)h_createdecl); jmp_patch(0x4ec6d0, (void *)h_createvb); jmp_patch(0x4ec870, (void *)h_destroyvb);
  jmp_patch(0x55d760, (void *)h_reinst); jmp_patch(0x4ea960, (void *)h_allocres); jmp_patch(0x4db9b0, (void *)h_ib32); jmp_patch(0x4db980, (void *)h_ib16);
  jmp_patch(0x4ff4c0, (void *)h_atlight); jmp_patch(0x4d0ff0, (void *)h_framegetltm); jmp_patch(0x4db600, (void *)h_xformworld); jmp_patch(0x4e3c50, (void *)h_getrs);
  jmp_patch(0x4dbca0, (void *)h_getcaps); jmp_patch(0x4e3c00, (void *)h_setrs);
  call_patch(0x55b395, (void *)h_patchinst);      /* the node's call of row 1: a stand-in on both sides (table slot 15 below), row 1's own code stays intact */
  init_vtbls();
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  tbl[15] = (void *)h_patchinst;
  bind((void *)ENGVAR, tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA); memcpy(save_eng, eng, sizeof eng);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  { char line[96]; sprintf(line, "%.0f log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL"); fputs(line, stdout); }      /* one write: the DLL's exit line must not split it */
  printf("DONE\n");
  return bad != 0;
}
