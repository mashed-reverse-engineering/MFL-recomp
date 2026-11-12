/* rw_d3d9rasq_check.c: bit-exact check of the librw DLL's d3d9rasq exports (group d3d9rasq, librw-mfl capi/rwd3d9rasq.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9rasq_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9rasq_check.sh)
   Rows: see F[] below.  The exe's device global (0x7d2240) holds the fake device of tools/delink/fakedev.c with the recorders of this file added (an unrecorded method traps);
   textures / surfaces are fake COM objects that log their calls; every exe callee of the rows is a jmp-patched logging stand-in.  Stack addresses are never logged (arena
   addresses are: the arena is the same block in both runs).  RenderQuad runs at x87 control word 0x7f or 0x27f, chosen per call.
   "rev" runs the original first (the default is DLL first).  va:byte patches the ORIGINAL's code first (mutation test: must give DIFF).
   Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
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

#include "fakedev.c"      /* the fake IDirect3DDevice9 for the exe's device global; unrecorded methods trap */
#define ENGVAR 0x7d2128u
#define DEVVAR 0x7d2240u
#define CURTGT 0x7d27f0u
#define QUAD 0x7d2780u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void sink(u32 w) { lg(w); }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }

#define ARENA (1u << 21)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
#define P(p) ((u32)(uintptr_t)(p))

/* fake COM objects: {vtbl, id, refcount} */
static void c_trap(int n) { printf("TRAP unexpected COM method %d\n", n); fflush(stdout); ExitProcess(5); }
#define CT(n) static long __stdcall ct_##n(void *self) { c_trap(n); return 0; }
CT(0) CT(1) CT(2) CT(3) CT(4) CT(5) CT(6) CT(7) CT(8) CT(9) CT(10) CT(11) CT(12) CT(13) CT(14) CT(15) CT(16) CT(17) CT(18) CT(19)
#define CTP(n) (void *)ct_##n
static void *const ctraps[20] = { CTP(0), CTP(1), CTP(2), CTP(3), CTP(4), CTP(5), CTP(6), CTP(7), CTP(8), CTP(9), CTP(10), CTP(11), CTP(12), CTP(13), CTP(14), CTP(15), CTP(16), CTP(17), CTP(18), CTP(19) };
struct Obj { void **vtbl; u32 id, rc; };
static void *v_surf[20], *v_tex2d[20], *v_texcube[20];
static u32 nobj;
static struct Obj *mkobj(void **vt) { struct Obj *o = amalloc(sizeof *o); o->vtbl = vt; o->id = 0xc0b00000u + nobj++; o->rc = 1 + hrnd() % 3; return o; }
static u32 __stdcall c_release(struct Obj *o) { lg(0xc20e); lg(o->id); if (o->rc) o->rc--; lg(o->rc); return o->rc; }
static long __stdcall c_getlevel(struct Obj *t, u32 level, void **pp) { lg(0xc318); lg(t->id); lg(level); *pp = mkobj(v_surf); lg(P(*pp)); return 0; }
static long __stdcall c_getcube(struct Obj *t, u32 face, u32 level, void **pp) { lg(0xc319); lg(t->id); lg(face); lg(level); *pp = mkobj(v_surf); lg(P(*pp)); return 0; }
static long __stdcall c_getdesc(struct Obj *t, u32 level, u32 *d) {      /* D3DSURFACE_DESC: Format Type Usage Pool MultiSampleType MultiSampleQuality Width Height */
  int k; d[0] = hrnd() % 70; d[1] = hrnd() % 4; d[2] = hrnd(); d[3] = hrnd() % 3 == 0 ? 0 : hrnd() % 4; d[4] = hrnd() % 3; d[5] = hrnd() % 4; d[6] = hrnd() % 2000; d[7] = hrnd() % 1200;
  lg(0xc311); lg(t->id); lg(level); for (k = 0; k < 8; k++) lg(d[k]); return 0;
}
static void init_vtbls(void) {
  int i;
  for (i = 0; i < 20; i++) v_surf[i] = v_tex2d[i] = v_texcube[i] = ctraps[i];
  v_surf[2] = (void *)c_release; v_tex2d[17] = v_texcube[17] = (void *)c_getdesc; v_tex2d[18] = (void *)c_getlevel; v_texcube[18] = (void *)c_getcube;
}
static void *devobj;

/* stand-ins for the exe callees */
static u8 *lockbuf;
static int32_t __cdecl h_regplugin(u32 size, u32 id, u32 ctor, u32 dtor, u32 copy) {
  int32_t r = hrnd() % 5 == 0 ? -1 - (int32_t)(hrnd() % 3) : (int32_t)(0x30 + 4 * (hrnd() % 16)); lg(0xa4d7820); lg(size); lg(id); lg(ctor); lg(dtor); lg(copy); lg((u32)r); return r;
}
static int __cdecl h_rwsetrt(u32 idx, u32 raster) { int r = hrnd() % 2; lg(0xa4db110); lg(idx); lg(raster); lg(r); return r; }
static void __cdecl h_setrt(u32 idx, u32 surf) { lg(0xa4d7bb0); lg(idx); lg(surf); }
static void __cdecl h_setds(u32 surf) { lg(0xa4d7b80); lg(surf); }
static int __cdecl h_beginscene(void) { int r = hrnd() % 4 != 0; lg(0xa4d9e60); lg(r); return r; }
static void __cdecl h_rwgetrs(u32 state, u32 *out) { u32 v = hrnd(); *out = v; lg(0xa4e5070); lg(state); lg(v); }
static void __cdecl h_getrs(u32 state, u32 *out) { u32 v = hrnd(); *out = v; lg(0xa4e3c50); lg(state); lg(v); }
static void __cdecl h_rwsetraster(u32 raster, u32 stage) { lg(0xa4e5440); lg(raster); lg(stage); }
static void __cdecl h_valpha(u32 v) { lg(0xa4e5860); lg(v); }
static void __cdecl h_srcblend(u32 v) { lg(0xa4e53a0); lg(v); }
static void __cdecl h_dstblend(u32 v) { lg(0xa4e53f0); lg(v); }
static void __cdecl h_rwsetrs(u32 s, u32 v) { lg(0xa4e5be0); lg(s); lg(v); }
static void __cdecl h_setrs(u32 s, u32 v) { lg(0xa4e3c00); lg(s); lg(v); }
static void __cdecl h_im2d(u32 type, const u32 *verts, u32 n) { int k; lg(0xa4ebb80); lg(type); lg(P(verts)); lg(n); for (k = 0; k < 28; k++) lg(verts[k]); }
static void __cdecl h_pix2rgb(u32 *out, const u32 *in, u32 fmt) { lg(0xa4ee0d0); lg(*in); lg(fmt); *out = hrnd(); lg(*out); }
static void __cdecl h_rgb2pix(u32 *out, const u32 *in, u32 fmt) { lg(0xa4ede80); lg(*in); lg(fmt); *out = hrnd(); lg(*out); }
static int __cdecl h_rasterlock(u8 **out, u32 raster, u32 mode) { int r = hrnd() % 6 != 0; lg(0xa4dfcd0); lg(raster); lg(mode); lg(r); if (r) *out = lockbuf; return r; }
static void __cdecl h_rasterunlock(u32 p, u32 raster, u32 mode) { lg(0xa4e0040); lg(p); lg(raster); lg(mode); }
static void __cdecl h_errorset(const u32 *e) { lg(0xa4ea160); lg(e[0]); lg(e[1]); }
static int __cdecl h_errcode(u32 code) { int r = (int)hrnd(); lg(0xa53eac0); lg(code); lg((u32)r); return r; }
static void *__cdecl h_flcreate(u32 size, u32 a, u32 b, u32 hint) { void *p = amalloc(0x20); lg(0xa4e90b0); lg(size); lg(a); lg(b); lg(hint); lg(P(p)); return p; }
static u32 __cdecl h_rendergeneric(u32 a, u32 b, u32 c, u32 d) { u32 r = hrnd(); lg(0xa4e0410); lg(a); lg(b); lg(c); lg(d); lg(r); return r; }

/* device recorders */
static long dh(void) { return hrnd() % 4 == 0 ? (long)0x8876086c : hrnd() % 8 == 0 ? 1 : 0; }
static long __stdcall fd_setvp(void *self, const u32 *vp) { int k; lg(0xde500000u | 47); for (k = 0; k < 6; k++) lg(vp[k]); return 0; }
static long __stdcall fd_stretch(void *self, u32 src, const u32 *sr, u32 dst, const u32 *dr, u32 filter) {
  int k; long h = dh(); lg(0xde500000u | 34); lg(src); for (k = 0; k < 4; k++) lg(sr[k]); lg(dst); for (k = 0; k < 4; k++) lg(dr[k]); lg(filter); lg((u32)h); return h;
}
static long __stdcall fd_updsurf(void *self, u32 src, const u32 *sr, u32 dst, const u32 *pt) {
  int k; long h = dh(); lg(0xde500000u | 30); lg(src); for (k = 0; k < 4; k++) lg(sr[k]); lg(dst); lg(pt[0]); lg(pt[1]); lg((u32)h); return h;
}
static long __stdcall fd_getrtdata(void *self, u32 src, u32 dst) { long h = dh(); lg(0xde500000u | 32); lg(src); lg(dst); lg((u32)h); return h; }
static long __stdcall fd_clear(void *self, u32 count, const u32 *rects, u32 flags, u32 color, u32 z, u32 stencil) {
  int k; lg(0xde500000u | 43); lg(count); for (k = 0; k < 4; k++) lg(rects[k]); lg(flags); lg(color); lg(z); lg(stencil); return dh();
}
static long __stdcall fd_colorfill(void *self, u32 surf, const u32 *rect, u32 color) { int k; lg(0xde500000u | 35); lg(surf); for (k = 0; k < 4; k++) lg(rect[k]); lg(color); return dh(); }

enum { ATT, RQUAD, CQUAD, CLRG, ROPEN, RREND, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rwD3D9RasterPluginAttach", 0x4df7b0}, {"rw37_rwD3D9RasterRenderQuad", 0x4e0560}, {"rw37_rwD3D9RasterCopyQuad", 0x4e0ab0},
  {"rw37_D3D9RasterClearGeneric", 0x4e0dd0}, {"rw37_rwD3D9RasterOpen", 0x4dfbd0}, {"rw37_rwD3D9RasterRender", 0x4e03f0}};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32);
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G2)(u32, u32);
static void *fn[2][NF]; static long ncalls[NF];
#define EXTOFF 0x40

static void reset(u32 seed) {
  u32 a;
  nlg = 0; ap = 0; nobj = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng);
  for (a = QUAD; a < 0x7d2810; a += 4) W(a) = 0;      /* quad vertices, current raster target, raster list state: the runs must start equal */
  for (a = 0x947160; a < 0x947380; a += 4) W(a) = 0;
  W(0x947144) = EXTOFF; W(0x9476f4) = 0; W(0x7d2248) = 0; W(0x7d2244) = 0;
  fakedev_log = sink; fakedev_hr = 0; devobj = fakedev_init(); W(DEVVAR) = P(devobj);
  fakedev_vtbl[47] = (void *)fd_setvp; fakedev_vtbl[34] = (void *)fd_stretch; fakedev_vtbl[30] = (void *)fd_updsurf; fakedev_vtbl[32] = (void *)fd_getrtdata;
  fakedev_vtbl[43] = (void *)fd_clear; fakedev_vtbl[35] = (void *)fd_colorfill;
  lockbuf = 0;
}
static u32 rdim(void) { return rnd() % 12 == 0 ? 0 : rnd() % 4 == 0 ? rnd() : 1 + rnd() % 300; }
static u16 roff(void) { return (u16)(rnd() % 4 ? rnd() % 50 : 0xfff0 + rnd() % 8); }
/* one raster block (0xa0 bytes, extension at EXTOFF): own parent, sizes, depth, stride, int16 offsets, type, format byte; ext: texture object (2D or cube by +9), palette
   (low entropy in half of the cases: equal distances), alpha flag, swap chain word */
static u8 *mkras1(u32 type) {
  u8 *r = amalloc(0xa0), *pal = amalloc(0x400); u32 k, low = rnd() % 2, al = rnd() % 3 == 0 ? ((rnd() % 6) << 4) | (1 + rnd() % 15) : (rnd() & 0xf0);
  for (k = 0; k < 0xa0; k++) r[k] = (u8)rnd();
  for (k = 0; k < 0x400; k++) pal[k] = low ? (u8)(rnd() % 4 * 85) : (u8)rnd();
  *(u32 *)r = P(r); *(u32 *)(r + 0xc) = rdim(); *(u32 *)(r + 0x10) = rdim(); *(u32 *)(r + 0x14) = 32; *(u32 *)(r + 0x18) = 4 * (1 + rnd() % 16);
  *(u16 *)(r + 0x1c) = roff(); *(u16 *)(r + 0x1e) = roff(); r[0x20] = (u8)type;
  r[EXTOFF + 9] = (u8)al; *(u32 *)(r + EXTOFF) = P(mkobj((al & 0xf) ? v_texcube : v_tex2d)); *(u32 *)(r + EXTOFF + 4) = P(pal);
  r[EXTOFF + 8] = (u8)(rnd() % 2 ? rnd() : 0); *(u32 *)(r + EXTOFF + 0x1c) = rnd() % 2 ? rnd() : 0;
  return r;
}
static u32 rtype(void) { static const u8 t[] = {0, 1, 2, 2, 4, 5, 5, 3}; return rnd() % 16 == 0 ? rnd() & 0xff : t[rnd() % 8]; }
/* a raster with 0..2 parents above it, every level with its own type and fields */
static u8 *mkchain(u32 type) {
  u8 *r = mkras1(type), *c = r; u32 n = rnd() % 3;
  while (n--) { u8 *p = mkras1(rtype()); *(u32 *)c = P(p); c = p; }
  return r;
}
static u8 *top_of(u8 *r) { while (*(u32 *)r != P(r)) r = (u8 *)(uintptr_t)*(u32 *)r; return r; }
static u32 hashw(const u32 *p, u32 n) { u32 h = 2166136261u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 16777619u; return h; }
static void setcw(u32 cw) { __asm__ volatile("fldcw %0" :: "m"(cw)); }
static void surfaces(void) {
  static const u32 caps[] = {0, 0x1000100, 0x2000200, 0x100, 0x2000000, 0x3000300};
  W(0x7d2248) = P(mkobj(v_surf)); W(0x7d2244) = P(mkobj(v_surf)); W(0x9476f4) = rnd() % 8 == 0 ? rnd() : caps[rnd() % 6];
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 8, k, r = 0;
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: {      /* rwD3D9RasterPluginAttach */
      W(0x947144) = rnd(); for (k = 0x947160; k < 0x947380; k += 4) W(k) = rnd();
      r = ((G0)f[ATT])(); ncalls[ATT]++; lg(r); lg(W(0x947144)); for (k = 0x947160; k < 0x947380; k += 4) lg(W(k));
      W(0x947144) = EXTOFF; break; }
    case 1: case 2: {      /* rwD3D9RasterRenderQuad */
      u8 *src = mkchain(rtype()), *tgt = mkras1(rnd() % 3 == 0 ? 2 : rtype()); int32_t *rect = amalloc(16); u32 cw;
      for (k = 0; k < 4; k++) rect[k] = rnd() % 5 == 0 ? (int32_t)rnd() : (int32_t)(rnd() % 700) - 50;
      for (k = 0; k < 28; k++) W(QUAD + 4 * k) = rnd();
      surfaces(); W(CURTGT) = P(tgt);
      setcw(rnd() % 2 ? 0x7f : 0x27f);
      ((G4)f[RQUAD])(P(src), P(rect), rnd() % 3 ? rnd() % 2 : rnd(), rnd() % 2); ncalls[RQUAD]++;
      __asm__ volatile("fnstcw %0" : "=m"(cw)); lg(cw & 0xffff); setcw(0x27f);
      for (k = 0; k < 28; k++) lg(W(QUAD + 4 * k));
      break; }
    case 3: case 4: {      /* rwD3D9RasterCopyQuad */
      u8 *src = mkchain(rtype()), *tgt = mkchain(rtype()); int32_t *rect = amalloc(16);
      for (k = 0; k < 4; k++) rect[k] = rnd() % 5 == 0 ? (int32_t)rnd() : (int32_t)(rnd() % 700) - 50;
      surfaces(); W(CURTGT) = P(tgt);
      r = ((G3)f[CQUAD])(P(src), P(rect), rnd() % 3 ? rnd() % 2 : rnd()); ncalls[CQUAD]++; lg(r);
      break; }
    case 5: case 6: {      /* D3D9RasterClearGeneric: the lock buffer is w x h pixels of the raster's depth, the rectangle lies inside it (or has a width / height <= 0) */
      static const u32 deps[] = {8, 16, 32, 32, 24}, types[] = {0, 4, 0, 4, 1, 2, 5, 3};
      u8 *tgt = mkchain(rnd() % 12 == 0 ? rnd() & 0xff : types[rnd() % 8]), *top = top_of(tgt), *buf; int32_t *rect = amalloc(16);
      u32 depth = deps[rnd() % 5], w = 1 + rnd() % 12, h = 1 + rnd() % 8, bpp = depth == 8 ? 1 : depth == 16 ? 2 : 4, stride = w * bpp + (rnd() % 3) * 4, x = rnd() % w, y = rnd() % h, pixel;
      buf = amalloc(stride * h + 4); for (k = 0; k < stride * h + 4; k++) buf[k] = (u8)rnd();
      *(u32 *)(tgt + 0x14) = depth; *(u32 *)(tgt + 0x18) = stride; tgt[0x23] = (u8)rnd();
      rect[0] = (int32_t)x; rect[1] = (int32_t)y; rect[2] = rnd() % 6 == 0 ? -(int32_t)(rnd() % 3) : (int32_t)(rnd() % (w - x + 1)); rect[3] = rnd() % 6 == 0 ? -(int32_t)(rnd() % 3) : (int32_t)(rnd() % (h - y + 1));
      pixel = rnd() % 3 == 0 ? *(u32 *)((u8 *)(uintptr_t)*(u32 *)(top + EXTOFF + 4) + 4 * (rnd() % 256)) : rnd();
      surfaces(); W(CURTGT) = P(tgt); lockbuf = buf;
      r = ((G2)f[CLRG])(P(rect), pixel); ncalls[CLRG]++; lg(r); lg(hashw((u32 *)buf, (stride * h + 3) / 4));
      break; }
    default: {     /* _rwD3D9RasterOpen / rwD3D9RasterRender */
      if (rnd() % 2) { W(0x7d280c) = rnd(); r = ((G0)f[ROPEN])(); ncalls[ROPEN]++; lg(r); lg(W(0x7d280c)); }
      else { r = ((G2)f[RREND])(rnd(), rnd()); ncalls[RREND]++; lg(r); }
      break; }
    }
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena;
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
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; long tot = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[64];
  static const u32 t[] = {0x947144, 0x947160, 0x4d7820, 0x4dfb80, 0x4dfba0, CURTGT, 0x7d2248, 0x7d2244, DEVVAR, 0x9476f4,
                          0x4db110, 0x4d7bb0, 0x4d7b80, 0x4d9e60, QUAD,
                          0x4e5070, 0x4e3c50, 0x4e5440, 0x4e5860, 0x4e53a0, 0x4e53f0, 0x4e5be0, 0x4e3c00, 0x4ebb80,
                          0x4ee0d0, 0x4ede80, 0x4dfcd0, 0x4e0040, 0x53eac0, 0x4ea160,
                          0x4e90b0, 0x7d280c, 0x4e0410};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  init_vtbls();
  bind = (void *)GetProcAddress(m, "mflrw_d3d9rasq_bind");
  if (!bind) { printf("mflrw_d3d9rasq_bind MISSING export\n"); return 1; }
  jmp_patch(0x4d7820, (void *)h_regplugin); jmp_patch(0x4db110, (void *)h_rwsetrt); jmp_patch(0x4d7bb0, (void *)h_setrt); jmp_patch(0x4d7b80, (void *)h_setds);
  jmp_patch(0x4d9e60, (void *)h_beginscene); jmp_patch(0x4e5070, (void *)h_rwgetrs); jmp_patch(0x4e3c50, (void *)h_getrs); jmp_patch(0x4e5440, (void *)h_rwsetraster);
  jmp_patch(0x4e5860, (void *)h_valpha); jmp_patch(0x4e53a0, (void *)h_srcblend); jmp_patch(0x4e53f0, (void *)h_dstblend); jmp_patch(0x4e5be0, (void *)h_rwsetrs);
  jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4ebb80, (void *)h_im2d); jmp_patch(0x4ee0d0, (void *)h_pix2rgb); jmp_patch(0x4ede80, (void *)h_rgb2pix);
  jmp_patch(0x4dfcd0, (void *)h_rasterlock); jmp_patch(0x4e0040, (void *)h_rasterunlock); jmp_patch(0x53eac0, (void *)h_errcode); jmp_patch(0x4ea160, (void *)h_errorset);
  jmp_patch(0x4e90b0, (void *)h_flcreate); jmp_patch(0x4e0410, (void *)h_rendergeneric);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind((void *)ENGVAR, tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
