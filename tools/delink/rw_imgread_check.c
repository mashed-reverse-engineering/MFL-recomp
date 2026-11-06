/* rw_imgread_check.c: bit-exact check of the librw DLL's BMP reader (tools/delink/rw_imgread.txt, librw-mfl capi/rwimgread.cpp: RtBMPImageRead 0x53d050)
   against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_image_check.c (same image mapping and harness heap).
   Harness side of the exe: RwEngineInstance 0x7d2128 -> harness engine (RwMalloc +0x108 / RwFree +0x10c log and bump-allocate from one arena);
   RwStreamOpen 0x4dc370 / Read 0x4dbe70 / Skip 0x4dc190 / Close 0x4dc2a0 -> a logged memory "file"; RwImageCreate 0x4dd550 / Destroy 0x4dd5b0 /
   AllocatePixels 0x4dd610 and RwErrorSet 0x4ea160 -> loggers on arena images.  BMPImageSetSpan 0x53d6c0 runs as original code on both sides (the DLL
   reaches it through mflrw_imgread_bind).  Compared: return value, call log, the used arena incl. slack (images, pixels, palettes, row buffer).
   Inputs: core (12) / info (40) / other (108) headers, 1 / 4 / 8 / 16 / 24 / 32 bit, compression 0 / 1 (RLE8 with runs, absolute runs incl. pad,
   end of line / bitmap, delta, junk) / 2 / 3, biClrUsed 0 / in range / too big, gaps or overlaps before bfOffBits, truncated files, bad magic,
   1 in 20 failing open / create / allocate pixels / malloc.
   usage: rw_imgread_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side reads the file with two distinct bytes of its pixel data swapped; the entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
static const struct { const char *name; u32 va; } T[] = {{"rw37_RtBMPImageRead", 0x53d050}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(const char *);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);

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
#define HSZ (4u << 20)
#define LGMAX (1u << 16)
static u8 *heap, *save0; static u32 hp, nalloc, failAt, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static u8 eng[0x400], eng0[0x400], engs[0x400];
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > (1u << 20) || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static int failnow(void) { return ++nalloc == failAt; }
static void *__cdecl hmalloc(u32 n, u32 hint) { logw(0xa1100000u); logw(n); logw(hint); if (failnow() || n > (1u << 18)) { logw(0); return 0; } { void *p = halloc(n + 2048); logw((u32)(uintptr_t)p); return p; } }      /* +2048: room for the original's row overrun */
static void __cdecl hfree(void *p) { logw(0xa1200000u); logw((u32)(uintptr_t)p); }
static void *__cdecl herr(int32_t *e) { logw(0xa1700000u); logw(e[0]); logw(e[1]); return e; }
/* the memory file */
static u8 file[0x8000]; static u32 flen, fpos; static u32 stream_tok[4];
static void *__cdecl hopen(int32_t type, int32_t access, const char *name) { logw(0xa2100000u); logw(type); logw(access); logw((u32)(uintptr_t)name); fpos = 0; if (failnow()) { logw(0); return 0; } return stream_tok; }
static u32 __cdecl hread(void *s, void *buf, u32 n) { u32 k = n < flen - fpos ? n : flen - fpos; logw(0xa2200000u); logw((u32)(uintptr_t)s); logw(n); memcpy(buf, file + fpos, k); fpos += k; logw(k); return k; }
static void *__cdecl hskip(void *s, u32 n) { logw(0xa2300000u); logw((u32)(uintptr_t)s); logw(n); if (n > flen - fpos) { logw(0); return 0; } fpos += n; return s; }
static int32_t __cdecl hclose(void *s, void *d) { logw(0xa2400000u); logw((u32)(uintptr_t)s); logw((u32)(uintptr_t)d); return 1; }
/* RwImage {flags, width, height, depth, stride, pixels, palette} */
static u32 *__cdecl hcreate(int32_t w, int32_t h, int32_t depth) { u32 *im; logw(0xa3100000u); logw(w); logw(h); logw(depth); if (failnow()) { logw(0); return 0; } im = halloc(28); im[0] = 0; im[1] = w; im[2] = h; im[3] = depth; im[4] = 0; im[5] = 0; im[6] = 0; logw((u32)(uintptr_t)im); return im; }
static int32_t __cdecl hdestroy(u32 *im) { logw(0xa3200000u); logw((u32)(uintptr_t)im); return 1; }
static u32 *__cdecl hpix(u32 *im) {
  int32_t w = im[1], h = im[2], d = im[3], stride = d == 32 ? 4 * w : w;
  logw(0xa3300000u); logw((u32)(uintptr_t)im); if (failnow()) { logw(0); return 0; }
  im[4] = stride; im[5] = (u32)(uintptr_t)halloc(stride * (h > 0 ? h : 0) + 8); if (d <= 8) im[6] = (u32)(uintptr_t)halloc(4u << d); im[0] |= 1; return im;
}

/* ---- the BMP generator */
static u8 *fp;
static void p8(u32 v) { *fp++ = (u8)v; }
static void p16(u32 v) { p8(v); p8(v >> 8); }
static void p32(u32 v) { p16(v); p16(v >> 16); }
static u32 pixlo;      /* file offset of the pixel data (negative fixture) */
static void mkbmp(void) {
  static const int bits[] = {1, 4, 8, 8, 16, 24, 32}; static const u32 hs[] = {12, 40, 40, 40, 40, 108};
  u32 biSize = hs[rnd() % 6], bpp = bits[rnd() % 7], comp = 0, ncol, used, k, y, off;
  int32_t w = 1 + rnd() % 48, h = rnd() % 16 == 0 ? -(int32_t)(rnd() % 3) : 1 + (int32_t)(rnd() % 24);
  if (biSize == 40) { u32 r = rnd() % 10; comp = r < 6 ? 0 : r < 9 ? 1 : 2 + rnd() % 2; if (comp == 1 && rnd() % 4) bpp = 8; }
  if (biSize == 12 && h < 0) h = 1;
  ncol = biSize == 40 && rnd() % 2 ? (rnd() % 4 ? rnd() % ((1u << (bpp & 31)) + 1) : 300) : 0;
  fp = file; p16(0x4d42); if (rnd() % 40 == 0) file[rnd() % 2] ^= 0x20;
  p32(0); p32(0); p32(0); p32(biSize);      /* size, reserved, bfOffBits (patched below), biSize */
  if (biSize == 12) { p16(w); p16(h); p16(1); p16(bpp); }
  else { p32(w); p32(h); p16(1); p16(bpp); p32(comp); p32(0); p32(2835); p32(2835); p32(ncol); p32(0); for (k = 40; k < biSize; k++) p8(rnd()); }
  if (bpp <= 8) { u32 n = ncol > 0 && ncol <= (1u << bpp) ? ncol : 1u << bpp; for (k = 0; k < n * (biSize == 12 ? 3 : 4); k++) p8(rnd()); }
  used = (u32)(fp - file);
  if (rnd() % 8 == 0) { for (k = rnd() % 9; k; k--) p8(rnd()); }
  pixlo = (u32)(fp - file); off = pixlo; if (rnd() % 16 == 0) off -= 1 + rnd() % 4;      /* bfOffBits before the end of the palette: the skip fails */
  file[10] = (u8)off; file[11] = (u8)(off >> 8); file[12] = file[13] = 0;
  if (comp == 1) {
    for (y = 0; y < (u32)(h > 0 ? h : 0); y++) {
      int32_t x = 0;
      while (x < w) {
        u32 r = rnd() % 10, n = 1 + rnd() % (w - x);
        if (r < 6) { p8(n); p8(rnd()); }
        else if (n >= 3) { p8(0); p8(n); for (k = 0; k < n; k++) p8(rnd()); if (n & 1) p8(rnd()); }
        else { p8(n); p8(rnd()); }
        x += n;
      }
      { u32 r = rnd() % 40; if (r == 0) { p8(0); p8(2); p8(1); p8(1); } else if (r == 1) { p8(0); p8(1); break; } else if (r == 2) p8(rnd()); else { p8(0); p8(0); } }
    }
    if (rnd() % 2) { p8(0); p8(1); }
  } else {
    u32 row = ((bpp * (u32)w + 7) / 8 + 3) & ~3u;
    for (k = 0; k < row * (u32)(h > 0 ? h : 0); k++) p8(rnd());
  }
  flen = (u32)(fp - file);
  if (rnd() % 8 == 0) flen = rnd() % (flen + 1);
  (void)used;
}

static u8 *swapA, *swapB;
static void swapb(void) { u8 t; if (!swapA) return; t = *swapA; *swapA = *swapB; *swapB = t; }
static char fname[] = "test.bmp";

static u32 run_case(void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r, k;
  rs = seed; hseed = seed * 2654435761u; hp = 0; nalloc = 0; failAt = 0; ln[side] = 0; swapA = swapB = 0;
  memcpy(eng, eng0, sizeof eng);
  mkbmp();
  if (rnd() % 5 == 0) failAt = 1 + rnd() % 4;      /* open, create, allocate pixels, row buffer */
  if (neg) for (k = pixlo; k + 1 < flen; k++) if (file[k] != file[k + 1]) { swapA = file + k; swapB = file + k + 1; break; }
  if (neg && isdll) swapb();
  r = ((F1)fn)(fname);
  if (neg && isdll) swapb();
  cov[0] = r != 0; cov[1] = flen > 54 && file[30] == 1;
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
  jmp_patch(0x4ea160, (void *)herr); jmp_patch(0x4dc370, (void *)hopen); jmp_patch(0x4dbe70, (void *)hread); jmp_patch(0x4dc190, (void *)hskip);
  jmp_patch(0x4dc2a0, (void *)hclose); jmp_patch(0x4dd550, (void *)hcreate); jmp_patch(0x4dd5b0, (void *)hdestroy); jmp_patch(0x4dd610, (void *)hpix);
  memset(eng0, 0xee, sizeof eng0);
  *(void **)(eng0 + 0x108) = (void *)hmalloc; *(void **)(eng0 + 0x10c) = (void *)hfree;
  W(0x7d2128) = (u32)(uintptr_t)eng;
  bind = (Bind)GetProcAddress(m, "mflrw_imgread_bind");
  if (!bind) { printf("mflrw_imgread_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x4dc370, (void *)0x4dbe70, (void *)0x4dc190, (void *)0x4dc2a0, (void *)0x4dd550, (void *)0x4dd5b0, (void *)0x4dd610, (void *)0x53d6c0, (void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL reads two pixel bytes swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0; long cov[2] = {0}, nonnull = 0; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = argc > 6 ? atoi(argv[6]) : 0; i < N; i++) {      /* argv[6]: first case (debug), its header is printed */
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      if (argc > 6 && i == atoi(argv[6])) { rs = seed; mkbmp(); printf("case %d: flen %u off %u hdr", i, flen, pixlo); for (k = 0; k < 64 && k < (int)flen; k++) printf(" %02x", file[k]); printf("\n"); }
      for (side = 0; side < 2; side++) {
        stackwipe();
        ret[side] = run_case(side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); memcpy(engs, eng, sizeof eng); }
      }
      cov[0] += c[0]; cov[1] += c[1]; nonnull += ret[1] != 0;
      if (ret[0] == ret[1] && hp0 == hp && ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * (ln[0] < LGMAX ? ln[0] : LGMAX)) && !memcmp(save0, heap, hp + 16) && !memcmp(engs, eng, sizeof eng)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap %u / %u, log %u / %u", T[fi].name, i, ret[0], ret[1], hp0, hp, ln[0], ln[1]);
        for (k = 0; k < (int)(ln[0] < ln[1] ? ln[0] : ln[1]) && k < (int)LGMAX; k++) if (lg[0][k] != lg[1][k]) { printf(", first log diff at %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k + 4 <= (int)(hp0 < hp ? hp0 : hp) + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first heap diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage: decoded %ld, RLE8 files %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
