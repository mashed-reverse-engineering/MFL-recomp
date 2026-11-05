/* rw_sortcmp_check.c: bit-exact check of the two qsort comparators the librw DLL exports for LIB_RW group sortcmp (tools/delink/rw_sortcmp.txt:
   TriStripSortCompare 0x502650 = librw-mfl capi/rwtristrip.cpp cmpTris, CollTreeValueCompare 0x54ecf0 = capi/rwcolltree.cpp cmpValue) against
   the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_camsync_check.c (same image mapping and harness arena).
   Inputs: two element pointers into the arena; tri-strip: build triangles {+8 material, +0xc..+0x12 four u16 indices} with materials from a
   small pool (equal / NULL / no texture, raster format byte +0x23 of the texture's raster, colour alpha +7); colltree: vertex infos {+8 float}
   with random floats incl. inf / NaN / -0 / denormals.  Compared: the return value, the arena incl. slack, the x87 control word; CW 0x7f on
   even and 0x27f on odd cases.
   usage: rw_sortcmp_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets its two arguments swapped; every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_TRI, K_COLL };
static const struct { const char *name; u32 va; int kind; } T[] = {{"rw37_TriStripSortCompare", 0x502650, K_TRI}, {"rw37_CollTreeValueCompare", 0x54ecf0, K_COLL}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F2)(u32, u32);

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
  return ((float)(int)(rnd() % 2001) - 1000.0f) * (m < 8 ? 0.001f : 1.0f);
}

static u32 cwlog[2];
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r, *pa = halloc(4), *pb = halloc(4), k; unsigned short cw = (seed & 1) ? 0x27f : 0x7f;
  rs = seed;
  if (kind == K_TRI) {
    u32 *mats[4], *e[2];
    for (k = 0; k < 4; k++) {
      u8 *mat = halloc(0x20), *tex = halloc(0x10), *ras = halloc(0x30);
      *(u32 *)tex = (u32)(uintptr_t)ras; ras[0x23] = (u8)(rnd() % 4 ? rnd() % 8 : rnd());
      *(u32 *)mat = rnd() % 4 ? (u32)(uintptr_t)tex : 0; mat[7] = rnd() % 2 ? 0xff : (u8)rnd();
      mats[k] = rnd() % 8 ? (u32 *)mat : 0;
    }
    for (k = 0; k < 2; k++) {
      u16 *t; e[k] = halloc(0x14); t = (u16 *)(e[k] + 3);
      e[k][2] = (u32)(uintptr_t)mats[rnd() % 4];
      t[0] = (u16)(rnd() % 3); t[1] = (u16)(rnd() % 3); t[2] = (u16)(rnd() % 3); t[3] = (u16)(rnd() % 3);
      if (rnd() % 8 == 0) t[rnd() % 4] = (u16)rnd();
    }
    *pa = (u32)(uintptr_t)e[0]; *pb = (u32)(uintptr_t)e[1];
    cov[0] = e[0][2] == e[1][2]; cov[1] = !e[0][2] || !e[1][2];
  } else {
    u32 *v[2]; float f;
    for (k = 0; k < 2; k++) { v[k] = halloc(12); f = rf(); memcpy(v[k] + 2, &f, 4); }
    if (rnd() % 8 == 0) v[1][2] = v[0][2];
    *pa = (u32)(uintptr_t)v[0]; *pb = (u32)(uintptr_t)v[1];
    cov[0] = v[0][2] == v[1][2]; cov[1] = 0;
  }
  cw_set(cw);
  r = neg && isdll ? ((F2)fn)((u32)(uintptr_t)pb, (u32)(uintptr_t)pa) : ((F2)fn)((u32)(uintptr_t)pa, (u32)(uintptr_t)pb);
  cwlog[!isdll] = cw_now(); cw_set(0x27f);
  return r;
}

static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ);
  if (!heap || !save0) { printf("no memory\n"); return 2; }
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets its two arguments swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
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
      if (++bad <= 3) printf("  DIFF %s case %d: ret dll %08x orig %08x, cw %04x / %04x\n", T[fi].name, i, ret[0], ret[1], cwlog[0], cwlog[1]);
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
