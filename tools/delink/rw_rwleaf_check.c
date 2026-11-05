/* rw_rwleaf_check.c: bit-exact check of the librw DLL's stateless leafs (tools/delink/rw_rwleaf.txt, librw-mfl capi/rwleaf.cpp:
   _rx_rxRadixExchangeSort 0x4f3280, _rwD3D9MeshGetNumVerticesMinIndex 0x4fa700, FUN_004fa760 (triangle list sort through the exe's CRT qsort
   0x5d38b0, comparator 0x4fa790), _rwD3D9ConvertToTriList 0x4fa820, D3D9MatrixInvertOrthoNormalized 0x50c110) against the ORIGINAL code of
   MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_camsync_check.c (same image mapping and harness arena).  The DLL's sort
   calls the exe's qsort (original code, mflrw_rwleaf_bind).  Each side rebuilds the case from the same seed; compared: the used arena incl.
   slack, the return value (not for the radix sort and the triangle sort: void, the original's eax is a leftover), the x87 control word
   (CW 0x7f on even and 0x27f on odd cases).  Inputs: radix sort 0..200 elements of 4..20 bytes, keys with duplicates, full / narrow key
   ranges, the invalid-parameter returns; tri-strips with degenerate triangles and minIndex 0 / random; matrices with inf / NaN / -0 and dst == src.
   usage: rw_rwleaf_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped; every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_RADIX, K_MINMAX, K_TRISORT, K_TOLIST, K_INVERT };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rx_rxRadixExchangeSort", 0x4f3280, K_RADIX}, {"rw37__rwD3D9MeshGetNumVerticesMinIndex", 0x4fa700, K_MINMAX}, {"rw37_D3D9SortTriList", 0x4fa760, K_TRISORT},
  {"rw37__rwD3D9ConvertToTriList", 0x4fa820, K_TOLIST}, {"rw37_D3D9MatrixInvertOrthoNormalized", 0x50c110, K_INVERT}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F2)(u32, u32);
typedef u32 (__cdecl *F4)(u32, u32, u32, u32);
typedef u32 (__cdecl *F6)(u32, u32, u32, u32, u32, u32);
typedef void (__cdecl *Bind)(void *);

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
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static float rf(void) {
  u32 m = rnd() % 16; float f;
  if (m == 0) return 0.0f;
  if (m == 1) { static const u32 sp[6] = {0x7f800000u, 0xff800000u, 0x7fc00000u, 0x80000000u, 0x00000001u, 0x7f7fffffu}; memcpy(&f, &sp[rnd() % 6], 4); return f; }
  if (m == 2) { u32 b = rnd(); memcpy(&f, &b, 4); return f; }
  return ((float)(int)(rnd() % 20001) - 10000.0f) * (m < 8 ? 0.0001f : 0.01f);
}
static void swap4(void *a, void *b) { u32 t; memcpy(&t, a, 4); memcpy(a, b, 4); memcpy(b, &t, 4); }
static void swap2(u16 *a, u16 *b) { u16 t = *a; *a = *b; *b = t; }

static u32 cwlog[2];
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, k; unsigned short cw = (seed & 1) ? 0x27f : 0x7f; int dneg = neg && isdll;
  rs = seed; cov[0] = cov[1] = 0;
  cw_set(cw);
  switch (kind) {
  case K_RADIX: {
    static const u32 sizes[6] = {4, 8, 12, 12, 16, 20};
    u32 n = rnd() % 4 == 0 ? rnd() % 8 : rnd() % 200, size = sizes[rnd() % 6], off = 4 * (rnd() % (size / 4)), mn = 0, mx = 0xffffffffu, nk;
    u8 *b;
    if (rnd() % 3 == 0) { mx = 1 + rnd() % (rnd() % 2 ? 1000 : 0x7fffffff); mn = rnd() % 4 ? 0 : rnd() % mx; }
    nk = rnd() % 4 == 0 ? 1 + rnd() % 8 : 0;      /* nk: number of distinct keys (duplicates) */
    b = halloc(n * size + 4);
    for (k = 0; k < n; k++) { u32 key = nk ? (rnd() % nk) * (mx / (nk + 1)) : (mx == 0xffffffffu ? rnd() : rnd() % (mx + 1)); memcpy(b + k * size + off, &key, 4); }
    if (rnd() % 32 == 0) { u32 t = mn; mn = mx; mx = t; }               /* minKey >= maxKey: nothing */
    if (rnd() % 64 == 0) off = size - 2;                                /* key beyond the element: nothing */
    if (dneg && n >= 2) swap4(b + off, b + (n - 1) * size + off);
    cov[0] = n > 5; cov[1] = nk != 0;
    ((F6)fn)(rnd() % 64 == 0 ? 0 : (u32)(uintptr_t)b, n, size, off, mn, mx);
    break; }
  case K_MINMAX: {
    u32 n = rnd() % 8 == 0 ? 0 : 1 + rnd() % 60; u16 *ix = halloc(2 * n + 2); u32 *o = halloc(8);
    for (k = 0; k < n; k++) ix[k] = (u16)(rnd() % 4 ? 100 + rnd() % 50 : rnd());
    cov[0] = n == 0;
    r = dneg ? ((F4)fn)((u32)(uintptr_t)ix, n, (u32)(uintptr_t)(o + 1), (u32)(uintptr_t)o) : ((F4)fn)((u32)(uintptr_t)ix, n, (u32)(uintptr_t)o, (u32)(uintptr_t)(o + 1));
    if (dneg) r = (u32)(uintptr_t)o;
    break; }
  case K_TRISORT: {
    u32 n = rnd() % 90; u16 *t = halloc(2 * n + 2);
    for (k = 0; k < n; k++) t[k] = (u16)(rnd() % 4 ? rnd() % 12 : rnd());
    if (dneg && n >= 3) { swap2(t, t + 1); if (t[0] == t[1]) t[2] ^= 1; }
    cov[0] = n % 3 != 0;
    ((F2)fn)((u32)(uintptr_t)t, n);
    break; }
  case K_TOLIST: {
    u32 n = 2 + rnd() % 60, mn = rnd() % 2 ? 0 : rnd() % 4 ? rnd() % 100 : rnd(); u16 *s = halloc(2 * n + 2), *d = halloc(6 * n + 6);
    for (k = 0; k < n; k++) s[k] = (u16)(rnd() % 3 ? mn + rnd() % 6 : rnd());
    if (dneg && n >= 3) { swap2(s, s + 2); if (s[0] == s[2]) s[1] ^= 1; }
    cov[0] = mn != 0;
    r = ((F4)fn)((u32)(uintptr_t)d, (u32)(uintptr_t)s, n, mn);
    break; }
  default: {
    float *src = halloc(64), *dst = rnd() % 4 ? halloc(64) : src;
    for (k = 0; k < 16; k++) src[k] = rf();
    if (dneg) swap4(src + 0, src + 12);
    cov[0] = dst == src;
    r = ((F2)fn)((u32)(uintptr_t)dst, (u32)(uintptr_t)src);
    break; }
  }
  cwlog[!isdll] = cw_now(); cw_set(0x27f);
  return r;
}

static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ);
  if (!heap || !save0) { printf("no memory\n"); return 2; }
  bind = (Bind)GetProcAddress(m, "mflrw_rwleaf_bind");
  if (!bind) { printf("mflrw_rwleaf_bind MISSING export\n"); return 1; }
  bind((void *)0x5d38b0);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-44s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        hseed = seed * 2654435761u; hp = 0;
        ret[side] = run_case(T[fi].kind, side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (ret[0] == ret[1] && hp0 == hp && cwlog[0] == cwlog[1] && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        int k; printf("  DIFF %s case %d: ret dll %08x orig %08x, cw %04x / %04x", T[fi].name, i, ret[0], ret[1], cwlog[0], cwlog[1]);
        for (k = 0; k + 4 <= (int)hp + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-44s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
