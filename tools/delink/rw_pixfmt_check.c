/* rw_pixfmt_check.c: bit-exact check of the librw DLL's pixfmt kernels (tools/delink/rw_pixfmt.txt, librw-mfl capi/pixfmt.cpp: _rwD3D9RGBToPixel,
   _rwD3D9PixelToRGB, D3D9ConvR8G8B8A8To{A1R5G5B5,A4R4G4B4,R4G4B4,L8,A8L8,L16}) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the
   same CPU.  Made from rw_rwmath_check.c (same image mapping: MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000).
   RwErrorSet 0x4ea160 is patched to a logger (the error struct words); both sides reach it (the DLL through mflrw_pixfmt_bind).
   usage: rw_pixfmt_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N cases (random RGBA bytes / pixel words, every format value 0x000..0xf00 plus random high bits and the PAL8 bit).  Compared
   per case: the arena (output with random bytes around it, the input), the return value, the error log.
   neg = negative fixture (negtest.sh): the DLL side gets bytes 0 <-> 2 of its input swapped (restored after the call); the check must report
   DIFF for every entry. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
enum { K_R2P, K_P2R, K_CONV };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rwD3D9RGBToPixel", 0x4ede80, K_R2P}, {"rw37__rwD3D9PixelToRGB", 0x4ee0d0, K_P2R},
  {"rw37_D3D9ConvR8G8B8A8ToA1R5G5B5", 0x4ef9f0, K_CONV}, {"rw37_D3D9ConvR8G8B8A8ToA4R4G4B4", 0x4efa30, K_CONV}, {"rw37_D3D9ConvR8G8B8A8ToR4G4B4", 0x4efaa0, K_CONV},
  {"rw37_D3D9ConvR8G8B8A8ToL8", 0x4efbd0, K_CONV}, {"rw37_D3D9ConvR8G8B8A8ToA8L8", 0x4efc30, K_CONV}, {"rw37_D3D9ConvR8G8B8A8ToL16", 0x4efc80, K_CONV}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(void *);
typedef u32 (__cdecl *F3)(void *, void *, u32);
typedef void (__cdecl *Bind)(void *errorSet);

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
static u8 arena[64], snap[64], after0[64];
static u32 lg[2][4]; static int ln[2], side;
static void *__cdecl herr(int32_t *e) { if (ln[side] < 2) { lg[side][2 * ln[side]] = e[0]; lg[side][2 * ln[side] + 1] = e[1]; } ln[side]++; return e; }
static volatile int cur_fi, cur_case;
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  jmp_patch(0x4ea160, (void *)herr);
  bind = (Bind)GetProcAddress(m, "mflrw_pixfmt_bind");
  if (!bind) { printf("mflrw_pixfmt_bind MISSING export\n"); return 1; }
  bind((void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets input bytes 0 <-> 2 swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[3] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N; i++) {
      u8 *out = arena + 8, *in = arena + 24; u32 fmt = (rnd() % 16) << 8, ret[2];
      cur_fi = fi; cur_case = i;
      for (k = 0; k < 64; k++) arena[k] = (u8)rnd();
      if (rnd() % 4 == 0) fmt |= rnd() & ~0x2f00u;
      if (kind == K_P2R && rnd() % 8 == 0) fmt |= 0x2000;
      if (rnd() % 8 == 0) for (k = 0; k < 4; k++) in[k] = rnd() % 2 ? 0xff : 0;
      memcpy(snap, arena, 64);
      for (side = 0; side < 2; side++) {
        void *fn = side ? orig : dll; u32 r;
        memcpy(arena, snap, 64); ln[side] = 0; memset(lg[side], 0, sizeof lg[side]);
        if (neg && !side) { u8 t = in[0]; in[0] = in[2]; in[2] = t; }
        r = kind == K_CONV ? ((F1)fn)(in) : ((F3)fn)(out, in, fmt);
        if (neg && !side) { u8 t = in[0]; in[0] = in[2]; in[2] = t; }
        ret[side] = r;
        if (!side) memcpy(after0, arena, 64);
      }
      if (kind != K_CONV) cov[ln[1] ? 2 : ret[1] ? 1 : 0]++; else cov[ret[1] != 0]++;
      if (ret[0] == ret[1] && ln[0] == ln[1] && !memcmp(lg[0], lg[1], sizeof lg[0]) && !memcmp(after0, arena, 64)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: format %x ret dll %08x orig %08x, errors %d / %d", T[fi].name, i, fmt, ret[0], ret[1], ln[0], ln[1]);
        for (k = 0; k < 64; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL");      /* "finite" only to share the verdict format of the other rw_*_check (all integer) */ if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld\n", cov[0], cov[1], cov[2]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
