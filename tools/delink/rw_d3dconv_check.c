/* rw_d3dconv_check.c: bit-exact check of the librw DLL's d3dconv group (tools/delink/rw_d3dconv.txt, librw-mfl capi/d3dconv.cpp: the 12 still-original
   image <-> raster conversion functions of d3d9convrt.obj) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.
   Made from rw_pixfmt_check.c (same image mapping: MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000).
   No device is involved: every callee of the group is an exe function, patched here to a logging stand-in (RwErrorSet, RwRasterLock / LockPalette /
   Unlock / UnlockPalette, _rwD3D9CheckValidTextureFormat, _rwD3D9CheckRasterFormat, RwImageCreateResample / Create / AllocatePixels / Destroy, the five
   RwPalQuant functions).  The stand-ins log their arguments (pointers as arena offsets, the palette quantiser's stack object as a tag) and act
   deterministically on the arena (lock = set cpPixels / palette and flip privateFlags bits, image create = bump allocator, quantiser = fixed patterns).
   The per-pixel converters stay the exe's own code (both sides call them through the original addresses; the DLL gets them in its bind table).
   usage: rw_d3dconv_check.exe <mflrw.dll> <MFL.exe> [N=20000] [name,name|all] [neg]
   Per entry N random cases: rasters (depth 4/8/16/24/32/other, every format nibble, every D3D format the converter tables know, sub rasters with a
   parent, locked / unlocked, compressed flag, palette or none), images (depth 4/8/32/other, alpha classes: opaque, 1 bit, 4 bit, 8 bit), caps
   (pow2 / square / nonpow2conditional, max sizes), display depth 16 / 32, _rwD3D9PixelFormatInfo alpha bytes, stand-in failures.  Compared per case:
   the whole 64 KB arena (objects, pixels, palettes, images the stand-ins made), the return value (5 entries return one), the stand-in call log.
   Inputs the original cannot take are not generated: width 0 in the do-while loops of Image32GetFrom*Raster (2^32 iterations), a D3D format without
   a converter where one is called (null call), negative sizes in _rwD3D9CheckRasterSize (endless loop), 4 bit pixel values above 15 (they index
   uninitialised lookup entries), non-palettised raster depths in D3D9RasterPalletizedSetFromImage's quantise path (endless recursion).
   neg = negative fixture (negtest.sh): the DLL side gets bytes 1 <-> 3 of every 4 byte group of its pixel / palette inputs swapped (w <-> h for
   _rwD3D9CheckRasterSize); every entry but D3D9SelectConvertFn (no pixel input, "neg n/a") must report DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8; typedef int32_t i32;

enum { K_UNCONV, K_SIZE, K_FMT, K_RASFMT, K_SELECT, K_G32D3D, K_G32, K_GET, K_PAL, K_2432, K_SET };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_D3D9ConvA1R5G5B5ToR8G8B8A8", 0x4eecd0, K_UNCONV}, {"rw37_D3D9ConvA4R4G4B4ToR8G8B8A8", 0x4eed10, K_UNCONV},
  {"rw37__rwD3D9CheckRasterSize", 0x4eddd0, K_SIZE}, {"rw37__rwD3D9ImageFindFormat", 0x4eef60, K_FMT}, {"rw37__rwD3D9ImageFindRasterFormat", 0x4ef150, K_RASFMT},
  {"rw37_D3D9SelectConvertFn", 0x4ef870, K_SELECT}, {"rw37_D3D9Image32GetFromD3D9Raster", 0x4ee920, K_G32D3D}, {"rw37_D3D9Image32GetFromRaster", 0x4ee4b0, K_G32},
  {"rw37__rwD3D9ImageGetFromRaster", 0x4ee2d0, K_GET}, {"rw37_D3D9RasterPalletizedSetFromImage", 0x4efce0, K_PAL},
  {"rw37_D3D9Raster24_32LinearSetFromImage", 0x4f03f0, K_2432}, {"rw37__rwD3D9RasterSetFromImage", 0x4ef240, K_SET}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef u32 (__cdecl *F2)(void *, u32);
typedef u32 (__cdecl *F3)(void *, void *, u32);
typedef void (__cdecl *Bind)(i32 n, void *const *t);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
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
#define G32(va) (*(u32 *)(uintptr_t)(va))

/* arena: every object the functions touch */
#define AN 0x10000
static u8 A[AN] __attribute__((aligned(16))), snap[AN], after0[AN];
#define RAS0 (A + 0x000)
#define RAS1 (A + 0x080)
#define IMG  (A + 0x100)
#define WH   (A + 0x140)
#define RPIX (A + 0x1000)
#define RPAL (A + 0x2800)
#define IPIX (A + 0x3000)
#define IPAL (A + 0x4800)
#define BUMP0 0x5000
static u32 bump;
static void *balloc(u32 n) { void *p = A + bump; bump = (bump + n + 15) & ~15u; if (bump > AN) { printf("CRASH (cannot continue) arena full\n"); ExitProcess(4); } memset(p, 0, n); return p; }
/* raster / image fields */
#define R_PARENT(r) (*(u8 **)((r) + 0x00))
#define R_PIX(r) (*(u8 **)((r) + 0x04))
#define R_PAL(r) (*(u8 **)((r) + 0x08))
#define R_W(r) (*(i32 *)((r) + 0x0c))
#define R_H(r) (*(i32 *)((r) + 0x10))
#define R_D(r) (*(i32 *)((r) + 0x14))
#define R_S(r) (*(i32 *)((r) + 0x18))
#define R_TYPE(r) ((r)[0x20])
#define R_PRIV(r) ((r)[0x22])
#define R_FMT(r) ((r)[0x23])
#define X_ALPHA(r) ((r)[0x34 + 8])
#define X_COMP(r) ((r)[0x34 + 0xa])
#define X_MIP(r) ((r)[0x34 + 0xb])
#define X_D3D(r) (*(u32 *)((r) + 0x34 + 0x18))
#define I_FLAGS(i) (*(i32 *)((i) + 0x00))
#define I_W(i) (*(i32 *)((i) + 0x04))
#define I_H(i) (*(i32 *)((i) + 0x08))
#define I_D(i) (*(i32 *)((i) + 0x0c))
#define I_S(i) (*(i32 *)((i) + 0x10))
#define I_PIX(i) (*(u8 **)((i) + 0x14))
#define I_PAL(i) (*(u8 **)((i) + 0x18))

/* stand-in behaviour of the current case (the same for both sides) */
static struct { int failLock, failLockPal, failCreate, failPq; u8 flipLock, flipPal; u32 validMask; i32 rasFmtRet; u8 pqAlpha; } S;
static u32 lg[2][2048]; static int ln[2], side; static void *pqs[2];
static void L(u32 v) { if (ln[side] < 2048) lg[side][ln[side]] = v; ln[side]++; }
static u32 P(const void *p) { uintptr_t u = (uintptr_t)p; if (!u) return 0; if (u >= (uintptr_t)A && u < (uintptr_t)A + AN) return 0x40000000u | (u32)(u - (uintptr_t)A); if (p == pqs[side]) return 0x7777; return 0x66660000u; }
static void *__cdecl s_errorset(i32 *e) { L(1); L(e[0]); L(e[1]); return e; }
static u8 *__cdecl s_lock(u8 *r, i32 level, i32 mode) { L(2); L(P(r)); L(level); L(mode); if (S.failLock) return 0; R_PIX(r) = RPIX; R_PRIV(r) ^= S.flipLock; return RPIX; }
static u8 *__cdecl s_lockpal(u8 *r, i32 mode) { L(3); L(P(r)); L(mode); if (S.failLockPal) return 0; R_PAL(r) = RPAL; R_PRIV(r) ^= S.flipPal; return RPAL; }
static u8 *__cdecl s_unlock(u8 *r) { L(4); L(P(r)); return r; }
static u8 *__cdecl s_unlockpal(u8 *r) { L(5); L(P(r)); return r; }
static i32 __cdecl s_validfmt(i32 f) { L(6); L(f); return (S.validMask >> (f & 31)) & 1; }
static i32 __cdecl s_rasfmt(u8 *r, u32 flags) { L(7); L(P(r)); L(flags); L(R_W(r)); L(R_H(r)); L(R_D(r)); return S.rasFmtRet; }
static int bpp(int d) { return d <= 8 ? 1 : d == 16 ? 2 : 4; }
static u8 *__cdecl s_resample(u8 *img, i32 w, i32 h) {
  u8 *n = balloc(0x1c); int d = I_D(img), k, s = w * bpp(d);
  L(8); L(P(img)); L(w); L(h);
  I_FLAGS(n) = 1; I_W(n) = w; I_H(n) = h; I_D(n) = d; I_S(n) = s; I_PIX(n) = balloc(s * h + 4);
  for (k = 0; k < s * h; k++) I_PIX(n)[k] = (u8)((k * 13 + w * 7 + h) & (d == 4 ? 0xf : 0xff));
  if (d <= 8) { I_PAL(n) = balloc(1024); memcpy(I_PAL(n), I_PAL(img), 1024); }
  return n;
}
static u8 *__cdecl s_imgcreate(i32 w, i32 h, i32 d) { u8 *n; L(9); L(w); L(h); L(d); if (S.failCreate) return 0; n = balloc(0x1c); I_W(n) = w; I_H(n) = h; I_D(n) = d; return n; }
static u8 *__cdecl s_imgalloc(u8 *n) { L(10); L(P(n)); I_S(n) = (((I_D(n) + 7) >> 3) * I_W(n) + 3) & ~3; I_PIX(n) = balloc(I_S(n) * I_H(n) + 4); I_PAL(n) = I_D(n) <= 8 ? balloc(1024) : 0; I_FLAGS(n) |= 1; return n; }
static i32 __cdecl s_imgdestroy(u8 *n) { L(11); L(P(n)); return 1; }
static i32 __cdecl s_pqinit(void *pq) { L(12); pqs[side] = pq; L(P(pq)); return !S.failPq; }
static void __cdecl s_pqadd(void *pq, u8 *img, float w) { L(13); L(P(pq)); L(P(img)); L(*(u32 *)&w); }
static i32 __cdecl s_pqresolve(u8 *pal, i32 n, void *pq) { int i; L(14); L(P(pal)); L(n); L(P(pq)); for (i = 0; i < 4 * n; i++) pal[i] = (i & 3) == 3 && S.pqAlpha ? 0xff : (u8)(i * 29 + 5); return n; }
static void __cdecl s_pqmatch(u8 *px, i32 stride, i32 depth, i32 dither, void *pq, u8 *img) {
  int k; L(15); L(P(px)); L(stride); L(depth); L(dither); L(P(pq)); L(P(img));
  for (k = 0; k < stride * I_H(img); k++) px[k] = (u8)((k * 7 + 3) & ((1 << depth) - 1));
}
static void __cdecl s_pqterm(void *pq) { L(16); L(P(pq)); }

static volatile int cur_fi, cur_case;
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

/* D3D formats: with a pack converter (D3D9SelectConvertFn), with an unpack converter (D3D9Image32GetFromD3D9Raster) */
static const u32 TOX[] = {0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1d, 0x1e, 0x32, 0x33, 0x51, 0x3c, 0x75, 0x3d, 0x20, 0x3f, 0x21, 0x3e};
static const u32 TORGBA[] = {0x18, 0x17, 0x19, 0x1a, 0x32, 0x1c, 0x16, 0x15, 0x1d, 0x1b, 0x1e, 0x34, 0x33, 0x51, 0x3c, 0x75, 0x3d, 0x20, 0x3f, 0x21, 0x3e};
#define PICK(a) (a[rnd() % (sizeof a / sizeof a[0])])
static const int RDEP[] = {4, 8, 16, 32};

static void fill(u8 *p, int n) { while (n--) *p++ = (u8)rnd(); }
/* alpha classes: 0 opaque, 1 some alpha 0..15, 2 some alpha > 15, 3 random */
static void alphas(u8 *p, int n4, int cls) { int i; for (i = 0; i < n4; i++) { u32 r = rnd(); p[4 * i + 3] = cls == 3 ? (u8)r : r % 5 ? 0xff : cls == 1 ? (u8)(r >> 8 & 0xf) : cls == 2 ? (u8)(0x10 + (r >> 8) % 0xef) : 0xff; } }
static void gen_ras(u8 *r, int depth, int w, int h, u8 fmt, u32 d3d) {
  int s = w * 4 + (rnd() % 3 ? 0 : rnd() % 9);
  fill(r, 0x58);
  if (depth <= 8 && rnd() % 3 == 0) s = w;          /* 8 bit raster with stride == width (one memcpy) */
  R_PIX(r) = RPIX; R_PAL(r) = rnd() % 4 ? RPAL : 0; R_W(r) = w; R_H(r) = h; R_D(r) = depth; R_S(r) = s;
  R_TYPE(r) = (u8)(rnd() % 3 ? 4 : rnd() % 6); R_PRIV(r) = (u8)rnd(); R_FMT(r) = fmt;
  X_ALPHA(r) = (u8)(rnd() % 2); X_COMP(r) = rnd() % 8 ? (u8)(rnd() & 0x0f) : (u8)(0x10 | (rnd() & 0xff)); X_MIP(r) = (u8)(rnd() % 3 ? 0 : rnd() % 3); X_D3D(r) = d3d;
}
static void gen_img(int depth, int w, int h) {
  int s = w * bpp(depth) + (rnd() % 3 ? 0 : rnd() % 9), cls = rnd() % 4, k;
  if (depth <= 8 && rnd() % 3 == 0) s = w;
  fill(IMG, 0x1c); I_W(IMG) = w; I_H(IMG) = h; I_D(IMG) = depth; I_S(IMG) = s; I_PIX(IMG) = IPIX; I_PAL(IMG) = IPAL;
  fill(IPIX, 0x1800); fill(IPAL, 0x400);
  if (depth == 4) for (k = 0; k < 0x1800; k++) IPIX[k] &= 0xf;
  if (depth > 8) alphas(IPIX, 0x600, cls); else alphas(IPAL, 0x100, cls);
}
static void gen_globals(void) {
  int k;
  G32(0x947144) = 0x34;                                     /* _RwD3D9RasterExtOffset */
  G32(0x94763c) = rnd() & 0x122;                            /* TextureCaps: POW2 2, SQUAREONLY 0x20, NONPOW2CONDITIONAL 0x100 */
  G32(0x947658) = rnd() % 3 ? 2048 : rnd() % 5000; G32(0x94765c) = rnd() % 3 ? 2048 : rnd() % 5000;
  G32(0x9475f4) = rnd() % 2 ? 32 : 16;                      /* display depth */
  for (k = 0; k < 0x200; k++) ((u8 *)0x947160)[k] = rnd() % 3 ? (rnd() % 2 ? 0xff : 0) : (u8)rnd();   /* _rwD3D9PixelFormatInfo alpha bytes */
  S.failLock = rnd() % 10 == 0; S.failLockPal = rnd() % 10 == 0; S.failCreate = rnd() % 10 == 0; S.failPq = rnd() % 10 == 0;
  S.flipLock = rnd() % 2 ? 0 : (u8)rnd(); S.flipPal = rnd() % 2 ? 0 : (u8)rnd(); S.validMask = rnd(); S.rasFmtRet = rnd() % 3 ? 1 : 0; S.pqAlpha = rnd() % 2;
}
/* neg fixture input buffers: swap bytes 1 <-> 3 of every 4 byte group */
static struct { u8 *p; int n, restore; } NB[4]; static int nnb;
static void nb(u8 *p, int n, int restore) { NB[nnb].p = p; NB[nnb].n = n; NB[nnb].restore = restore; nnb++; }
static void swap13(u8 *p, int n) { int k; for (k = 0; k + 3 < n; k += 4) { u8 t = p[k + 1]; p[k + 1] = p[k + 3]; p[k + 3] = t; } }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 20000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  static void *tab[59]; int k;
  static const u32 tox_va[15] = {0x4ef990, 0x4ef9c0, 0x4ef9f0, 0x4efa30, 0x4efa70, 0x4efaa0, 0x4efad0, 0x4efb00, 0x4efb30, 0x4efb60, 0x4efb90, 0x4efbc0, 0x4efbd0, 0x4efc30, 0x4efc80};
  static const u32 torgba_va[18] = {0x4eec40, 0x4eec70, 0x4eeca0, 0x4eecd0, 0x4eed10, 0x4eed60, 0x4eed90, 0x4eedc0, 0x4eedf0, 0x4eee10, 0x4eee30, 0x4eee50, 0x4eee80, 0x4eeeb0, 0x4eeed0, 0x4eeef0, 0x4eef20, 0x4eef40};
  static const u32 fix_va[26] = {0x4ea160, 0x53eac0, 0x4d79f0, 0x4d7850, 0x4d7790, 0x4d77b0, 0x4e3660, 0x4e1ad0, 0x4eb400, 0x4dd550, 0x4dd610, 0x4dd5b0,
                                 0x4e8f10, 0x4e7520, 0x4e7a00, 0x4e89c0, 0x4e8fc0, 0x947144, 0x947160, 0x5ea0f0, 0x9475f4, 0x947600, 0x62e9f4, 0x62ea30, 0x62ea78, 0x62eab0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  jmp_patch(0x4ea160, (void *)s_errorset); jmp_patch(0x4d79f0, (void *)s_lock); jmp_patch(0x4d7850, (void *)s_lockpal); jmp_patch(0x4d7790, (void *)s_unlock);
  jmp_patch(0x4d77b0, (void *)s_unlockpal); jmp_patch(0x4e3660, (void *)s_validfmt); jmp_patch(0x4e1ad0, (void *)s_rasfmt); jmp_patch(0x4eb400, (void *)s_resample);
  jmp_patch(0x4dd550, (void *)s_imgcreate); jmp_patch(0x4dd610, (void *)s_imgalloc); jmp_patch(0x4dd5b0, (void *)s_imgdestroy); jmp_patch(0x4e8f10, (void *)s_pqinit);
  jmp_patch(0x4e7520, (void *)s_pqadd); jmp_patch(0x4e7a00, (void *)s_pqresolve); jmp_patch(0x4e89c0, (void *)s_pqmatch); jmp_patch(0x4e8fc0, (void *)s_pqterm);
  for (k = 0; k < 26; k++) tab[k] = (void *)(uintptr_t)fix_va[k];
  for (k = 0; k < 15; k++) tab[26 + k] = (void *)(uintptr_t)tox_va[k];
  for (k = 0; k < 18; k++) tab[41 + k] = (void *)(uintptr_t)torgba_va[k];
  bind = (Bind)GetProcAddress(m, "mflrw_d3dconv_bind");
  if (!bind) { printf("mflrw_d3dconv_bind MISSING export\n"); return 1; }
  bind(59, tab);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets bytes 1 <-> 3 of its pixel / palette inputs swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0, kind = T[fi].kind; long cov[3] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-42s MISSING export\n", T[fi].name); bad_total++; continue; }
    if (neg && kind == K_SELECT) { printf("%-42s neg n/a\n", T[fi].name); continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N; i++) {
      u32 a0 = 0, a1 = 0, a2 = 0, ret[2]; int w = 1 + rnd() % 12, h = rnd() % 13, d, side_;
      cur_fi = fi; cur_case = i; nnb = 0;
      memset(A, 0, BUMP0); gen_globals(); fill(RPIX, 0x1800); fill(RPAL, 0x400);
      switch (kind) {
      case K_UNCONV: a0 = (u32)(uintptr_t)WH; a1 = rnd() % 4 ? rnd() & 0xffff : rnd(); nb((u8 *)&a1, 4, 1); break;
      case K_SIZE: { i32 *p = (i32 *)WH; p[0] = rnd() % 4 ? rnd() % 5000 : 1 << rnd() % 13; p[1] = rnd() % 4 ? rnd() % 5000 : 1 << rnd() % 13; if (rnd() % 16 == 0) p[rnd() % 2] = 0;
        a0 = (u32)(uintptr_t)p; a1 = (u32)(uintptr_t)(p + 1); a2 = (rnd() % 8) | (rnd() % 2 ? 0x8000 : 0) | (rnd() & 0x7f00); break; }
      case K_FMT: case K_RASFMT: { static const int ID[] = {4, 8, 32, 32, 16}; d = ID[rnd() % 5]; gen_img(d, rnd() % 13, h); nb(IPIX, 0x1800, 1); nb(IPAL, 0x400, 1);
        if (kind == K_FMT) a0 = (u32)(uintptr_t)IMG; else { gen_ras(RAS0, PICK(RDEP), w, h, (u8)rnd(), PICK(TOX)); R_PARENT(RAS0) = RAS0; a0 = (u32)(uintptr_t)RAS0; a1 = (u32)(uintptr_t)IMG; a2 = (rnd() % 8) | (rnd() & 0xff00) | (rnd() % 4 ? 0 : rnd() & 0x70000); }
        break; }
      case K_SELECT: a0 = rnd() % 4 ? rnd() % 0x80 : rnd(); break;
      case K_G32D3D: case K_G32: case K_GET: {
        static const int GD[] = {4, 8, 16, 32, 24, 4, 8, 16, 32, 12}; u8 fmt = kind == K_G32D3D ? (u8)rnd() : rnd() % 3 ? (u8)(rnd() % 16 | (rnd() & 0x60)) : 0; u32 f;
        d = GD[rnd() % 10];
        f = PICK(TORGBA);
        if (kind != K_G32D3D && (fmt & 0xf) && (d == 24 || d == 32) && rnd() % 2) { static const u32 F3L[] = {0x15, 0x16, 0x23}; f = PICK(F3L); }   /* 0x23: no unpack converter, only with a format nibble */
        if (kind == K_GET && rnd() % 2) { d = 8; if (rnd() % 2) fmt |= 0x01; }
        gen_ras(RAS1, d, w, h, fmt, f); gen_ras(RAS0, d, w, h, fmt, f); R_PARENT(RAS1) = RAS1; R_PARENT(RAS0) = rnd() % 3 ? RAS0 : RAS1;
        if (kind == K_GET) { static const int IDG[] = {32, 8, 8, 4, 16}; gen_img(IDG[rnd() % 5], w, h); if (rnd() % 3 == 0) I_S(IMG) = R_S(RAS0); }
        else { gen_img(32, w, h); if (I_S(IMG) < 4 * w) I_S(IMG) = 4 * w; }
        if (I_D(IMG) == 32 && I_S(IMG) < 4 * w) I_S(IMG) = 4 * w;
        nb(RPIX, 0x1800, 1); nb(RPAL, 0x400, 1);
        a0 = (u32)(uintptr_t)IMG; a1 = (u32)(uintptr_t)RAS0; break; }
      case K_PAL: case K_2432: case K_SET: {
        static const int ID2[] = {4, 8, 32, 32, 16}; static const int SD[] = {4, 8, 16, 24, 32, 4, 8, 16, 32, 0x40, 12};
        u8 nib = (u8)(rnd() % 16), fmt; u32 f = PICK(TOX); int iw = w, ih = h;
        d = kind == K_PAL ? (rnd() % 2 ? 4 : 8) : kind == K_2432 ? (rnd() % 2 ? 24 : 32) : SD[rnd() % 11];
        if (kind == K_PAL && rnd() % 3 == 0) nib = 4;
        if (kind == K_2432 && rnd() % 2) nib = rnd() % 2 ? 5 : 6;
        if (kind == K_SET && (d == 4 || d == 8) && rnd() % 4 == 0) f = 0x29;
        if (kind == K_PAL && nib == 4) { static const u32 LF[] = {0x1b, 0x34, 0x1c, 0x32, 0x29}; f = PICK(LF); }
        if (kind == K_SET && rnd() % 4 == 0) { iw = 1 + rnd() % 12; ih = rnd() % 13; }
        fmt = (u8)(nib | (rnd() % 3 ? 0 : rnd() & 0xf0));
        gen_ras(RAS1, d, w, h, fmt, f); gen_ras(RAS0, d, w, h, fmt, f); R_PARENT(RAS1) = RAS1; R_PARENT(RAS0) = rnd() % 3 ? RAS0 : RAS1;
        if (kind == K_SET && rnd() % 2) R_TYPE(RAS0) = 0;
        gen_img(ID2[rnd() % 5], iw, ih);
        nb(IPIX, 0x1800, 1); nb(IPAL, 0x400, 1);
        a0 = (u32)(uintptr_t)RAS0; a1 = (u32)(uintptr_t)IMG; a2 = rnd(); break; }
      }
      memcpy(snap, A, AN);
      for (side_ = 0; side_ < 2; side_++) {
        void *fn = side_ ? orig : dll; u32 r = 0; int b;
        side = side_; memcpy(A, snap, AN); ln[side] = 0; bump = BUMP0; pqs[side] = 0;
        if (neg && !side) { if (kind == K_SIZE) { i32 *p = (i32 *)WH, t = p[0]; p[0] = p[1]; p[1] = t; } else for (b = 0; b < nnb; b++) swap13(NB[b].p, NB[b].n); }
        switch (kind) {
        case K_UNCONV: ((F2)fn)((void *)(uintptr_t)a0, a1); break;
        case K_FMT: r = ((F1)fn)(a0); break;
        case K_SELECT: r = ((F1)fn)(a0); break;
        case K_G32D3D: case K_G32: case K_PAL: case K_2432: ((F2)fn)((void *)(uintptr_t)a0, a1); break;
        case K_GET: r = ((F2)fn)((void *)(uintptr_t)a0, a1); break;
        default: r = ((F3)fn)((void *)(uintptr_t)a0, (void *)(uintptr_t)a1, a2);
        }
        if (kind == K_SIZE) r = 0;
        if (neg && !side && kind != K_SIZE) for (b = 0; b < nnb; b++) if (NB[b].restore) swap13(NB[b].p, NB[b].n);
        ret[side] = r;
        if (!side) memcpy(after0, A, AN);
      }
      cov[ret[1] != 0]++; if (ln[1] > 0 && lg[1][0] == 1) cov[2]++;
      if (kind == K_UNCONV) { /* the output bytes are the arena; the input word is a value */ }
      if (ret[0] == ret[1] && ln[0] == ln[1] && !memcmp(lg[0], lg[1], sizeof(u32) * (ln[0] < 2048 ? ln[0] : 2048)) && !memcmp(after0, A, AN)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, log %d / %d", T[fi].name, i, ret[0], ret[1], ln[0], ln[1]);
        for (k = 0; k < ln[0] && k < ln[1] && k < 2048; k++) if (lg[0][k] != lg[1][k]) { printf(", log word %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k < AN; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(A + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(A + k)); break; }
        printf("\n");
      }
    }
    printf("%-42s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage ret0 %ld ret!=0 %ld error-first %ld\n", cov[0], cov[1], cov[2]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
