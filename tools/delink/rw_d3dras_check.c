/* rw_d3dras_check.c: behavioural check of the librw DLL's d3dras exports (tools/delink/rw_d3dras.txt: the 7 raster create / lock / destroy functions of
   d3d9raster.obj, librw-mfl capi/d3dras.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3dras_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3dras_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c with the raster slots of fakedev_raster.c (textures, cube textures,
   surfaces), 0x7d2238 the fake IDirect3D9 (CheckDeviceFormat / CheckDepthStencilMatch: one consistent answer table per seed).  The exe callees outside the group
   that create objects or touch other modules (RwErrorSet, _rwerror, RwFreeListCreate, RwRasterCreate / Destroy, the camera-texture / z-buffer creators,
   RwTextureRasterGenerateMipmaps, the raster stage get / set, GetClientRect) get a jmp to a recorder at their original entry; the format checks
   (_rwD3D9CheckValid*Format, CheckAutoMipmapGen*Format) and _rwD3D9CheckRasterSize run as original code on both sides.  RwEngineInstance 0x7d2128 points to a
   fake engine (malloc / realloc / free-list alloc / free on a bump arena).  RwRasterCreate's recorder calls the side's own _rwD3D9RasterCreate.
   Per seed and control word one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the original
   addresses.  Logged after every op: the device / object / recorder calls, the return value, the output, the control word, the touched raster (header + ext),
   a hash of every live raster, the d3d9raster bss block 0x7d27f4..0x7d280c, a walk of the video-memory list and the palette index stack, the stage table.
   Extra arguments va:byte patch the ORIGINAL's code first (negative fixtures: must give DIFF).  Exit 1 on DIFF.
   Classes: every (type, format nibble, palette bits, mipmap bits) request that reaches _rwD3D9RasterCreate / FindCorrectRasterFormat / RwD3D9RasterCreate is
   counted; the summary lists how many non-palettised and palettised classes were run.
   Window 3 (group d3dras2, librw-mfl capi/d3dras2.cpp, rw_d3dras2.txt): the format checks, CheckRasterFormat, the camera-texture / z-buffer creators, mip levels,
   texture set raster, sub raster and the video-memory list release / restore are under test too: d3dras's bind table points the DLL side at the DLL's own
   d3dras2 exports (as in the game when both groups are linked), the original side runs the original code (no recorder on the creators any more). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "fakedev.c"
#include "fakedev_raster.c"
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

/* exe variables (bind table data entries, d3dras.cpp enum B order) */
#define DEVVAR 0x7d2240u
#define D3DVAR 0x7d2238u
#define ADAPTER 0x7d223cu
#define DEVTYPE 0x62e748u
#define HWNDVAR 0x7d2234u
#define DEPTHSURF 0x7d2244u
#define BACKSURF 0x7d2248u
#define ENGVAR 0x7d2128u
#define EXTOFF 0x947144u
#define CAPS 0x947600u
#define ADAPTERFMT 0x9475f0u
#define DISPDEPTH 0x9475f4u
#define ZDEPTH 0x9475fcu
#define DISPFMT 0x947748u
#define DEPTHFMT 0x947768u
#define PFI 0x947160u
#define FMTTAB 0x5ea0f0u
#define PALCOUNT 0x7d27f4u
#define PALCAP 0x7d27f8u
#define PALNEXT 0x7d27fcu
#define PALARRAY 0x7d2800u
#define PALFL 0x7d2804u
#define VIDLIST 0x7d2808u
#define VIDFL 0x7d280cu
#define CLIENTRECT_IAT 0x5dd2b4u
#define PRESENT 0x947740u      /* D3DPRESENT_PARAMETERS (14 words; +8 = DISPFMT, +0x28 = DEPTHFMT) */
#define DISPW 0x9475e4u
#define DISPH 0x9475e8u
#define OOVM ((int32_t)0x8876017c)
#define INVCALL ((int32_t)0x8876086c)
#define T 0xc0de0000u      /* recorder tags */
#define RSZ 0x58u          /* raster + D3D9 extension (RwRaster37 0x34 + ext 0x24) */
#define EXT(r) ((u8 *)(r) + 0x34)

/* log */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }

/* entries under test: [0] DLL export, [1] original */
enum { RCREATE, RDESTROY, LOCK, UNLOCK, FIND, CTEX, DCREATE,
       VTEX, VCAM, VZ, VCUBE, AMTEX, AMCAM, AMCUBE, CRFMT, CAMTEX, ZRAS, GETMIP, TEXSET, SUBRAS, VIDREL, VIDRES, NF };
static const struct { const char *dll; u32 va; } F[NF] = {
  {"rw37__rwD3D9RasterCreate", 0x4e20f0}, {"rw37__rwD3D9RasterDestroy", 0x4e2e40}, {"rw37__rwD3D9RasterLock", 0x4dfcd0}, {"rw37__rwD3D9RasterUnlock", 0x4e0040},
  {"rw37__rwD3D9FindCorrectRasterFormat", 0x4e13f0}, {"rw37_rwD3D9CreateTextureRaster", 0x4e2730}, {"rw37_RwD3D9RasterCreate", 0x4e32c0},
  {"rw37__rwD3D9CheckValidTextureFormat", 0x4e3660}, {"rw37__rwD3D9CheckValidCameraTextureFormat", 0x4e36a0},
  {"rw37__rwD3D9CheckValidZBufferTextureFormat", 0x4e3700}, {"rw37__rwD3D9CheckValidCameraCubeTextureFormat", 0x4e3760},
  {"rw37__rwD3D9CheckAutoMipmapGenTextureFormat", 0x4e37c0}, {"rw37__rwD3D9CheckAutoMipmapGenCameraTextureFormat", 0x4e3810},
  {"rw37__rwD3D9CheckAutoMipmapGenCubeTextureFormat", 0x4e3860}, {"rw37__rwD3D9CheckRasterFormat", 0x4e1ad0},
  {"rw37_rwD3D9CreateCameraTextureRaster", 0x4e2a30}, {"rw37_rwD3D9CreateZBufferRaster", 0x4e2be0}, {"rw37_rwD3D9RasterGetMipLevels", 0x4e3200},
  {"rw37_rwD3D9TextureSetRaster", 0x4e3280}, {"rw37_rwD3D9RasterSubRaster", 0x4e3290}, {"rw37__rxD3D9VideoMemoryRasterListRelease", 0x4e1b20},
  {"rw37__rxD3D9VideoMemoryRasterListRestore", 0x4e1be0}};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NF];
static long ncalls[NF], nok[NF];
static int side;
static volatile int cur_step, cur_op;

/* bump arena (fake engine memory functions); every block has its size in the word before it */
static u8 *arena; static u32 narena;
#define ARENA 0x1000000u
static u8 *bump(u32 n) { u8 *p; n = (n + 15) & ~7u; if (narena + n > ARENA) { printf("ARENA full\n"); fflush(stdout); ExitProcess(6); } p = arena + narena + 8; ((u32 *)p)[-1] = n - 8; narena += n; return p; }
static u32 eng[0x50], vidfl[2], lists[256][2], nlists, stage[8], crect[4];
static void *__cdecl fe_malloc(u32 size, u32 hint) { u8 *p = bump(size); lg(T | 0x108); lg(size); lg(hint); memset(p, 0, size); lg((u32)(uintptr_t)p); return p; }
static void *__cdecl fe_realloc(u8 *old, u32 size, u32 hint) { u8 *p = bump(size); u32 o = ((u32 *)old)[-1]; lg(T | 0x110); lg((u32)(uintptr_t)old); lg(size); lg(hint); memset(p, 0, size); memcpy(p, old, o < size ? o : size); lg((u32)(uintptr_t)p); return p; }
static void *__cdecl fe_alloc(u32 *list, u32 hint) { u8 *p; lg(T | 0x118); lg((u32)(uintptr_t)list); lg(hint); p = bump(list[0]); memset(p, 0xcd, list[0]); lg((u32)(uintptr_t)p); return p; }
static void __cdecl fe_free(u32 list, u32 p) { lg(T | 0x11c); lg(list); lg(p); }

/* recorders at the callee entries */
static u32 *__cdecl st_errorset(u32 *e) { lg(T | 1); lg(e[0]); lg(e[1]); return e; }
static u32 __cdecl st_errcode(u32 code, u32 a1) { lg(T | 2); lg(code); lg(code & 0x80000000u ? 0 : a1); return code + 0x1000; }
static u32 __cdecl st_flcreate(u32 size, u32 n, u32 al, u32 hint) { u32 *l = lists[nlists++ & 255]; lg(T | 3); lg(size); lg(n); lg(al); lg(hint); l[0] = size; return (u32)(uintptr_t)l; }
static u8 *new_raster(u32 w, u32 h, u32 depth) {
  u8 *r = bump(RSZ); u32 i;
  for (i = 0; i < RSZ; i++) r[i] = (u8)(0x5a + i);      /* not zero: the driver has to write every field it relies on */
  *(u32 *)r = (u32)(uintptr_t)r; *(u32 *)(r + 4) = 0; *(u32 *)(r + 8) = 0; *(u32 *)(r + 0xc) = w; *(u32 *)(r + 0x10) = h; *(u32 *)(r + 0x14) = depth;
  *(u16 *)(r + 0x1c) = 0; *(u16 *)(r + 0x1e) = 0; r[0x21] = 0; r[0x22] = 0;
  *(u32 *)(EXT(r) + 0x1c) = 0;                          /* rwD3D9RasterCtor */
  return r;
}
static u32 __cdecl st_rascreate(u32 w, u32 h, u32 depth, u32 flags) {      /* RwRasterCreate 0x4d7950 -> this side's _rwD3D9RasterCreate */
  u8 *r; u32 ok;
  lg(T | 4); lg(w); lg(h); lg(depth); lg(flags);
  r = new_raster(w, h, depth);
  ok = ((G4)fn[side][RCREATE])(0, (u32)(uintptr_t)r, flags, 0);
  lg(T | 0x104); lg(ok);
  return ok ? (u32)(uintptr_t)r : 0;
}
static u32 __cdecl st_rasdestroy(u8 *r) { lg(T | 5); lg((u32)(uintptr_t)r); lg(((G4)fn[side][RDESTROY])(0, (u32)(uintptr_t)r, 0, 0)); return 1; }
/* the DLL side reaches its creators through d3dras's bind table: count them there */
static u32 __cdecl w_camtex(u8 *r, u8 *e) { u32 k = ((u32 (__cdecl *)(u8 *, u8 *))fn[0][CAMTEX])(r, e); ncalls[CAMTEX]++; if (k) nok[CAMTEX]++; return k; }
static u32 __cdecl w_zras(u8 *r, u8 *e) { u32 k = ((u32 (__cdecl *)(u8 *, u8 *))fn[0][ZRAS])(r, e); ncalls[ZRAS]++; if (k) nok[ZRAS]++; return k; }
static void __cdecl st_genmips(u32 r, u32 x) { lg(T | 8); lg(r); lg(x); }
static u32 __cdecl st_getstage(u32 s) { lg(T | 9); lg(s); return s < 8 ? stage[s] : 0; }
static void __cdecl st_setstage(u32 r, u32 s) { lg(T | 10); lg(r); lg(s); if (s < 8) stage[s] = r; }
static long __stdcall st_clientrect(u32 hwnd, u32 *rc) { lg(T | 11); lg(hwnd); memcpy(rc, crect, 16); return 1; }
static const struct { u32 va; void *fn; } STUB[] = {
  {0x4ea160, st_errorset}, {0x53eac0, st_errcode}, {0x4e90b0, st_flcreate}, {0x4d7950, st_rascreate}, {0x4d77e0, st_rasdestroy},
  {0x4d5f60, st_genmips}, {0x4e55c0, st_getstage}, {0x4e5440, st_setstage}};
static void patch_jmp(u32 va, void *f) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)f - (va + 5); }

/* rasters the generator knows */
#define NPOOL 96
static struct { u8 *r; int sub; } pool[NPOOL]; static int npool;
static u32 classes[2][8 * 16 * 4 * 4];      /* [palettised?][type, fmt nibble, pal bits, mip bits] */
static void count_class(u32 type, u32 flags) {
  u32 k = ((type & 7) * 16 + ((flags >> 8) & 0xf)) * 16 + ((flags >> 13) & 3) * 4 + ((flags >> 12) & 1) + ((flags >> 14) & 2);
  if (side == 0) classes[(flags & 0x6000) != 0][k]++;
}

static void reset(u32 seed, unsigned short cw) {
  u32 a, i;
  nlg = 0; rs = seed * 2654435761u + cw + 1; rnd(); rnd();
  narena = 0; nlists = 0; npool = 0;
  fakedev_reset_bufs(); fakedev_hr = 0; fakedev_raster_reset(rnd());
  for (a = PALCOUNT; a <= VIDFL; a += 4) W(a) = 0;
  vidfl[0] = 8; W(VIDFL) = (u32)(uintptr_t)vidfl;
  W(EXTOFF) = 0x34;
  memset((void *)CAPS, 0, 0x130);
  { static const u32 mx[] = { 2048, 4096, 1024, 256 }; W(CAPS + 0x58) = mx[rnd() % 4]; W(CAPS + 0x5c) = mx[rnd() % 4]; }
  W(CAPS + 0x3c) = rnd() & 0x1a2;                       /* POW2 2, SQUAREONLY 0x20, ALPHAPALETTE 0x80, NONPOW2CONDITIONAL 0x100 */
  W(CAPS + 0xc) = rnd() % 2 ? 0x40000000 : 0;
  for (i = 0; i < 14; i++) W(PRESENT + 4 * i) = rnd() % 4 ? rnd() % 8 : rnd();      /* DISPFMT / DEPTHFMT set below */
  W(PRESENT) = rnd() % 4 ? 640 + rnd() % 1400 : rnd() % 4096; W(PRESENT + 4) = rnd() % 4 ? 480 + rnd() % 1000 : rnd() % 4096;
  W(DISPW) = rnd() % 4 ? 320 + rnd() % 2000 : 0; W(DISPH) = rnd() % 4 ? 240 + rnd() % 1500 : 0;
  { static const u32 dd[] = { 16, 32, 32, 24, 15 }, zd[] = { 16, 32, 24, 16 }, af[] = { 0x16, 0x17, 0x15, 0x16 };
    static const u32 df[] = { 0x16, 0x17, 0x15, 0x23, 0x18, 0x19, 0x1a, 0 }, dsf[] = { 0x50, 0x4b, 0x4d, 0x47 };
    W(DISPDEPTH) = dd[rnd() % 5]; W(ZDEPTH) = zd[rnd() % 4]; W(ADAPTERFMT) = af[rnd() % 4];
    W(DISPFMT) = df[rnd() % 8]; if (!W(DISPFMT)) W(DISPFMT) = rnd() % 0x80; W(DEPTHFMT) = dsf[rnd() % 4]; }
  for (i = 0; i < 0x80; i++) { static const u8 dp[] = { 8, 16, 32, 24, 4, 0 }; u8 *p = (u8 *)(uintptr_t)(PFI + 4 * i); p[0] = rnd() % 2; p[1] = dp[rnd() % 6]; *(u16 *)(p + 2) = (u16)(rnd() & 0xff00); }
  W(DEVTYPE) = 1; W(ADAPTER) = rnd() % 2; W(HWNDVAR) = 0x1234;
  W(D3DVAR) = (u32)(uintptr_t)fakedev_d3d9;
  W(BACKSURF) = (u32)(uintptr_t)fakedev_surface_new(640 + rnd() % 1400, 480 + rnd() % 1000, W(DISPFMT), 1);
  W(DEPTHSURF) = (u32)(uintptr_t)fakedev_surface_new(640, 480, W(DEPTHFMT), 2);
  memset(stage, 0, sizeof stage);
}

static u32 hraster(const u8 *r) { u32 h = 0x811c9dc5u, i; for (i = 0; i < RSZ / 4; i++) h = (h ^ ((const u32 *)r)[i]) * 0x01000193u; return h; }
static void dump_raster(const u8 *r) { u32 i; if (!r) { lg(0); return; } for (i = 0; i < RSZ / 4; i++) lg(((const u32 *)r)[i]); }
static u32 walk(void) {      /* video-memory list, palette index stack, stage table, every live raster */
  u32 h = 0x811c9dc5u, n = 0, p; int i;
#define H(v) (h = (h ^ (v)) * 0x01000193u)
  for (p = W(VIDLIST); p && n < 100000; p = ((u32 *)(uintptr_t)p)[1], n++) { H(p); H(((u32 *)(uintptr_t)p)[0]); }
  H(n);
  if (W(PALARRAY)) for (n = 0; n < W(PALCOUNT) && n < 0x100000; n++) H(((u16 *)(uintptr_t)W(PALARRAY))[n]);
  for (i = 0; i < 8; i++) H(stage[i]);
  for (i = 0; i < npool; i++) H(hraster(pool[i].r));
#undef H
  return h;
}
static u8 *top(u8 *r) { u8 *p = *(u8 **)r; while (*(u8 **)p != p) p = *(u8 **)p; return p; }
static int lockable(u8 *r) {      /* the original dereferences these without a check: keep the generator off them */
  u8 *t = top(r); u32 ty = r[0x20] & 7;
  if (*(u32 *)(r + 4)) return 1;                        /* error path */
  if (ty == 0 || ty == 4 || ty == 5) return *(u32 *)EXT(t) != 0;
  return 1;
}
static u32 rflags(void) {      /* a raster create request: type, format nibble, palette / mipmap bits, other cFlags bits */
  u32 q = rnd() % 100, ty = q < 35 ? 4 : q < 55 ? 0 : q < 67 ? 5 : q < 79 ? 1 : q < 91 ? 2 : q < 94 ? 3 : 6 + rnd() % 2;
  u32 nib = rnd() % 3 ? (rnd() % 2 ? 0 : 1 + rnd() % 6) : rnd() % 16, pal = rnd() % 3 ? 0 : (rnd() % 3 + 1) << 13, mip = rnd() % 2 ? 0 : (rnd() % 3 + 1) << 12;
  if (mip == 0x3000) mip = 0x9000; else if (mip == 0x2000) mip = 0x8000;
  return ty | (nib << 8) | pal | mip | (rnd() % 4 == 0 ? 0x80 : 0) | (rnd() % 8 == 0 ? rnd() & 0x78 : 0);
}
static u32 rdim(void) { u32 q = rnd() % 100; return q < 6 ? 0 : q < 50 ? 1u << (rnd() % 12) : q < 90 ? 1 + rnd() % 1024 : 1 + rnd() % 4096; }
static u8 *pick(void) { return npool ? pool[rnd() % npool].r : 0; }
static void pool_add(u8 *r, int sub) { if (r && npool < NPOOL) { pool[npool].r = r; pool[npool].sub = sub; npool++; } }
static void pool_del(u8 *r) { int i; for (i = 0; i < npool; i++) if (pool[i].r == r) { pool[i] = pool[--npool]; return; } }

static void run(int sd, u32 seed, int steps, unsigned short cw) {
  void **f = fn[sd]; int s, lost = 0;
  side = sd;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, a2 = 0, a3 = 0, r, out = 0xeeeeeeee, q = rnd() % 100; int k; u8 *ras = 0;
    k = q < 20 ? RCREATE : q < 27 ? DCREATE : q < 32 ? CTEX : q < 47 ? LOCK : q < 62 ? UNLOCK : q < 71 ? RDESTROY : q < 78 ? FIND : q < 84 ? VTEX + (int)(rnd() % 7) :
        q < 87 ? CRFMT : q < 90 ? GETMIP : q < 91 ? TEXSET : q < 93 ? VIDREL : q < 96 ? VIDRES : -1;
    if ((k == RCREATE || k == DCREATE || k == -1) && npool >= NPOOL) k = RDESTROY;
    if ((k == LOCK || k == UNLOCK || k == RDESTROY || k == CTEX || k == GETMIP || k == TEXSET) && !npool) k = FIND;
    if (lost && k != VIDREL && rnd() % 2) k = VIDRES;      /* a lost device mostly gets reset soon */
    lost = k == VIDREL ? 1 : k == VIDRES ? 0 : lost;
    if (k == VIDREL || k == VIDRES) { int i; for (i = 0; i < npool; i++) if (*(u32 *)(pool[i].r + 4)) k = UNLOCK; }      /* a device reset sees no locked raster */
    /* device state for this op */
    fakedev_texhr = rnd() % 12 == 0 ? (rnd() % 2 ? OOVM : INVCALL) : 0; fakedev_texonce = rnd() % 2; fakedev_offhr = rnd() % 15 == 0 ? OOVM : 0;
    fakedev_levelhr = rnd() % 25 == 0 ? INVCALL : 0; fakedev_surflockhr = rnd() % 20 == 0 ? INVCALL : 0;
    fakedev_dshr = rnd() % 10 == 0 ? (rnd() % 2 ? OOVM : INVCALL) : 0; fakedev_swaphr = rnd() % 10 == 0 ? (rnd() % 2 ? INVCALL : 1) : 0;
    fakedev_fmtflip = rnd() % 30 == 0;
    { static u32 caps0; if (s == 0) caps0 = fakedev_capsseed; fakedev_capsseed = rnd() % 6 ? caps0 : rnd(); }      /* now and then another driver answer table for one op */
    crect[0] = 0; crect[1] = 0; crect[2] = rnd() % 2 ? 4096 : rnd() % 2048; crect[3] = rnd() % 2 ? 4096 : rnd() % 2048;
    if (rnd() % 6 == 0 && npool) stage[rnd() % 8] = (u32)(uintptr_t)pick();
    if (k == -1) {      /* generator step: a sub raster of a live raster (header as RwRasterSubRaster, ext through the original rwD3D9RasterSubRaster) */
      u8 *p = pick(), *sr; u32 w, h, x, y;
      if (!p || !*(u32 *)(p + 0xc) || !*(u32 *)(p + 0x10) || *(u32 *)(p + 4)) { k = FIND; goto op; }
      w = *(u32 *)(p + 0xc); h = *(u32 *)(p + 0x10); x = rnd() % w; y = rnd() % h;
      sr = new_raster(0, 0, 0);
      r = ((G4)f[RCREATE])(0, (u32)(uintptr_t)sr, (p[0x20] & 7) | 0x80, 0);
      lg(0x5b500000u + s); lg(r); dump_raster(sr);
      if (!r) continue;
      *(u32 *)(sr + 0xc) = 1 + rnd() % (w - x); *(u32 *)(sr + 0x10) = 1 + rnd() % (h - y);
      *(u16 *)(sr + 0x1c) = (u16)(*(u16 *)(p + 0x1c) + x); *(u16 *)(sr + 0x1e) = (u16)(*(u16 *)(p + 0x1e) + y);
      r = ((G4)f[SUBRAS])((u32)(uintptr_t)sr, (u32)(uintptr_t)p, 0, 0); lg(r); ncalls[SUBRAS]++; if (r) nok[SUBRAS]++;
      *(u32 *)sr = *(u32 *)p;
      pool_add(sr, 1); dump_raster(sr);
      continue;
    }
  op:
    switch (k) {
    case RCREATE: { u32 fl = rflags(); u32 d = rnd() % 3 ? 0 : rnd() % 33;
      ras = new_raster(rdim(), rdim(), d); a1 = (u32)(uintptr_t)ras; a2 = fl; count_class(fl, fl);
      if (rnd() % 3 == 0) { crect[2] = *(u32 *)(ras + 0xc); crect[3] = *(u32 *)(ras + 0x10); }      /* a window-sized z buffer shares the depth surface */
      break; }
    case VTEX: case VCAM: case VZ: case VCUBE: case AMTEX: case AMCAM: case AMCUBE: {
      static const u32 fm[] = { 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x20, 0x21, 0x28, 0x29, 0x32, 0x33, 0x3c, 0x46, 0x47, 0x49, 0x4b, 0x4d, 0x4f, 0x50, 0x31545844 };
      a0 = rnd() % 8 ? fm[rnd() % (sizeof fm / 4)] : rnd(); break; }
    case CRFMT: ras = new_raster(rdim(), rdim(), rnd() % 33); a0 = (u32)(uintptr_t)ras; a1 = rflags() | (rnd() % 4 == 0 ? rnd() & 0xffff00ffu : 0); count_class(a1, a1); break;
    case GETMIP: case TEXSET: a0 = (u32)(uintptr_t)&out; ras = pick(); a1 = (u32)(uintptr_t)ras; a2 = rnd() % 3; break;
    case VIDREL: case VIDRES: break;
    case DCREATE: { static const u32 fm[] = { 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x20, 0x21, 0x28, 0x29, 0x29, 0x32, 0x33, 0x3c, 0x46, 0x47, 0x48, 0x49, 0x4b,
                                                 0x4d, 0x4f, 0x50, 0x31545844, 0x32545844, 0x33545844, 0x34545844, 0x35545844, 0x7f, 0x80, 0x200 };
      a0 = rdim(); a1 = rdim(); if (!a0) a0 = 1; if (!a1) a1 = 1;
      a2 = rnd() % 10 ? fm[rnd() % (sizeof fm / 4)] : rnd() % 0x90; a3 = rflags(); count_class(a3, a3); break; }
    case CTEX: { int i, n = 0; u8 *c[NPOOL];
      for (i = 0; i < npool; i++)      /* (size 0 becomes 0x80000000 in _rwD3D9CheckRasterSize, and a negative size never ends there: as the original, not reached by the game) */
        if (!pool[i].sub && (pool[i].r[0x20] == 0 || pool[i].r[0x20] == 4) && !*(u32 *)EXT(pool[i].r) && *(u32 *)(EXT(pool[i].r) + 0x18) - 0x46 > 0xa      /* not a released depth texture: the restore would make it a plain one again */
            && *(int32_t *)(pool[i].r + 0xc) > 0 && *(int32_t *)(pool[i].r + 0x10) > 0)
          c[n++] = pool[i].r;
      if (!n) { k = FIND; goto op; }
      ras = c[rnd() % n];
      EXT(ras)[9] = rnd() % 4 == 0 ? (u8)((rnd() % 6) << 4 | 1) : 0;
      if (rnd() % 4 == 0) EXT(ras)[0xa] = (u8)(EXT(ras)[0xa] ^ 1);
      if (rnd() % 6 == 0) ras[0x23] = (u8)rnd();       /* any format byte, palettised ones included */
      count_class(ras[0x20], (u32)ras[0x23] << 8);
      a0 = (u32)(uintptr_t)ras; a1 = (u32)(uintptr_t)EXT(ras); break; }
    case LOCK: { int t;
      for (t = 0; t < 8; t++) { ras = pick(); if (lockable(ras)) break; }
      if (t == 8) { k = FIND; goto op; }
      { u32 qq = rnd() % 100, lv = qq < 70 ? 0 : qq < 92 ? 1 + rnd() % 4 : rnd() % 256;
        a0 = (u32)(uintptr_t)&out; a1 = (u32)(uintptr_t)ras; a2 = lv << 8 | rnd() % 4; }
      break; }
    case UNLOCK: { int t;
      for (t = 0; t < 4; t++) { ras = pick(); if (*(u32 *)(ras + 4)) break; }
      a1 = (u32)(uintptr_t)ras; break; }
    case RDESTROY: ras = pick(); a1 = (u32)(uintptr_t)ras; break;
    case FIND: { u32 qq = rnd() % 20; a0 = qq < 16 ? qq % 6 : qq < 19 ? 6 + rnd() % 2 : rnd(); a1 = rflags() | (rnd() % 4 == 0 ? rnd() & 0xffff00ffu : 0);
      count_class(a0, a1); break; }
    }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1); lg(a2); lg(a3);
    cur_step = s; cur_op = k;
    cw_set(cw);
    r = ((G4)f[k])(a0, a1, a2, a3); if (k == VIDREL) r = 0;      /* void */
    ncalls[k]++; if (r) nok[k]++;
    lg(r); lg(cw_now()); lg(out);
    cw_set(0x27f);
    switch (k) {      /* generator state */
    case RCREATE:
      if (r) {
        pool_add(ras, 0);
        if ((ras[0x20] & 7) == 2 && rnd() % 3 == 0) {      /* a window camera: its own swap chain, its back buffer, its window */
          *(void **)EXT(ras) = fakedev_surface_new(*(u32 *)(ras + 0xc), *(u32 *)(ras + 0x10), W(DISPFMT), 1);
          *(void **)(EXT(ras) + 0x1c) = fakedev_swapchain_new(*(u32 *)(ras + 0xc), *(u32 *)(ras + 0x10), W(DISPFMT)); *(u32 *)(EXT(ras) + 0x20) = 0x777 + npool;
        }
      }
      break;
    case DCREATE: ras = (u8 *)(uintptr_t)r; pool_add(ras, 0); break;
    case CTEX: if (r && rnd() % 2) ras[0x21] &= 0x7f; break;
    case RDESTROY: pool_del(ras); break;
    }
    if (ras) dump_raster(ras); else lg(0);
    if (ras && k != RDESTROY) dump_raster(top(ras));
    for (u32 a = PALCOUNT; a <= VIDFL; a += 4) lg(a == VIDFL ? 0 : W(a));
    lg(walk());
    if (npool && rnd() % 8 == 0) {      /* a cube raster turns to another face (6 = invalid, the fake fails it) */
      u8 *c = pick(), *e = EXT(top(c));
      if ((e[9] & 0xf) && *(u32 *)e && ((struct fdr_obj *)*(void **)e)->kind == FDR_CUBE) e[9] = (u8)((rnd() % 7) << 4 | (e[9] & 0xf));
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
    printf("  seed %u cw %x: log differs at entry %u (step %u op %s): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, op < NF ? F[op].dll : "?",
           i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    if (bad_dumps++ < 2) for (k = i > 24 ? i - 24 : 0; k < i + 8 && k < n; k++) printf("    %6u  %08x %08x%s\n", k, save_log[k], LG[k], save_log[k] != LG[k] ? "  <" : "");
    return 1;
  }
  return 0;
}
static void sink(u32 w) { lg(w); }
static volatile int cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %d side %d step %d op %d\n", cur_seed, side, cur_step, cur_op); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH %08lx at %p seed %d side %d step %d op %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed, side, cur_step, cur_op); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0, ci, seed0 = 1; u32 seed; long tot = 0;
  if (argc > 3 && strchr(argv[3], '-')) { seed0 = atoi(argv[3]); seeds = atoi(strchr(argv[3], '-') + 1); }      /* seeds "a-b": only seeds a..b */
  static const unsigned short cws[2] = { 0x7f, 0x27f };
  int (__cdecl *bind)(int, void *const *);
  static void *table[44];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)(seeds - seed0 + 1) * (u32)steps / 2), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init(); fakedev_raster_init();
  eng[0x108 / 4] = (u32)(uintptr_t)fe_malloc; eng[0x110 / 4] = (u32)(uintptr_t)fe_realloc;
  eng[0x118 / 4] = (u32)(uintptr_t)fe_alloc; eng[0x11c / 4] = (u32)(uintptr_t)fe_free; W(ENGVAR) = (u32)(uintptr_t)eng;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  W(CLIENTRECT_IAT) = (u32)(uintptr_t)st_clientrect;
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  for (i = 0; i < NF; i++) fn[1][i] = (void *)(uintptr_t)F[i].va;
  /* self-test of the fake device: the original format choice and a texture raster create against it give a non-empty call log and no trap */
  side = 1; reset(1, 0x7f);
  ((G4)fn[1][FIND])(4, 0x500, 0, 0); { u8 *t = new_raster(64, 64, 0); ((G4)fn[1][RCREATE])(0, (u32)(uintptr_t)t, 0x504, 0); }
  printf("fakedev selftest: original FindCorrectRasterFormat / RasterCreate logged %u words (%s)\n", nlg, nlg ? "ok" : "EMPTY");
  if (!nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3dras_bind");
  if (!bind) { printf("mflrw_d3dras_bind MISSING export\n"); return 1; }
  { static const u32 data[] = { DEVVAR, D3DVAR, ADAPTER, DEVTYPE, HWNDVAR, DEPTHSURF, BACKSURF, ENGVAR, EXTOFF, CAPS, ADAPTERFMT, DISPDEPTH, ZDEPTH, DISPFMT,
                                DEPTHFMT, PFI, FMTTAB, PALCOUNT, PALCAP, PALNEXT, PALARRAY, PALFL, VIDLIST, VIDFL, 0x62e8b4, 0x62e8d0, 0x62e8f0,
                                0x4ea160, 0x53eac0, 0x4e90b0, 0x4d7950, 0x4d77e0, 0x4eddd0, 0x4e3660, 0x4e36a0, 0x4e3700, 0x4e37c0, 0x4e3810,
                                0x4e2a30, 0x4e2be0, 0x4d5f60, 0x4e55c0, 0x4e5440 };
    for (i = 0; i < NF; i++) { fn[0][i] = (void *)GetProcAddress(m, F[i].dll); if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
    if (bad) return 1;
    for (i = 0; i < 43; i++) table[i] = (void *)(uintptr_t)data[i];
    table[43] = (void *)st_clientrect;
    /* d3dras2's functions: the DLL's own (the game links both groups) */
    table[33] = fn[0][VTEX]; table[34] = fn[0][VCAM]; table[35] = fn[0][VZ]; table[36] = fn[0][AMTEX]; table[37] = fn[0][AMCAM];
    table[38] = (void *)w_camtex; table[39] = (void *)w_zras; }
  if (bind(43, table)) { printf("mflrw_d3dras_bind accepted a short table (DIFF)\n"); return 1; }
  if (!bind(44, table)) { printf("mflrw_d3dras_bind refused the table (DIFF)\n"); return 1; }
  { static void *t2[26]; int (__cdecl *bind2)(int, void *const *) = (void *)GetProcAddress(m, "mflrw_d3dras2_bind");
    static const u32 data2[] = { DEVVAR, D3DVAR, ADAPTER, DEVTYPE, HWNDVAR, DEPTHSURF, EXTOFF, CAPS, ADAPTERFMT, DISPDEPTH, ZDEPTH, DEPTHFMT, PFI, FMTTAB,
                                 VIDLIST, PRESENT, DISPW, DISPH, 0x62e938, 0x62e96c, 0x62e988, 0x4ea160, 0x53eac0, 0x4eddd0 };
    if (!bind2) { printf("mflrw_d3dras2_bind MISSING export\n"); return 1; }
    for (i = 0; i < 24; i++) t2[i] = (void *)(uintptr_t)data2[i];
    t2[24] = fn[0][FIND]; t2[25] = (void *)st_clientrect;
    if (bind2(25, t2)) { printf("mflrw_d3dras2_bind accepted a short table (DIFF)\n"); return 1; }
    if (!bind2(26, t2)) { printf("mflrw_d3dras2_bind refused the table (DIFF)\n"); return 1; } }
  printf("DLL %s, %d seeds x %d steps x CW 0x7f / 0x27f\n", argv[1], seeds, steps);
  for (ci = 0; ci < 2; ci++)
    for (seed = (u32)seed0; seed <= (u32)seeds; seed++) {
      cur_seed = seed;
      run(0, seed, steps, cws[ci]);
      nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
      run(1, seed, steps, cws[ci]);
      tot += nlg;
      if (compare(seed, cws[ci]) && ++bad >= 5) goto done;
    }
done:
  for (i = 0; i < NF; i++) printf("%-45s %9ld calls (both sides), %9ld returned non-zero\n", F[i].dll, ncalls[i], nok[i]);
  { int c, n[2] = { 0, 0 }; for (c = 0; c < 8 * 16 * 4 * 4; c++) { n[0] += classes[0][c] != 0; n[1] += classes[1][c] != 0; }
    printf("request classes run: %d non-palettised, %d palettised (type x format nibble x palette bits x mipmap bits)\n", n[0], n[1]); }
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
