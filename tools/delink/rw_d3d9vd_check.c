/* rw_d3d9vd_check.c: bit-exact check of the librw DLL's D3D9 vertex-declaration converters (tools/delink/rw_d3d9vd.txt, librw-mfl
   capi/d3d9vd.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_bezpat_check.c (same image
   mapping: this exe is linked at 0x400000 and MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000, so the original
   functions run in place with their own size table and constants).  The converters touch no exe state.
   usage: rw_d3d9vd_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N finite cases + N/5 special cases (inf / NaN / random bits in the float inputs; reported, not counted).  x87 CW 0x7f (the
   game's), MXCSR 0x9fc0.  Compared per case: the whole arena after the call (the output with random bytes around and between the elements,
   the inputs), the return value, the control word.  Types 0..16 (the original indexes its 17-entry size table with the type).
   neg = negative fixture (negtest.sh): the DLL's InstV3d / InstV2d read a copy of the source (outside the arena) with components x and y
   swapped; the check must report DIFF for both. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_V3D, K_V2D, K_MORPH, K_WGT, K_COL, K_REMAP, K_UN3, K_UN2, NK };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rpD3D9VertexDeclarationInstV3d", 0x5063f0, K_V3D}, {"rw37__rpD3D9VertexDeclarationInstV2d", 0x507e00, K_V2D},
  {"rw37__rpD3D9VertexDeclarationInstV3dMorph", 0x507480, K_MORPH}, {"rw37__rpD3D9VertexDeclarationInstWeights", 0x505c40, K_WGT},
  {"rw37__rpD3D9VertexDeclarationInstColor", 0x508400, K_COL}, {"rw37__rpD3D9VertexDeclarationInstIndicesRemap", 0x508460, K_REMAP},
  {"rw37__rpD3D9VertexDeclarationUnInstV3d", 0x508cb0, K_UN3}, {"rw37__rpD3D9VertexDeclarationUnInstV2d", 0x5090a0, K_UN2}};
typedef u32 (__cdecl *FInst)(u32 type, void *mem, const void *src, int32_t n, u32 stride);
typedef u32 (__cdecl *FMorph)(u32 type, void *mem, const void *s1, const void *s2, float scale, int32_t n, u32 stride);
typedef int (__cdecl *FCol)(void *mem, const void *col, int32_t n, u32 stride);
typedef u32 (__cdecl *FRemap)(u32 type, void *mem, const void *idx, const void *remap, int32_t n, u32 stride);
typedef void (__cdecl *FUn)(u32 type, void *dst, const void *src);
static const u32 esz[17] = { 4, 8, 12, 16, 4, 4, 4, 8, 4, 4, 8, 4, 8, 4, 4, 4, 8 };

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float spf(float f) {      /* in special mode a few values become inf / NaN / random bits */
  u32 r; if (!special || rnd() % 6) return f;
  r = rnd() % 4; return bitsf(r == 0 ? 0x7f800000 : r == 1 ? 0xff800000 : r == 2 ? 0x7fc00000 : rnd());
}
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
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

/* ---- arena (both sides start from the same bytes; compared after the call) */
#define ASZ (1u << 18)
static u8 *arena, *snap, *after0, *negsrc;
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; return p; }
static void *allocr(u32 n) { u8 *p = alloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }

static float rval(int kind) {      /* a component: normals / uvs / weights in range, rounding ties of the scaled types, out of range, large */
  int k = rnd() % 16;
  static const float sc[4] = { 255.0f, 32767.0f, 65535.0f, 511.0f };
  if (k == 0) return 0;
  if (k == 1) return (float)((int)(rnd() % 1024) - 512);
  if (k == 2) return ((float)(rnd() % 600) + 0.5f) / sc[rnd() % 4] * (rnd() % 2 ? 1 : -1);      /* x * scale lands on a .5 tie (or close) */
  if (k == 3) return (float)((int)(rnd() % 2000) - 1000) + 0.5f;       /* ties of the unscaled types */
  if (k == 4) return uf(-1, 1) * (rnd() % 2 ? 1e5f : 3e9f);            /* fistp overflow -> 0x80000000 */
  if (k == 5) return rnd() % 2 ? 1.0f : -1.0f;
  if (kind == K_WGT) return k < 12 ? ud() : uf(0, 0.01f);
  return uf(-1.5f, 1.5f);
}

static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 16u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); after0 = malloc(ASZ); negsrc = malloc(ASZ); if (!arena || !snap || !after0 || !negsrc) { printf("no memory\n"); return 2; }
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: InstV3d / InstV2d read x <-> y swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, kind = T[fi].kind; long cov[4] = {0};
    if (only && !strstr(only, T[fi].name + 29)) continue;
    if (!dll) { printf("%-46s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u8 *mem = 0, *src = 0, *src2 = 0, *rmp = 0; u32 type, stride = 0, ret[2], ncomp = 0, sstep = 0; int32_t n = 0; float scale = 0;
      cur_fi = fi; cur_case = i;
      special = i >= N; an = 0;
      type = rnd() % 8 == 0 ? 15 + rnd() % 2 : rnd() % 15;
      /* ---- build the case (arena only) */
      if (kind == K_UN3 || kind == K_UN2) {
        mem = allocr(16 + 16); src = allocr(16 + 16);
        if (rnd() % 2) for (k = 0; k < 4; k++) { float f = spf(rval(kind)); memcpy(src + 4 * k, &f, 4); }
        cov[type <= 3 ? 0 : type <= 8 ? 1 : type <= 14 ? 2 : 3]++;
      } else {
        n = rnd() % 16 == 0 ? -(int32_t)(rnd() % 3) : (int32_t)(rnd() % 24);
        if (kind == K_REMAP) type = rnd() % 4 == 0 ? rnd() % 17 : 4 + rnd() % 4;
        if (kind == K_COL) type = 4;
        stride = esz[type] + 4 * (rnd() % 4); if (rnd() % 4 == 0) stride = esz[type];
        if (kind == K_V3D && type == 2 && stride == 12 && n < 0) n = 0;      /* block copy: n < 0 copies ~4 GB in the original */
        if (kind == K_V2D && type == 1 && stride == 8 && n < 0) n = 0;
        mem = allocr((n > 0 ? n : 1) * stride + 32);
        ncomp = kind == K_V2D ? 2 : kind == K_WGT ? 4 : 3; sstep = 4 * ncomp;
        if (kind == K_COL || kind == K_REMAP) {
          int allff = rnd() % 3 == 0;
          src = allocr((n > 0 ? n : 1) * 4 + 16);
          if (kind == K_COL && allff) for (k = 0; k < n; k++) src[4 * k + 3] = 0xff;
          if (kind == K_REMAP) rmp = allocr(256 + 16);
        } else {
          float *s = allocr((n > 0 ? n : 1) * sstep + 16), *s2 = kind == K_MORPH ? allocr((n > 0 ? n : 1) * sstep + 16) : 0;
          int how = rnd() % 4;
          for (k = 0; k < (n > 0 ? n : 0) * (int)ncomp; k++) {
            s[k] = spf(rval(kind));
            if (kind == K_WGT && how == 0 && k % 4 == 0 && rnd() % 2) s[k] = rnd() % 2 ? 1.0f : uf(1, 2);      /* first weight >= 1 */
            if (kind == K_WGT && how == 1 && k % 4 == 2) s[k] = rnd() % 2 ? 0.0019569471f : uf(0, 0.004f);     /* around 1/511 */
            if (s2) s2[k] = spf(rnd() % 4 ? s[k] + uf(-0.2f, 0.2f) : rval(kind));
          }
          src = (u8 *)s; src2 = (u8 *)s2;
          if (kind == K_MORPH) { int q = rnd() % 8; scale = spf(q == 0 ? 0 : q == 1 ? 1 : q == 2 ? uf(-1, 2) : ud()); }
        }
        cov[n <= 0 ? 0 : type <= 3 ? 1 : type <= 14 ? 2 : 3]++;
      }
      memcpy(snap, arena, an);
      for (side = 0; side < 2; side++) {      /* both sides run on the same arena bytes (restored before each) */
        void *fn = side ? orig : dll; u32 r = 0; unsigned short c; const u8 *s = src;
        memcpy(arena, snap, an);
        if (neg && !side && (kind == K_V3D || kind == K_V2D) && n > 0) {      /* the DLL reads x <-> y swapped from a copy outside the arena */
          memcpy(negsrc, src, n * sstep); for (k = 0; k < n; k++) { u32 t; memcpy(&t, negsrc + k * sstep, 4); memcpy(negsrc + k * sstep, negsrc + k * sstep + 4, 4); memcpy(negsrc + k * sstep + 4, &t, 4); }
          s = negsrc;
        }
        cw_set(0x7f);
        switch (kind) {
        case K_V3D: case K_V2D: case K_WGT: r = ((FInst)fn)(type, mem, s, n, stride); break;
        case K_MORPH: r = ((FMorph)fn)(type, mem, src, src2, scale, n, stride); break;
        case K_COL: r = (u32)((FCol)fn)(mem, src, n, stride); break;
        case K_REMAP: r = ((FRemap)fn)(type, mem, src, rmp, n, stride); break;
        default: ((FUn)fn)(type, mem, src); break;
        }
        c = cw_now(); cw_set(0x7f);
        ret[side] = r ^ ((u32)c << 16 & 0xffff0000u) ^ (c & 0xffff);
        if (!side) memcpy(after0, arena, an);
      }
      if (ret[0] == ret[1] && !memcmp(after0, arena, an)) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: type %u n %d stride %u ret dll %08x orig %08x", T[fi].name, i, type, n, stride, ret[0], ret[1]);
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-46s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld; %d of %d special cases differ\n", cov[0], cov[1], cov[2], cov[3], badsp, N / 5); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
