/* rw_splopen_check.c: bit-exact check of the librw DLL's spline plugin open / close callbacks (tools/delink/rw_splopen.txt, librw-mfl
   capi/rwsplopen.cpp: SplineOpen 0x556bf0, SplineClose 0x556b90, FUN_00556ec0 = SplineBasisMatrixCreate) against the ORIGINAL code of MFL.exe,
   both run natively (Wine) on the same CPU.  Made from rw_rwleaf2_check.c (same image mapping and harness arena).  The engine (RwEngineInstance
   0x7d2128) is a harness block whose malloc +0x108 / free +0x10c bump-allocate from the arena and log every call (1 in 4 cases fails the k-th
   allocation); RwErrorSet 0x4ea160 is jmp-patched to a logger.  The DLL is bound (mflrw_splopen_bind) to the same engine pointer, the open
   counter 0x7da7b8, rapGTable 0x7da7bc and 0x4ea160.  Each side rebuilds the case from the same seed (open counter 0 / 1..3 / -1, random slot
   words); compared: the used arena incl. slack, the call log, the global block 0x7da7b8..0x7da818, the return value and the x87 control word
   (CW 0x7f on even and 0x27f on odd cases).
   usage: rw_splopen_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets a changed input (instance pointer, matrix order); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
enum { K_OPEN, K_CLOSE, K_BASIS };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_SplineOpen", 0x556bf0, K_OPEN}, {"rw37_SplineClose", 0x556b90, K_CLOSE}, {"rw37_SplineBasisMatrixCreate", 0x556ec0, K_BASIS}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef u32 (__cdecl *F3)(u32, u32, u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *);

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
static void cw_set(unsigned short cw) { __asm__ volatile("fldcw %0" : : "m"(cw)); }
static unsigned short cw_now(void) { unsigned short cw; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw; }
#define W32(va) (*(u32 *)(uintptr_t)(va))
#define GLO 0x7da7b8u
#define GSZ 0x60u

#define HSZ (1u << 20)
#define LGMAX 4096
static u8 *heap, *save0; static u32 hp, hseed, nalloc, failAt;
static u32 lg[2][LGMAX], ln[2], gsave[2][GSZ / 4]; static int side;
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static void *__cdecl hmalloc(u32 n, u32 hint) { logw(0xa1100000u); logw(n); logw(hint); if (++nalloc == failAt || n > (1u << 18)) { logw(0); return 0; } { void *p = halloc(n); logw((u32)(uintptr_t)p); return p; } }
static void __cdecl hfree(void *p) { logw(0xa1200000u); logw((u32)(uintptr_t)p); }
static void *__cdecl herr(int32_t *e) { logw(0xa1700000u); logw(e[0]); logw(e[1]); return e; }
static u32 eng[0x50];

static u32 cwlog[2];
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, k; unsigned short cw = (seed & 1) ? 0x27f : 0x7f; int dneg = neg && isdll;
  rs = seed; cov[0] = cov[1] = 0; nalloc = 0; ln[side] = 0;
  failAt = rnd() % 4 == 0 ? 1 + rnd() % 12 : 0;
  for (k = 0; k < GSZ / 4; k++) W32(GLO + 4 * k) = rnd() % 4 ? (u32)(uintptr_t)halloc(4) : rnd();
  cw_set(cw);
  switch (kind) {
  case K_OPEN: {
    u32 inst = rnd(); int32_t cnt = rnd() % 4 == 0 ? (int32_t)(1 + rnd() % 3) : 0;
    W32(GLO) = (u32)cnt;
    if (dneg) inst ^= 4;
    cov[0] = cnt == 0; cov[1] = failAt != 0;
    r = ((F3)fn)(inst, rnd(), rnd());
    break; }
  case K_CLOSE: {
    u32 inst = rnd(); int32_t cnt = rnd() % 3 ? 1 : rnd() % 2 ? (int32_t)(2 + rnd() % 3) : 0;
    W32(GLO) = (u32)cnt;
    if (dneg) inst ^= 4;
    cov[0] = cnt == 1;
    r = ((F3)fn)(inst, rnd(), rnd());
    break; }
  default: {
    int32_t n = 1 + rnd() % 12;
    if (dneg) n = n % 12 + 1;
    cov[0] = n >= 4 && n <= 10; cov[1] = failAt != 0;
    r = ((F1)fn)((u32)n);
    break; }
  }
  cwlog[!isdll] = cw_now(); cw_set(0x27f);
  for (k = 0; k < GSZ / 4; k++) gsave[!isdll][k] = W32(GLO + 4 * k);
  return r;
}

static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 40u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ);
  if (!heap || !save0) { printf("no memory\n"); return 2; }
  eng[0x108 / 4] = (u32)(uintptr_t)hmalloc; eng[0x10c / 4] = (u32)(uintptr_t)hfree; W32(0x7d2128) = (u32)(uintptr_t)eng;
  jmp_patch(0x4ea160, (void *)herr);
  bind = (Bind)GetProcAddress(m, "mflrw_splopen_bind");
  if (!bind) { printf("mflrw_splopen_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)GLO, (void *)(GLO + 4), (void *)0x4ea160);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets a changed input" : "");
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
      if (ret[0] == ret[1] && hp0 == hp && cwlog[0] == cwlog[1] && ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * (ln[0] < LGMAX ? ln[0] : LGMAX))
          && !memcmp(gsave[0], gsave[1], GSZ) && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        int k; printf("  DIFF %s case %d: ret dll %08x orig %08x, cw %04x / %04x, log %u / %u words", T[fi].name, i, ret[0], ret[1], cwlog[0], cwlog[1], ln[0], ln[1]);
        for (k = 0; k < (int)(GSZ / 4); k++) if (gsave[0][k] != gsave[1][k]) { printf(", global +0x%x: dll %08x orig %08x", 4 * k, gsave[0][k], gsave[1][k]); break; }
        for (k = 0; k + 4 <= (int)hp + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first arena diff at +0x%x (ret %+d): dll %08x orig %08x", k, (int)((u32)(uintptr_t)(heap + k) - ret[1]), *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-44s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
