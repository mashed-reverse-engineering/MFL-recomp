/* rw_d3dim2d_check.c: behavioural check of the librw DLL's d3dim2d exports (tools/delink/rw_d3dim2d.txt: the 17 functions of d3d92drend.obj and
   d3d9vertexbuffer.obj, librw-mfl capi/d3dim2d.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3dim2d_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3dim2d_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c (GetAvailableTextureMem, Create{Vertex,Index}Buffer,
   Draw[Indexed]Primitive, buffer objects with Release / Lock / Unlock over one scratch arena; any other method = TRAP, the run fails).
   The exe callees outside the group (RwRasterGetCurrentContext, _rwD3D9SetVertexDeclaration, SetVertexShader / PixelShader, RwD3D9SetStreamSource, SetIndices,
   RwD3D9Create / DeleteVertexDeclaration, RwD3D9SetRenderState / SetTextureStageState, the render-state flush, GetRasterStage, RwFreeListCreate / Destroy) get a
   jmp to a recorder at their original entry, and RwEngineInstance 0x7d2128 points to a fake engine whose free-list alloc / free (+0x118 / +0x11c) are recorders on a
   bump arena; so both sides make the same calls into the same recorders.  The DLL is bound (mflrw_d3dim2d_bind) to the same exe state.
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the
   original addresses: the 17 entries mixed with the 2 C functions of the module (RwD3D9DynamicVertexBufferDestroy / Unlock, original bytes on both sides).
   Logged after every op: the device / buffer / recorder calls (with the hash of the bytes written into each locked range), the return value (not for the void
   functions), the outputs, the control word, the whole bss block 0x7d51c8..0x7d522c (2D buffers, base index, ring offsets / sizes / buffers, discard slot),
   a walk of the stride and dynamic buffer lists, the variables the dynamic buffers write back to.  Extra arguments va:byte patch the ORIGINAL's code first
   (negative fixtures: must give DIFF).  Exit 1 on DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "fakedev.c"
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
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

#define DEVVAR 0x7d2240u
#define ENGVAR 0x7d2128u
#define BSS0 0x7d51c8u
#define BSS1 0x7d522cu
#define CAPS 0x947600u
#define DEVCAPS 0x94761cu
#define DEFVB 0x62e9d0u
#define CAMVAR 0x9473c0u
#define PRIMTAB 0x5ea28cu
#define OWNVB 0x7d51c8u
#define VBVAR 0x7d51d0u
#define IBVAR 0x7d51d8u
#define STRIDES 0x7d51e0u
#define DYNLIST 0x7d51f0u
#define MGR(n) (0x7d521cu + 4 * (n))
#define T 0xc0de0000u      /* recorder tags */

/* log */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }

/* recorders at the callee entries, the fake engine, the bump arena */
static u8 *arena; static u32 narena;
#define ARENA 0x800000u
static u32 eng[0x50];
static u32 lists[64][4], nlists, ndecl, nflush, rstage_val, decl_ok, ctxras;
static void *__cdecl fe_alloc(u32 list, u32 hint) { u8 *p; lg(T | 0x118); lg(list); lg(hint); if (narena + 0x20 > ARENA) { printf("ARENA full\n"); ExitProcess(6); } p = arena + narena; narena += 0x20; memset(p, 0xcd, 0x20); lg((u32)(uintptr_t)p); return p; }
static void __cdecl fe_free(u32 list, u32 p) { lg(T | 0x11c); lg(list); lg(p); }
static u32 __cdecl st_getctx(void) { lg(T | 1); return ctxras; }
static void __cdecl st_setdecl(u32 d) { lg(T | 2); lg(d); }
static void __cdecl st_setvs(u32 d) { lg(T | 3); lg(d); }
static void __cdecl st_setps(u32 d) { lg(T | 4); lg(d); }
static void __cdecl st_setstream(u32 a, u32 b, u32 c, u32 d) { lg(T | 5); lg(a); lg(b); lg(c); lg(d); }
static void __cdecl st_setindices(u32 d) { lg(T | 6); lg(d); }
static u32 __cdecl st_createdecl(const u32 *e, u32 *out) { int i; lg(T | 7); for (i = 0; i < 8; i++) lg(e[i]); *out = decl_ok ? 0xdec10000u + ++ndecl : 0; return *out != 0; }
static void __cdecl st_deletedecl(u32 d) { lg(T | 8); lg(d); }
static void __cdecl st_setrs(u32 s, u32 v) { lg(T | 9); lg(s); lg(v); }
static u32 __cdecl st_tss(u32 st, u32 ty, u32 v) { lg(T | 10); lg(st); lg(ty); lg(v); return 0; }
static u32 __cdecl st_flush(void) { lg(T | 11); return 0xf1000000u + ++nflush; }
static u32 __cdecl st_getstage(u32 st) { lg(T | 12); lg(st); return rstage_val; }
static u32 __cdecl st_flcreate(u32 a, u32 b, u32 c, u32 d) { u32 *l = lists[nlists++ & 63]; lg(T | 13); lg(a); lg(b); lg(c); lg(d); l[0] = a; return (u32)(uintptr_t)l; }
static void __cdecl st_fldestroy(u32 l) { lg(T | 14); lg(l); }
static const struct { u32 va; void *fn; } STUB[] = {
  {0x4d7750, st_getctx}, {0x4db1b0, st_setdecl}, {0x4db1f0, st_setvs}, {0x4db230, st_setps}, {0x4db270, st_setstream}, {0x4db400, st_setindices},
  {0x4db9e0, st_createdecl}, {0x4dbbc0, st_deletedecl}, {0x4e3c00, st_setrs}, {0x4e3c70, st_tss}, {0x4e3b30, st_flush}, {0x4e55c0, st_getstage},
  {0x4e90b0, st_flcreate}, {0x4e92b0, st_fldestroy}};
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

/* entries: the 17 under test ([0] DLL export, [1] original) and the 2 C functions (original on both sides) */
enum { OPEN2, CLOSE2, LINE, TRI, FLUSH, PRIM, IPRIM, CVB, DVB, FDISC, DREL, DREST, MCREATE, DCREATE, DLOCK, VOPEN, VCLOSE, NF,
       C_DDESTROY = NF, C_DUNLOCK, NALL };
static const struct { const char *dll; u32 va; int isvoid; } F[NALL] = {
  {"rw37__rwD3D9Im2DRenderOpen", 0x4eb500, 0}, {"rw37__rwD3D9Im2DRenderClose", 0x4eb670, 1}, {"rw37_rwD3D9Im2DRenderLine", 0x4eb6d0, 0},
  {"rw37_rwD3D9Im2DRenderTriangle", 0x4eb960, 0}, {"rw37_rxD3D9Im2DRenderFlush", 0x4eb890, 0}, {"rw37__rwD3D9Im2DRenderPrimitive", 0x4ebb80, 0},
  {"rw37_rwD3D9Im2DRenderIndexedPrimitive", 0x4ebdb0, 0}, {"rw37_RwD3D9CreateVertexBuffer", 0x4ec6d0, 0}, {"rw37_RwD3D9DestroyVertexBuffer", 0x4ec870, 1},
  {"rw37__rwD3D9DynamicVertexBufferManagerForceDiscard", 0x4ec9c0, 1}, {"rw37__rwD3D9DynamicVertexBufferRelease", 0x4eca00, 1},
  {"rw37__rwD3D9DynamicVertexBufferRestore", 0x4eca90, 0}, {"rw37_D3D9DynamicVertexBufferManagerCreate", 0x4ecb00, 0},
  {"rw37_RwD3D9DynamicVertexBufferCreate", 0x4ecbc0, 0}, {"rw37_RwD3D9DynamicVertexBufferLock", 0x4eccd0, 0},
  {"rw37__rwD3D9VertexBufferManagerOpen", 0x4ece70, 0}, {"rw37__rwD3D9VertexBufferManagerClose", 0x4ecee0, 1},
  {"RwD3D9DynamicVertexBufferDestroy", 0x4ecca0, 1}, {"RwD3D9DynamicVertexBufferUnlock", 0x4ece50, 0}};
typedef u32 (__cdecl *G5)(u32, u32, u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL];

static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
/* objects outside the exe state */
#define NVERT 12000
static u8 *verts, *rasters, *cam; static u16 *idx; static u32 vars[16], outs[4];
static struct { u32 stride, size, vb, off; } blk[256]; static int nblk;
static int hw, open2d, released;

static u32 rfloat(void) {
  static const u32 sp[] = { 0x7fc00000, 0x7f800001, 0xff800000, 0x00000001, 0x80000000, 0x3f800001, 0x4b000000, 0x47000000 };
  float f;
  if (rnd() % 40 == 0) return sp[rnd() % 8];
  f = (float)((int)(rnd() % 20000) - 10000) / (float)(1 + rnd() % 64);
  return *(u32 *)&f;
}
static void setras(u8 *r) {      /* nOffsetX / nOffsetY (int16 at 0x1c / 0x1e) */
  u32 q = rnd() % 10;
  *(u16 *)(r + 0x1c) = q < 6 ? 0 : (u16)(rnd() % 2 ? rnd() % 9 : rnd());
  *(u16 *)(r + 0x1e) = q < 6 ? 0 : q < 8 ? (u16)(rnd() % 9 - 4) : 0;
}
static void reset(u32 seed, unsigned short cw) {
  u32 a; int i;
  nlg = 0; rs = seed * 2654435761u + cw + 1;
  for (a = BSS0; a < BSS1; a += 4) W(a) = 0;
  hw = rnd() % 2;
  memset((void *)CAPS, 0, 0x130);
  W(DEVCAPS) = (hw ? 0x10000 : 0) | (rnd() & 0xfffeffffu);
  { static const u32 dv[] = { 0x20000, 0x400, 0x1000, 0x100, 0x20000, 0x8000 }; W(DEFVB) = dv[rnd() % 6]; }
  narena = 0; nlists = 0; ndecl = 0; nflush = 0; nblk = 0; open2d = 0; released = 0;
  memset(vars, 0, sizeof vars); memset(outs, 0, sizeof outs);
  fakedev_reset_bufs(); fakedev_hr = 0;
  for (i = 0; i < NVERT * 7; i++) ((u32 *)verts)[i] = rnd() % 3 ? rfloat() : rnd();
  for (i = 0; i < NVERT; i++) idx[i] = (u16)i;
  memset(rasters, 0, 0x100); memset(cam, 0, 0x100);
  *(u32 *)(cam + 0x60) = (u32)(uintptr_t)rasters; ctxras = (u32)(uintptr_t)(rasters + 0x80);
  W(CAMVAR) = (u32)(uintptr_t)cam;
}
static u32 walk(void) {      /* the stride lists and the dynamic buffer list (cap: 20000 nodes) */
  u32 h = 0x811c9dc5u, s, f, n = 0;
#define H(v) (h = (h ^ (v)) * 0x01000193u)
  for (s = W(STRIDES); s && n < 20000; s = W(s + 12), n++) {
    H(s); H(W(s)); H(W(s + 4)); H(W(s + 8));
    for (f = W(s + 4); f && n < 20000; f = W(f + 12), n++) { H(f); H(W(f)); H(W(f + 4)); H(W(f + 8)); H(W(f + 16)); }
    for (f = W(s + 8); f && n < 20000; f = W(f + 4), n++) { H(f); H(W(f)); }
  }
  for (s = W(DYNLIST); s && n < 20000; s = W(s + 16), n++) { H(s); H(W(s)); H(W(s + 4)); H(W(s + 8)); H(W(s + 12)); }
  H(n);
  for (n = 0; n < 16; n++) H(vars[n]);
#undef H
  return h;
}
static u32 anybuf(void) {      /* a live buffer for RwD3D9DynamicVertexBufferUnlock, 0 if there is none */
  u32 c[8], n = 0, i;
  for (i = 0; i < 4; i++) if (W(MGR(i))) c[n++] = W(MGR(i));
  if (W(VBVAR)) c[n++] = W(VBVAR);
  if (W(IBVAR)) c[n++] = W(IBVAR);
  return n ? c[rnd() % n] : 0;
}
static u32 cnt(void) { u32 q = rnd() % 100; return q < 60 ? 1 + rnd() % 64 : q < 90 ? 1 + rnd() % 2000 : q < 98 ? 1 + rnd() % 9400 : 9000 + rnd() % 3000; }

static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, r; int k, i;
    int render_ok = W(OWNVB) ? W(VBVAR) != 0 : !released;
    if (s == 0) k = VOPEN;
    else if (s == 1) k = OPEN2;
    else {
      u32 q = rnd() % 100;
      k = q < 8 ? LINE : q < 16 ? TRI : q < 34 ? PRIM : q < 48 ? IPRIM : q < 50 ? FLUSH : q < 52 ? OPEN2 : q < 54 ? CLOSE2 : q < 60 ? CVB : q < 66 ? DVB
        : q < 72 ? FDISC : q < 74 ? DREL : q < 77 ? DREST : q < 78 ? MCREATE : q < 82 ? DCREATE : q < 92 ? DLOCK : q < 93 ? VOPEN : q < 94 ? VCLOSE
        : q < 97 ? C_DDESTROY : C_DUNLOCK;
      if ((k == LINE || k == TRI || k == PRIM || k == IPRIM) && !render_ok) k = FDISC;
      if ((k == DCREATE || k == DLOCK || k == MCREATE) && released) k = DREST;
    }
    fakedev_hr = rnd() % 10 == 0 ? (int32_t)0x8876086c : rnd() % 20 == 0 ? 1 : 0;
    fakedev_lockhr = rnd() % 25 == 0 ? (int32_t)0x8876021c : 0;
    fakedev_createhr = 0;
    fakedev_texmem = rnd() % 3 ? 0x4000000 : rnd() % 2 ? 0x800000 : 0x800001;
    rstage_val = rnd() % 2 ? 0 : 0x1234;
    decl_ok = rnd() % 10 != 0;
    setras(rasters); setras(rasters + 0x80);
    W(CAMVAR) = (u32)(uintptr_t)cam;
    switch (k) {
    case OPEN2: if (rnd() % 8 == 0) fakedev_createhr = (int32_t)0x8876017c; break;
    case LINE: { u32 n = 2 + rnd() % 30; a0 = (u32)(uintptr_t)(verts + 0x1c * (rnd() % 1000)); a1 = n; a2 = rnd() % n; a3 = rnd() % n; break; }
    case TRI: { u32 n = 3 + rnd() % 30; a0 = (u32)(uintptr_t)(verts + 0x1c * (rnd() % 1000)); a1 = n; a2 = rnd() % n; a3 = rnd() % n; a4 = rnd() % n; break; }
    case PRIM: { u32 n = cnt(); if (n > NVERT) n = NVERT; a0 = rnd() % 6; a1 = (u32)(uintptr_t)(verts + 0x1c * (rnd() % (NVERT - n + 1))); a2 = n;
      if (rnd() % 8 == 0) W(CAMVAR) = 0; break; }
    case IPRIM: { u32 nv = 1 + (rnd() % 3 ? rnd() % 64 : rnd() % 4000), ni = rnd() % 3 ? 1 + rnd() % (2 * nv + 2) : cnt(), j;
      if (ni > NVERT) ni = NVERT;
      for (j = 0; j < ni; j++) idx[j] = (u16)(rnd() % nv);
      a0 = rnd() % 6; a1 = (u32)(uintptr_t)verts; a2 = nv; a3 = (u32)(uintptr_t)idx; a4 = ni; break; }
    case CVB: a0 = 4 * (1 + rnd() % 16); a1 = rnd() % 4 ? a0 * (1 + rnd() % 64) : 1 + rnd() % 0x3000; a2 = (u32)(uintptr_t)&outs[0]; a3 = (u32)(uintptr_t)&outs[1];
      if (rnd() % 12 == 0) fakedev_createhr = (int32_t)0x8876017c;
      outs[0] = outs[1] = 0xeeeeeeee; break;
    case DVB:
      if (nblk && rnd() % 6) { i = rnd() % nblk; a0 = blk[i].stride; a1 = blk[i].size; a2 = blk[i].vb; a3 = blk[i].off; if (rnd() % 5) blk[i] = blk[--nblk]; }
      else { a0 = 4 * (1 + rnd() % 16); a1 = 1 + rnd() % 0x400; a2 = nblk ? blk[rnd() % nblk].vb : 0x12345678; a3 = rnd() % 0x4000; }
      break;
    case DCREATE: { static const u32 sz[] = { 0x40000, 0x1000, 0x800, 0x40000, 0x2000, 0x100 }; a0 = sz[rnd() % 6]; a1 = (u32)(uintptr_t)&vars[rnd() % 16]; break; }
    case DLOCK: a0 = rnd() % 3 ? 0x1c : 4 * (1 + rnd() % 16); a1 = cnt(); if (a0 * a1 > 0x300000) a1 = 0x300000 / a0;
      a2 = (u32)(uintptr_t)&outs[0]; a3 = (u32)(uintptr_t)&outs[1]; a4 = (u32)(uintptr_t)&outs[2]; outs[0] = outs[1] = outs[2] = 0xeeeeeeee; break;
    case C_DDESTROY: { u32 b = anybuf(); a0 = rnd() % 3 && b ? b : vars[rnd() % 16]; break; }
    case C_DUNLOCK: a0 = anybuf(); if (!a0) k = FDISC; break;
    }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1); lg(a2); lg(a3); lg(a4);
    cw_set(cw);
    r = ((G5)f[k])(a0, a1, a2, a3, a4); ncalls[k]++;
    lg(F[k].isvoid ? 0 : r); lg(cw_now());
    cw_set(0x27f);
    for (i = 0; i < 4; i++) lg(outs[i]);
    for (u32 a = BSS0; a < BSS1; a += 4) lg(W(a));
    lg(walk());
    switch (k) {      /* generator state: what the next ops may touch without a null buffer */
    case OPEN2: open2d = r != 0; break;
    case CLOSE2: open2d = 0; break;
    case CVB: if (r && nblk < 256) { blk[nblk].stride = a0; blk[nblk].size = a1; blk[nblk].vb = outs[0]; blk[nblk].off = outs[1]; nblk++; } break;
    case DREL: if (hw) released = 1; break;
    case DREST: case VOPEN: case VCLOSE: released = 0; if (k != DREST) nblk = 0; break;
    }
  }
}

static u32 *save_log, nsave; static int bad_dumps;
static int compare(u32 seed, unsigned short cw) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u cw %x: log differs at entry %u (step %u op %s): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, op < NALL ? F[op].dll : "?",
           i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    if (bad_dumps++ < 2) for (k = i > 24 ? i - 24 : 0; k < i + 8 && k < n; k++) printf("    %6u  %08x %08x%s\n", k, save_log[k], LG[k], save_log[k] != LG[k] ? "  <" : "");
    return 1;
  }
  return 0;
}
static void sink(u32 w) { lg(w); }
static volatile int cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %d\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH %08lx at %p seed %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0, ci; u32 seed; long tot = 0;
  static const unsigned short cws[2] = { 0x7f, 0x27f };
  int (__cdecl *bind)(int, void *const *);
  static void *table[21];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); verts = malloc(NVERT * 0x1c); idx = malloc(NVERT * 2); rasters = malloc(0x100); cam = malloc(0x100);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  eng[0x118 / 4] = (u32)(uintptr_t)fe_alloc; eng[0x11c / 4] = (u32)(uintptr_t)fe_free; W(ENGVAR) = (u32)(uintptr_t)eng;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test of the fake device: the original VB manager open + Im2D open + a primitive against it give a non-empty call log and no trap */
  reset(1, 0x7f); ((G5)(uintptr_t)F[VOPEN].va)(0, 0, 0, 0, 0); ((G5)(uintptr_t)F[OPEN2].va)(0, 0, 0, 0, 0); ((G5)(uintptr_t)F[PRIM].va)(3, (u32)(uintptr_t)verts, 6, 0, 0);
  printf("fakedev selftest: original VB open / Im2D open / primitive logged %u words (%s)\n", nlg, nlg ? "ok" : "EMPTY");
  if (!nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3dim2d_bind");
  if (!bind) { printf("mflrw_d3dim2d_bind MISSING export\n"); return 1; }
  table[0] = (void *)DEVVAR; table[1] = (void *)CAPS; table[2] = (void *)ENGVAR; table[3] = (void *)BSS0; table[4] = (void *)DEFVB; table[5] = (void *)CAMVAR;
  table[6] = (void *)PRIMTAB;
  for (i = 0; i < 14; i++) table[7 + i] = (void *)(uintptr_t)STUB[i].va;
  if (!bind(21, table)) { printf("mflrw_d3dim2d_bind refused the table (DIFF)\n"); return 1; }
  for (i = 0; i < NALL; i++) {
    fn[1][i] = (void *)(uintptr_t)F[i].va;
    fn[0][i] = i < NF ? (void *)GetProcAddress(m, F[i].dll) : fn[1][i];
    if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; }
  }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps x CW 0x7f / 0x27f\n", argv[1], seeds, steps);
  for (ci = 0; ci < 2; ci++)
    for (seed = 1; seed <= (u32)seeds; seed++) {
      cur_seed = seed;
      run(0, seed, steps, cws[ci]);
      nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
      run(1, seed, steps, cws[ci]);
      tot += nlg;
      if (compare(seed, cws[ci]) && ++bad >= 5) goto done;
    }
done:
  for (i = 0; i < NALL; i++) printf("%-55s %9ld calls (both sides)\n", F[i].dll, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
