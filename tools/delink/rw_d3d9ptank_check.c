/* rw_d3d9ptank_check.c: bit-exact check of the librw DLL's d3d9ptank exports (group d3d9ptank, librw-mfl capi/d3d9ptank.cpp) against the ORIGINAL code of MFL.exe, both
   run natively (Wine).
   usage: rw_d3d9ptank_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9ptank_check.sh)
   Rows: see F[] below.  The device is tools/delink/fakedev.c (81 DrawPrimitive, 82 DrawIndexedPrimitive, 92 SetVertexShader, 104 SetIndices, 107 SetPixelShader) plus a
   recorder of this check for slot 89 SetFVF; every other device slot prints TRAP and exits.  Every exe callee of the rows (dynamic vertex buffer lock / unlock, frame
   LTM, lighting callback, transform, render state get / set / flush, texture stage state, raster stage, surface properties, stream source, geometry / material create /
   destroy, atomic set geometry, index buffer create, the generic instance select callback) and the engine's render state functions are logging stand-ins; the shared
   index buffer is a fake COM object of this check (Release 2, Lock 11, Unlock 12 with a hash of the indices; other slots print ABORT and exit).  The exe state the rows
   touch (ptank d3d9 block 0x7da6c4.., device caches 0x62e760.., device, max point size, plugin offset) is preset per call and logged after it.  Each call runs at x87
   control word 0x7f, 0x27f or 0x37f.  "rev" runs the original first.  va:byte patches the ORIGINAL's code first (mutation test: must give DIFF).
   Exit 1 on DIFF / MISSING / TRAP / ABORT / TIMEOUT / CRASH. */
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

#define ENGVAR 0x7d2128u
#define DEVVAR 0x7d2240u
#define BSS 0x7da6c4u      /* IB, max index, VB, VB offset, size per particle, point sprites */
#define CACHE 0x62e760u    /* last FVF, declaration, vertex shader, pixel shader, index buffer */
#define MAXPOINT 0x63a220u
#define PLUGOFF 0x7da6bcu
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void lgwords(const void *p, u32 n) { u32 k; for (k = 0; k < n; k++) lg(((const u32 *)p)[k]); }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 21)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
static u8 *hblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)hrnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))
static void setcw(u32 cw) { __asm__ volatile("fninit; fldcw %0" :: "m"(cw)); }      /* fninit: an out of range fsincos leaves the x87 stack one off */
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }
static u32 getcw(void) { u32 cw = 0; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw & 0xffff; }

#include "fakedev.c"      /* the fake IDirect3DDevice9 for the exe's device global; unrecorded methods trap */
static long __stdcall fd_SetFVF(void *self, u32 fvf) { lg(FAKEDEV_TAG | 89); lg(fvf); return fakedev_hr; }

/* the shared index buffer: a fake COM object */
struct HBuf { void **vtbl; u32 id, n, refs; u8 *data; };
static void *hb_vtbl[16];
static void hb_abort(int n) { printf("ABORT unfilled fake buffer slot %d\n", n); fflush(stdout); ExitProcess(6); }
#define HT(n) static long __stdcall ht_##n(void *self) { hb_abort(n); return 0; }
HT(0) HT(1) HT(2) HT(3) HT(4) HT(5) HT(6) HT(7) HT(8) HT(9) HT(10) HT(11) HT(12) HT(13) HT(14) HT(15)
static u32 nbuf;
static u32 __stdcall hb_release(struct HBuf *b) { lg(0xb1002); lg(b->id); return hrnd() % 2; }
static long __stdcall hb_lock(struct HBuf *b, u32 off, u32 size, void **pp, u32 flags) {
  long h; lg(0xb100b); lg(b->id); lg(off); lg(size); lg(flags);
  if (hrnd() % 6 == 0) h = (long)0x8876086c; else { *pp = b->data; h = hrnd() % 4 == 0; }
  lg((u32)h); return h;
}
static long __stdcall hb_unlock(struct HBuf *b) { u32 h = 2166136261u, k; for (k = 0; k < b->n + 16; k++) h = (h ^ b->data[k]) * 16777619u; lg(0xb100c); lg(b->id); lg(h); return 0; }
static void init_vtbls(void) {
  static long (__stdcall *const t[16])(void *) = { ht_0, ht_1, ht_2, ht_3, ht_4, ht_5, ht_6, ht_7, ht_8, ht_9, ht_10, ht_11, ht_12, ht_13, ht_14, ht_15 };
  int i; for (i = 0; i < 16; i++) hb_vtbl[i] = (void *)t[i];
  hb_vtbl[2] = (void *)hb_release; hb_vtbl[11] = (void *)hb_lock; hb_vtbl[12] = (void *)hb_unlock;
}
static struct HBuf *mkbuf(u32 n) {
  struct HBuf *b = amalloc(sizeof *b); b->vtbl = hb_vtbl; b->id = 0xb0f00000u + nbuf++; b->n = n; b->data = amalloc(n + 16); memset(b->data, 0x22, n + 16); return b;
}

/* ---- stand-ins ---- */
static u8 *ltmbuf, *lockbuf; static u32 selpos, selnorm, selcol;
static u32 __cdecl h_vbunlock(u32 vb) { u32 r = hrnd(); lg(0xa4ece50); lg(vb); lg(r); return r; }
static u32 __cdecl h_vblock(u32 stride, u32 n, u32 *vb, u32 *ptr, u32 *off) {
  lg(0xa4eccd0); lg(stride); lg(n); lg(P(vb)); lg(P(off));
  if (hrnd() % 5 == 0) { lg(0); return 0; }
  *vb = 0xb0000000u + hrnd() % 16; { u32 k = hrnd() % 16; if (k) *ptr = P(lockbuf) + k * 4; } *off = hrnd() % 1000; lg(*ptr); return 1 + hrnd() % 2;
  /* k == 0: lock pointer left unwritten (InsSetup's arg 4 = its local preset 0 at 545f7d, stored into the instance info block) */
}
static u8 *__cdecl h_framegetltm(u32 frame) { lg(0xa4d0ff0); lg(frame); return ltmbuf; }
static u32 __cdecl h_atlight(u32 atomic) { lg(0xa4ff4c0); lg(atomic); return hrnd(); }
static void __cdecl h_xformworld(u32 m) { lg(0xa4db600); lg(m); }
static void __cdecl h_getrs(u32 state, u32 *out) {      /* lighting (0x89) is sometimes left unwritten: the row then tests what its slot held */
  u32 v = hrnd() % 2 ? hrnd() % 3 : 0, wr = !(state == 0x89 && hrnd() % 8 == 0); if (wr) *out = v; lg(0xa4e3c50); lg(state); lg(wr ? v : 0xdead);
}
static void __cdecl h_setrs(u32 s, u32 v) { lg(0xa4e3c00); lg(s); lg(v); }
static void __cdecl h_settss(u32 st, u32 ty, u32 v) { lg(0xa4e3c70); lg(st); lg(ty); lg(v); }
static void __cdecl h_setraster(u32 r, u32 st) { lg(0xa4e55d0); lg(r); lg(st); }
static void __cdecl h_setsurf(u32 a, u32 b, u32 f) { lg(0xa4e3d10); lg(a); lg(b); lg(f); }
static void __cdecl h_setstream(u32 s, u32 vb, u32 off, u32 stride) { lg(0xa4db270); lg(s); lg(vb); lg(off); lg(stride); }
static void __cdecl h_flush(void) { lg(0xa4e3b30); }
static u32 __cdecl e_rsset(u32 s, u32 v) { lg(0xe0000020); lg(s); lg(v); return hrnd(); }
static u32 __cdecl e_rsget(u32 s, u32 *out) { *out = hrnd(); lg(0xe0000024); lg(s); lg(*out); return hrnd(); }
static u32 __cdecl h_geomcreate(u32 radius) { u8 *g = hblock(0x60); lg(0xa4f8730); lg(radius); return P(g); }
static u32 __cdecl h_matcreate(void) { u8 *m = hblock(0x20); lg(0xa4f8140); return P(m); }
static void __cdecl h_trisetmat(u32 geom, u32 tri, u32 mat) { lg(0xa4f8b00); lg(geom); lg(mat); lgwords((void *)(uintptr_t)(mat + 0xc), 3); }
static void __cdecl h_atsetgeom(u32 atomic, u32 geom, u32 f) { lg(0xa4f69e0); lg(atomic); lg(geom); lg(f); lg(*(u32 *)(uintptr_t)(geom + 8)); }
static void __cdecl h_geomdestroy(u32 g) { lg(0xa4f9060); lg(g); }
static void __cdecl h_matdestroy(u32 m) { lg(0xa4f81d0); lg(m); }
static u32 __cdecl h_ibcreate(u32 n, u32 *out) {
  lg(0xa4db980); lg(n); lg(P(out));
  if (hrnd() % 5 == 0) { lg(0); return 0; }
  *out = P(mkbuf(n * 2 < 0x10000 ? n * 2 : 0x10000)); lg(1); return 1 + hrnd() % 2;
}
static void __cdecl h_selectcb(u32 atomic) {      /* the generic instance functions it would select */
  u8 *pd = (u8 *)(uintptr_t)*(u32 *)(uintptr_t)(atomic + W(PLUGOFF)); lg(0xa546d90); lg(atomic);
  *(u32 *)(pd + 0x24) = 0x5485c0; *(u32 *)(pd + 0x28) = selpos; *(u32 *)(pd + 0x30) = selcol; *(u32 *)(pd + 0x34) = selnorm; *(u32 *)(pd + 0x38) = 0x548910;
}

enum { ENDING, SETUP, CREATE, RENDER, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rpPTankD3D9InsEnding", 0x545f50}, {"rw37_rpPTankD3D9InsSetup", 0x545f60}, {"rw37_rpPTankD3D9Create", 0x546780},
  {"rw37_rpPTankD3D9RenderCallBack", 0x5462c0}};
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G3)(u32, u32, u32); typedef u32 (__cdecl *G4)(u32, u32, u32, u32); typedef u32 (__cdecl *G5)(u32, u32, u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF];
static void sink(u32 w) { lg(w); }

static void reset(u32 seed) {
  u32 k;
  nlg = 0; ap = 0; nbuf = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng); for (k = 0; k < 6; k++) W(BSS + 4 * k) = 0; for (k = 0; k < 5; k++) W(CACHE + 4 * k) = 0; W(MAXPOINT) = 0; W(PLUGOFF) = 0;
  fakedev_log = sink; fakedev_hr = 0; W(DEVVAR) = P(fakedev_init()); fakedev_vtbl[89] = (void *)fd_SetFVF;
  EW(0x20) = P(e_rsset); EW(0x24) = P(e_rsget);
}
static u32 fbits(void) {      /* float bits: plain values, zeros, infinities, quiet and signalling NaNs, any bit pattern */
  static const u32 v[] = {0x3f800000, 0xbf800000, 0x3f000000, 0x3f19999a, 0x3f4ccccd, 0x40490fdb, 0xc0490fdb, 0x3fc90fdb, 0, 0x80000000, 0x7f800000, 0xff800000,
                          0x7fc00000, 0x7f800001, 0xffa00000, 0x00000001, 0x41200000, 0x3eaaaaab};
  return rnd() % 4 == 0 ? rnd() : v[rnd() % 18];
}
static u32 anglebits(void) {      /* mostly angles in range; sometimes beyond 2^63 (fsincos leaves its operand), infinities, NaNs */
  static const u32 big[] = {0x5f000000, 0x60ad78ec, 0x7149f2ca, 0x7f800000, 0xff800000, 0x7fc00000, 0x7f800001, 0xdf000001};
  union { float f; u32 u; } x;
  if (rnd() % 8 == 0) return big[rnd() % 8];
  if (rnd() % 4 == 0) return fbits();
  x.f = (float)((int)(rnd() % 20001) - 10000) * 0.001f; return x.u;
}
/* atomic -> geometry (flags +8, material list +0x20) -> material (texture +0) */
static u8 *mkatomic(u8 **geomOut, u8 **matOut) {
  u8 *atomic = rblock(0x100), *geom = rblock(0x60), *mat = rblock(0x20); u32 *mats = amalloc(8);
  *(u32 *)(atomic + 0x18) = P(geom); *(u32 *)(geom + 0x20) = P(mats); mats[0] = P(mat); mats[1] = rnd();
  *(u32 *)mat = rnd() % 2 ? 0 : rnd() | 1; *(u32 *)(geom + 8) = (rnd() & ~0x1cu) | (rnd() % 2 ? 4 : 0) | (rnd() % 2 ? 8 : 0) | (rnd() % 2 ? 0x10 : 0);
  *geomOut = geom; *matOut = mat; return atomic;
}
static void lgstate(void) { u32 k; for (k = 0; k < 6; k++) lg(W(BSS + 4 * k)); for (k = 0; k < 5; k++) lg(W(CACHE + 4 * k)); }
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 10, k, r = 0;
    lg(0x57e90000u + s); lg(op);
    ap = 0; fakedev_hr = rnd() % 4 == 0 ? (int32_t)0x8876086c : 0;
    switch (op) {
    case 0: {      /* rpPTankD3D9InsEnding */
      W(BSS + 8) = rnd();
      setcw(pickcw()); r = ((G0)f[ENDING])(); ncalls[ENDING]++; lg(getcw()); setcw(0x27f); lg(r); lgstate();
      break; }
    case 1: case 2: case 3: {      /* rpPTankD3D9InsSetup */
      u8 *geom, *mat, *atomic = mkatomic(&geom, &mat), *pt = rblock(0x100), *cam = rblock(0x20); u32 *lock = (u32 *)rblock(0x48), num = rnd() % 4 == 0 ? rnd() : rnd() % 200;
      for (k = 0; k < 4; k++) lock[k] = rnd() % 3 == 0 ? 0 : rnd() % 2 ? 1 : rnd();
      EW(0) = P(cam); ltmbuf = amalloc(0x40); for (k = 0; k < 16; k++) ((u32 *)ltmbuf)[k] = fbits();
      *(u32 *)(pt + 0x58) = (rnd() & ~0x28000u) | (rnd() % 4 == 0 ? 0x8000 : 0) | (rnd() % 2 ? 0x20000 : 0); *(u32 *)(pt + 0x78) = anglebits();
      for (k = 0; k < 8; k++) *(u32 *)(pt + 0xb0 + 4 * k) = fbits();
      lockbuf = amalloc(0x100); for (k = 0; k < 6; k++) W(BSS + 4 * k) = rnd();
      setcw(pickcw()); r = ((G4)f[SETUP])(P(lock), P(atomic), P(pt), num); ncalls[SETUP]++; lg(getcw()); setcw(0x27f);
      lg(r); lgwords(lock, 18); lg(*(u32 *)(geom + 8)); lgstate();
      break; }
    case 4: case 5: case 6: {      /* rpPTankD3D9Create */
      static const u32 pos[] = {0x5474a0, 0x547a70, 0x546f50, 0x5470e0, 0x5475f0, 0x5472a0}, mp[] = {0x3f800000, 0x3f000000, 0x42800000, 0x7fc00000, 0x3f800001, 0, 0xbf800000,
        0x7f800000, 0x3f7fffff, 0x41000000}, pf[] = {0, 4, 4, 0x24, 8, 0x10, 0x13, 0x100000, 0x80000, 0x31080017, 0x40, 0x1000000,
        0x100, 0x104, 0x80004, 0x1000004, 0x14, 0x31080013};      /* 0xcef7ffec / 0xcef7ffe8: bits inside both, only bit 4, bits outside (alone and with bit 4) */
      u8 *atomic = rblock(0x100), *pt = rblock(0x100), *pd = rblock(0x40); int max = rnd() % 8 == 0 ? (rnd() % 2 ? 0 : -3) : 1 + (int)(rnd() % 50);
      u32 df = (rnd() & ~0x40052u) | (rnd() % 2 ? 0x10 : 0) | (rnd() % 3 ? 0 : rnd() % 2 ? 0x40 : rnd() % 2 ? 2 : 0x40000), plat = rnd();
      W(PLUGOFF) = 0x60; *(u32 *)(atomic + 0x60) = P(pd);
      *(u32 *)(pt + 0x58) = rnd() % 6 == 0 ? rnd() : pf[rnd() % 18]; W(MAXPOINT) = mp[rnd() % 10]; W(BSS + 0x14) = rnd() % 2;
      W(BSS) = rnd() % 2 ? P(mkbuf(64)) : 0; W(BSS + 4) = rnd() % 4 == 0 ? rnd() : (u32)(max * 6 + (int)(rnd() % 5) - 2); W(BSS + 0x10) = rnd() % 2;
      selpos = rnd() % 8 == 0 ? rnd() : pos[rnd() % 6]; selnorm = rnd() % 2 ? 0x548550 : 0x5484e0; selcol = rnd() % 2 ? 0x548130 : 0x548190;
      setcw(pickcw()); r = ((G5)f[CREATE])(P(atomic), P(pt), (u32)max, df, plat); ncalls[CREATE]++; lg(getcw()); setcw(0x27f);
      lg(r); lgwords(pd + 0x20, 8); lg(*(u32 *)(pt + 0x7c)); lgstate();
      break; }
    default: {      /* rpPTankD3D9RenderCallBack */
      static const int nn[] = {0, 1, 2, 7, 100, 0x3fff, 0x4000, 0x4001, 0x7ffe, 0x7fff, 0x8000, 0xffff, 0x10000, 0x10001, 0x20001, -1, 0x1ffff, 0x1fffe, 3, 50};
      static const u32 fv[] = {2, 0x12, 0x42, 0x102, 0x142, 0x152, 0x112, 0x52, 0x22, 0x122, 0x162, 0x172};
      u8 *geom, *mat, *atomic = mkatomic(&geom, &mat), *pt = rblock(0x100);
      *(u32 *)(pt + 0x58) = rnd() % 2 ? rnd() : rnd() & ~0x40040u; *(u32 *)(pt + 0x64) = rnd() % 2 ? 0 : rnd() % 2 ? 1 : rnd();
      for (k = 0; k < 6; k++) W(BSS + 4 * k) = rnd(); W(BSS + 0x14) = rnd() % 2 ? 0 : 1 + rnd() % 2; W(BSS) = 0x1b000000u + rnd() % 2; W(BSS + 0xc) = rnd() % 5000;
      W(CACHE) = rnd() % 4 == 0 ? rnd() : fv[rnd() % 12]; W(CACHE + 4) = rnd(); W(CACHE + 8) = rnd() % 2 ? 0 : rnd(); W(CACHE + 0xc) = rnd() % 2 ? 0 : rnd();
      W(CACHE + 0x10) = 0x1b000000u + rnd() % 2; W(MAXPOINT) = fbits();
      setcw(pickcw()); r = ((G3)f[RENDER])(P(atomic), P(pt), (u32)nn[rnd() % 20]); ncalls[RENDER]++; lg(getcw()); setcw(0x27f);
      lg(r); lgstate();
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
  void (__cdecl *bind)(void *, void *); static void *tbl[48];
  /* table order = d3d9ptank.cpp enum E */
  static const u32 t[] = {BSS, MAXPOINT, PLUGOFF, DEVVAR, CACHE, CACHE + 4, CACHE + 8, CACHE + 0xc, CACHE + 0x10,
                          0x4ece50, 0x4eccd0, 0x4d0ff0, 0x4ff4c0, 0x4db600, 0x4e3c50, 0x4e3c00, 0x4e3c70, 0x4e55d0, 0x4e3d10, 0x4db270, 0x4e3b30,
                          0x4f8730, 0x4f8140, 0x4f8b00, 0x4f69e0, 0x4f9060, 0x4f81d0, 0x4db980, 0x546d90,
                          0x5474a0, 0x547a70, 0x546f50, 0x5470e0, 0x548550, 0x548130,
                          0x5461e0, 0x546ac0, 0x546b30, 0x546a10, 0x546a70, 0x546bb0, 0x546bf0, 0x545f60, 0x545f50};
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
  bind = (void *)GetProcAddress(m, "mflrw_d3d9ptank_bind");
  if (!bind) { printf("mflrw_d3d9ptank_bind MISSING export\n"); return 1; }
  jmp_patch(0x4ece50, (void *)h_vbunlock); jmp_patch(0x4eccd0, (void *)h_vblock); jmp_patch(0x4d0ff0, (void *)h_framegetltm); jmp_patch(0x4ff4c0, (void *)h_atlight);
  jmp_patch(0x4db600, (void *)h_xformworld); jmp_patch(0x4e3c50, (void *)h_getrs); jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4e3c70, (void *)h_settss);
  jmp_patch(0x4e55d0, (void *)h_setraster); jmp_patch(0x4e3d10, (void *)h_setsurf); jmp_patch(0x4db270, (void *)h_setstream); jmp_patch(0x4e3b30, (void *)h_flush);
  jmp_patch(0x4f8730, (void *)h_geomcreate); jmp_patch(0x4f8140, (void *)h_matcreate); jmp_patch(0x4f8b00, (void *)h_trisetmat); jmp_patch(0x4f69e0, (void *)h_atsetgeom);
  jmp_patch(0x4f9060, (void *)h_geomdestroy); jmp_patch(0x4f81d0, (void *)h_matdestroy); jmp_patch(0x4db980, (void *)h_ibcreate); jmp_patch(0x546d90, (void *)h_selectcb);
  init_vtbls();
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
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
