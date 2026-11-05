/* rw_resample_check.c: bit-exact check of the librw DLL's resample kernels (tools/delink/rw_resample.txt, librw-mfl capi/rwresample.cpp:
   RwImageResample, ImageResampleGetSpan) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU, x87 control word 0x7f
   (the game's) on even cases and 0x27f on odd ones.  Made from rw_image_check.c (same image mapping, images in one arena that is rebuilt
   from the case seed for each side).  Compared: the whole arena (destination pixels, the span output), the return value, the control word.
   Inputs: 32 bit source images 1..64 x 1..64 (random pixels, 1 in 4 with a few distinct values, strides with padding), destinations
   1..64 x 1..64 (down and up sampling, same size); GetSpan with 16.16 spans inside one source row, single and multi pixel.
   usage: rw_resample_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two source pixels swapped (restored after the call); both entries must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
enum { K_RESAMPLE, K_SPAN };
static const struct { const char *name; u32 va; int kind; } T[] = {{"rw37_RwImageResample", 0x4eacf0, K_RESAMPLE}, {"rw37_ImageResampleGetSpan", 0x4eb110, K_SPAN}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F2)(u32, u32);
typedef void (__cdecl *F5)(u32, u32, u32, u32, u32);

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
static void cw_set(unsigned short cw) { __asm__ volatile("fldcw %0" : : "m"(cw)); }
static unsigned short cw_now(void) { unsigned short cw; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw; }

#define HSZ (1u << 20)
static u8 *heap, *save0; static u32 hp, hseed;
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) { u8 *p; u32 sz = (n + 16 + 3) & ~3u; if (hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); } p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p; }
/* RwImage {flags, width, height, depth 32, stride, pixels, palette} */
static u32 *mkimage(int w, int h, int fill) {
  u32 *im = halloc(28); int stride = 4 * w + (rnd() % 3 ? 0 : 4 * (rnd() % 4)), y, x, few = rnd() % 4 == 0; u8 *p = halloc(stride * h + 4);
  im[0] = rnd() & 0xf3; im[1] = w; im[2] = h; im[3] = 32; im[4] = stride; im[5] = (u32)(uintptr_t)p; im[6] = 0;
  if (fill) for (y = 0; y < h; y++) for (x = 0; x < 4 * w; x++) p[y * stride + x] = few ? (u8)(rnd() % 3 * 127) : (u8)rnd();
  return im;
}
static u8 *swapA, *swapB;
static void swapw(void) { u32 t; if (!swapA) return; memcpy(&t, swapA, 4); memcpy(swapA, swapB, 4); memcpy(swapB, &t, 4); }

static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, *src, *dst; int sw, sh;
  rs = seed; hseed = seed * 2654435761u; hp = 0; swapA = 0;
  sw = 1 + rnd() % 64; sh = 1 + rnd() % 64;
  if (rnd() % 8 == 0) { sw = 1 + rnd() % 4; sh = 1 + rnd() % 4; }
  src = mkimage(sw, sh, 1);
  swapA = (u8 *)(uintptr_t)src[5]; swapB = swapA + (sw > 1 ? 4 : src[4] * (sh > 1));
  if (swapB == swapA) swapA = 0;
  if (kind == K_RESAMPLE) {
    int dw = rnd() % 3 == 0 ? sw : 1 + rnd() % 64, dh = rnd() % 3 == 0 ? sh : 1 + rnd() % 64;
    dst = mkimage(dw, dh, 0);
    cov[0] = dw < sw; cov[1] = dh < sh;
    if (neg && isdll) swapw();
    r = ((F2)fn)((u32)(uintptr_t)dst, (u32)(uintptr_t)src);
    if (neg && isdll) swapw();
  } else {
    float *out = halloc(16); u32 x0 = rnd() % (sw << 16), x1, y = rnd() % (sh << 16);
    x1 = rnd() % 2 ? x0 + rnd() % 0x10000 : x0 + rnd() % ((sw << 16) - x0);
    if (x1 >= (u32)(sw << 16)) x1 = (sw << 16) - 1;
    if (x1 == x0 && rnd() % 2) x1 = x0 + 1 < (u32)(sw << 16) ? x0 + 1 : x0;
    cov[0] = (x0 >> 16) != (x1 >> 16); cov[1] = x1 == x0;
    if (neg && isdll) swapw();
    ((F5)fn)((u32)(uintptr_t)src, x0, x1, y, (u32)(uintptr_t)out);
    if (neg && isdll) swapw();
  }
  return r ^ (u32)cw_now() << 16;
}

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[4096]; int k; for (k = 0; k < 4096; k++) b[k] = 0; }
static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ);
  if (!heap || !save0) { printf("no memory\n"); return 2; }
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two source pixels swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        cw_set(i & 1 ? 0x27f : 0x7f);
        ret[side] = run_case(kind, side ? orig : dll, !side, neg, seed, c);
        cw_set(0x27f);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (ret[0] == ret[1] && hp0 == hp && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap %u / %u", T[fi].name, i, ret[0], ret[1], hp0, hp);
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
