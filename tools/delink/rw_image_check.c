/* rw_image_check.c: bit-exact check of the librw DLL's image kernels (tools/delink/rw_image.txt, librw-mfl capi/rwimage.cpp: RwImageMakeMask,
   RwImageApplyMask, ImageStraightCopy, ImageConvertDepth, RwImageCopy, RwImageGammaCorrect) against the ORIGINAL code of MFL.exe, both run
   natively (Wine) on the same CPU.  Made from rw_tristrip_check.c (same image mapping and harness heap: RwEngineInstance 0x7d2128 -> harness
   engine, malloc / free / free-list alloc / free bump-allocate from one arena and log every call; RwErrorSet 0x4ea160 and
   _rwPluginRegistryInitObject 0x4e9ce0 / DeInitObject 0x4e9d40 -> loggers).  imageModule 0x7d2724 points at a harness block in the engine
   (image free list, random gamma table at +0xc).  RwImageAllocatePixels runs as original code on both sides (the DLL reaches it through
   mflrw_image_bind).  Each side rebuilds the case from the same seed (images in the arena), then calls its function; compared: the used arena
   incl. slack, the call log, the return value.
   Inputs: RwImages of depth 4 / 8 / 32 (and invalid depths for the error paths), 1..40 x 1..20 pixels, strides with padding, 256 entry
   palettes (4 bit index bytes mostly < 16), owned / not owned pixels, matching and mismatching mask sizes, 1 in 10 with a failing allocation.
   RwImageCopy gets only supported depth pairs (the original dereferences NULL after the error for the others).
   usage: rw_image_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (restored after the call); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_MAKEMASK, K_APPLYMASK, K_STRAIGHT, K_CONVERT, K_COPY, K_GAMMA };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_RwImageMakeMask", 0x4dd700, K_MAKEMASK}, {"rw37_RwImageApplyMask", 0x4dd7d0, K_APPLYMASK}, {"rw37_ImageStraightCopy", 0x4de3a0, K_STRAIGHT},
  {"rw37_ImageConvertDepth", 0x4de450, K_CONVERT}, {"rw37_RwImageCopy", 0x4de560, K_COPY}, {"rw37_RwImageGammaCorrect", 0x4de880, K_GAMMA}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef u32 (__cdecl *F2)(u32, u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *, void *, void *);

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
#define HSZ (8u << 20)
#define LGMAX (1u << 16)
static u8 *heap, *save0; static u32 hp, nalloc, failAt, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static u8 eng[0x400], eng0[0x400], engs[0x400];
#define MODOFF 0x200
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static void *__cdecl hmalloc(u32 n, u32 hint) { logw(0xa1100000u); logw(n); logw(hint); if (++nalloc == failAt || n > (1u << 20)) { logw(0); return 0; } { void *p = halloc(n); logw((u32)(uintptr_t)p); return p; } }
static void __cdecl hfree(void *p) { logw(0xa1200000u); logw((u32)(uintptr_t)p); }
static void *__cdecl hflalloc(u32 *fl, u32 hint) { logw(0xa1300000u); logw((u32)(uintptr_t)fl); logw(hint); if (++nalloc == failAt) { logw(0); return 0; } { void *p = halloc(fl[1]); logw((u32)(uintptr_t)p); return p; } }
static void *__cdecl hflfree(void *fl, void *p) { logw(0xa1400000u); logw((u32)(uintptr_t)fl); logw((u32)(uintptr_t)p); return fl; }
static void *__cdecl herr(int32_t *e) { logw(0xa1700000u); logw(e[0]); logw(e[1]); return e; }
static void *__cdecl hinit(void *reg, void *obj) { logw(0xa1800000u); logw((u32)(uintptr_t)reg); logw((u32)(uintptr_t)obj); return obj; }
static void *__cdecl hdeinit(void *reg, void *obj) { logw(0xa1900000u); logw((u32)(uintptr_t)reg); logw((u32)(uintptr_t)obj); return obj; }

/* RwImage {flags, width, height, depth, stride, pixels, palette} */
static const int32_t depths[] = {4, 8, 32, 32, 8, 4, 16, 24};
static u32 *mkimage(int32_t w, int32_t h, int32_t depth, int pix) {
  u32 *im = halloc(28); int32_t bpp = depth == 32 ? 4 : 1, stride = w * bpp + (rnd() % 3 ? 0 : 4 * (rnd() % 4)), y, x;
  im[0] = (rnd() % 2) | (rnd() % 4 == 0 ? 2 : 0) | (rnd() & 0xf0); im[1] = w; im[2] = h; im[3] = depth; im[4] = stride; im[5] = 0; im[6] = 0;
  if (pix && rnd() % 16) {
    u8 *p = halloc(stride * h + 4);
    for (y = 0; y < h; y++) for (x = 0; x < w * bpp; x++) p[y * stride + x] = depth == 4 && rnd() % 8 ? (u8)(rnd() % 16) : (u8)rnd();
    im[5] = (u32)(uintptr_t)p;
    if ((depth == 4 || depth == 8) && rnd() % 16) im[6] = (u32)(uintptr_t)halloc(1024);
  }
  return im;
}

static u8 *swapA, *swapB; static int swapn = 4;
static void swapw(void) { u32 t; if (!swapA) return; memcpy(&t, swapA, swapn); memcpy(swapA, swapB, swapn); memcpy(swapB, &t, swapn); }

static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, *a, *b; int32_t w, h, da, db; int k;
  rs = seed; hseed = seed * 2654435761u; hp = 0; nalloc = 0; failAt = 0; ln[side] = 0; swapA = swapB = 0; swapn = 4;
  memcpy(eng, eng0, sizeof eng);
  { u32 *fl = halloc(8); fl[0] = 0xf1f1f1f1u; fl[1] = 0x40; *(u32 *)(eng + MODOFF) = (u32)(uintptr_t)fl; for (k = 0; k < 256; k++) eng[MODOFF + 0xc + k] = rnd() % 4 ? (u8)(255 - k) : (u8)rnd(); }
  w = 1 + rnd() % 40; h = 1 + rnd() % 20; if (rnd() % 16 == 0) h = 0;
  da = depths[rnd() % (kind == K_COPY || kind == K_CONVERT || kind == K_STRAIGHT ? 6 : 8)];      /* 16 / 24 (error paths) only where the original tests the depth */
  db = depths[rnd() % 6];
  if (kind == K_COPY) { static const int32_t ok[][2] = {{4, 4}, {8, 8}, {32, 32}, {4, 8}, {8, 8}, {4, 32}, {8, 32}}; int j = rnd() % 7; db = ok[j][0]; da = ok[j][1]; }
  a = mkimage(w, h, da, 1);
  cov[0] = da == 32; cov[1] = 0;
  switch (kind) {
  case K_MAKEMASK: case K_GAMMA:
    if (kind == K_MAKEMASK) {     /* MakeMask does not test pixels / palette (the original crashes on NULL too) */
      if (!a[5]) a[5] = (u32)(uintptr_t)halloc(a[4] * h + 4);
      if ((da == 4 || da == 8) && !a[6]) a[6] = (u32)(uintptr_t)halloc(1024);
    }
    swapn = 1;      /* red <-> alpha of the first pixel / palette entry */
    if (a[5]) { swapA = (u8 *)(uintptr_t)a[5]; swapB = swapA + 3; }
    if (a[6]) { swapA = (u8 *)(uintptr_t)a[6]; swapB = swapA + 3; }
    if (neg && isdll) swapw();
    r = ((F1)fn)((u32)(uintptr_t)a);
    if (neg && isdll) swapw();
    cov[1] = !a[5];
    break;
  case K_APPLYMASK:
    b = mkimage(rnd() % 10 ? w : w + 1, rnd() % 10 ? h : h + 1, db, 1);
    if (!a[5] && (da == 4 || da == 8 || da == 32)) { a[5] = (u32)(uintptr_t)halloc(a[4] * h + 4); }
    if (!b[5]) b[5] = (u32)(uintptr_t)halloc(b[4] * b[2] + 4);
    if ((db == 4 || db == 8) && !b[6]) b[6] = (u32)(uintptr_t)halloc(1024);
    if ((da == 4 || da == 8) && !a[6]) a[6] = (u32)(uintptr_t)halloc(1024);
    if (rnd() % 10 == 0) failAt = 1 + rnd() % 2;      /* the free-list image or its pixels; a failing RwImageAllocatePixels(image) is not tested (both sides write through NULL) */
    swapA = (u8 *)(uintptr_t)b[5]; swapB = swapA + 4;
    cov[1] = failAt != 0;
    if (neg && isdll) swapw();
    r = ((F2)fn)((u32)(uintptr_t)a, (u32)(uintptr_t)b);
    if (neg && isdll) swapw();
    break;
  default:      /* straight copy / convert / copy: dst a (pixels always), src b of the same size */
    if (!a[5]) a[5] = (u32)(uintptr_t)halloc(a[4] * h + 4);
    b = mkimage(w, h, kind == K_STRAIGHT ? da : db, 1);
    if (kind == K_STRAIGHT && rnd() % 4 == 0) { b[3] = da == 32 ? 8 : 32; }
    if (!b[5]) b[5] = (u32)(uintptr_t)halloc(b[4] * h + 4);
    if ((b[3] == 4 || b[3] == 8) && !b[6] && kind != K_STRAIGHT) b[6] = (u32)(uintptr_t)halloc(1024);
    swapA = (u8 *)(uintptr_t)b[5]; swapB = swapA + 4;
    cov[1] = a[3] != b[3];
    if (neg && isdll) swapw();
    r = ((F2)fn)((u32)(uintptr_t)a, (u32)(uintptr_t)b);
    if (neg && isdll) swapw();
    break;
  }
  return r;
}

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[8192]; int k; for (k = 0; k < 8192; k++) b[k] = 0; }
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  u32 *sp = (u32 *)(uintptr_t)e->ContextRecord->Esp; int k;
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx; stack", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  for (k = 0; k < 16; k++) printf(" %08x", sp[k]);
  printf("\n"); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ); lg[0] = malloc(4 * LGMAX); lg[1] = malloc(4 * LGMAX);
  if (!heap || !save0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  jmp_patch(0x4ea160, (void *)herr); jmp_patch(0x4e9ce0, (void *)hinit); jmp_patch(0x4e9d40, (void *)hdeinit);
  memset(eng0, 0xee, sizeof eng0);
  *(void **)(eng0 + 0x108) = (void *)hmalloc; *(void **)(eng0 + 0x10c) = (void *)hfree; *(void **)(eng0 + 0x118) = (void *)hflalloc; *(void **)(eng0 + 0x11c) = (void *)hflfree;
  W(0x7d2128) = (u32)(uintptr_t)eng; W(0x7d2724) = MODOFF;
  bind = (Bind)GetProcAddress(m, "mflrw_image_bind");
  if (!bind) { printf("mflrw_image_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x7d2724, (void *)0x62e848, (void *)0x4e9ce0, (void *)0x4e9d40, (void *)0x4dd610, (void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        ret[side] = run_case(kind, side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); memcpy(engs, eng, sizeof eng); }
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
