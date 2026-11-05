/* rw_bezpat_check.c: bit-exact check of the librw DLL's RtBezPat / RpPatch refine kernels (tools/delink/rw_bezpat.txt, librw-mfl capi/bezpat.cpp)
   against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_splanim_check.c (same image mapping: this exe
   is linked at 0x400000 and MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000, so the original functions run in
   place; the original refiners call the original Bezier kernels, the DLL's call the DLL's).  The kernels touch no exe state.
   usage: rw_bezpat_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N finite cases + N/5 special cases (inf / NaN / random bits in the inputs; reported, not counted).  x87 CW 0x7f (the
   game's), MXCSR 0x9fc0.  Compared per case: the whole arena after the call (the output with random bytes around and between the
   vertices, the inputs), the return value, the control word.
   neg = negative fixture (negtest.sh): the DLL's RtBezierQuadOriginDifference3d / 4d get stepU and stepV swapped; the check must report
   DIFF for both. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_BW3, K_BW4, K_OD3, K_OD4, K_VEC, K_COL, K_TEX, K_IDX, NK };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_RtBezierQuadBernsteinWeight3d", 0x55db00, K_BW3}, {"rw37_RtBezierQuadBernsteinWeight4d", 0x55e200, K_BW4},
  {"rw37_RtBezierQuadOriginDifference3d", 0x55eb50, K_OD3}, {"rw37_RtBezierQuadOriginDifference4d", 0x55f1c0, K_OD4},
  {"rw37__rpPatchRefineQuadVector", 0x55cb20, K_VEC}, {"rw37__rpPatchRefineQuadColor", 0x55cea0, K_COL},
  {"rw37__rpPatchRefineQuadTexCoords", 0x55d4e0, K_TEX}, {"rw37__rpPatchGenerateQuadIndex", 0x55d6c0, K_IDX}};
typedef void (__cdecl *FBw)(void *w, const void *b);
typedef void (__cdecl *FOd)(void *d, const void *w, float su, float sv);
typedef void *(__cdecl *FRef)(void *out, int32_t stride, const void *ctrl, int32_t n);
typedef void *(__cdecl *FIdx)(void *out, int32_t base, int32_t n);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float spf(float f) {      /* in special mode a few values become inf / NaN / random bits */
  u32 r; if (!special || rnd() % 12) return f;
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
#define ASZ (1u << 20)
static u8 *arena, *snap, *after0;
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; return p; }
static void *allocr(u32 n) { u8 *p = alloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }

static float scale;
static float rval(void) {      /* a control value: mostly smooth data, sometimes exact small integers / zero / large */
  int k = rnd() % 16;
  if (k == 0) return 0;
  if (k == 1) return (float)((int)(rnd() % 512) - 256);
  if (k == 2) return uf(-1, 1) * 1e30f;
  if (k == 3) return uf(-1, 1) * 3e38f;      /* sums leave the float range: values the original keeps on the x87 stack differ from stored ones */
  return uf(-1, 1) * scale;
}
static void mkmat(float *m, int color) {      /* 4 x 4 RtBezierV4d; colours mostly in 0..255 (with overshoot) */
  int i, how = rnd() % 5;
  for (i = 0; i < 64; i++) {
    if (color && how) m[i] = how == 1 ? (float)(rnd() % 256) : uf(-40, 300);
    else if (how == 0 && i >= 4) m[i] = m[i - 4] + ((i & 3) == 0 ? scale / 4 : 0);      /* planar ramp */
    else m[i] = rval();
    m[i] = spf(m[i]);
  }
}
static float rstep(void) {
  int k = rnd() % 10;
  if (k < 5) return 1.0f / (float)(1 + rnd() % 32);
  if (k == 5) return rnd() % 2 ? 0 : 1; if (k == 6) return (float)ldexp(uf(1, 2), -(int)(rnd() % 48));      /* down to 2^-47: step^4..6 in the float denormal range */
  if (k == 7) return uf(-2, 2); if (k == 8) return uf(0, 1e-6f);
  return rnd() % 2 ? uf(1, 1000) : uf(1e6f, 1e8f);      /* step^6 leaves the float range */
}

/* watchdog and crash report: a hang or a fault ends the run with a message instead of a stuck container */
static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 16u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); after0 = malloc(ASZ); if (!arena || !snap || !after0) { printf("no memory\n"); return 2; }
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: OriginDifference3d / 4d get stepU <-> stepV" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, kind = T[fi].kind; long cov[5] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u8 *a0 = 0, *a1 = 0; float su = 0, sv = 0; int32_t n = 0, stride = 0; u32 ret[2];
      cur_fi = fi; cur_case = i;
      special = i >= N; an = 0;
      scale = (float)ldexp(1.0, (int)(rnd() % 24) - 8);
      /* ---- build the case (arena only) */
      if (kind <= K_OD4) {
        float *in = allocr(256 + 32), *out = allocr(256 + 32);
        if (kind <= K_BW4) mkmat(in, 0); else { for (k = 0; k < 64; k++) in[k] = spf(rval()); }
        a0 = (u8 *)out; a1 = (u8 *)in;
        su = spf(rstep()); sv = rnd() % 3 ? su : spf(rstep());
        cov[su == sv ? 0 : 1]++;
      } else if (kind == K_IDX) {
        int k2 = rnd() % 10; n = k2 == 0 ? -(int32_t)(rnd() % 3) : k2 == 1 ? 0 : 1 + rnd() % 24;
        a0 = allocr(4 * (n > 0 ? n : 1) * (n + 2) + 64);
        stride = k2 == 2 ? (int32_t)rnd() : rnd() % 2 ? (int32_t)(rnd() % 65536) : (int32_t)(rnd() % 1000);      /* base (truncated to 16 bits) */
        cov[n <= 0 ? 0 : 1]++;
      } else {
        int minst = kind == K_VEC ? 12 : kind == K_COL ? 4 : 8, nv;
        n = rnd() % 12 == 0 ? (kind == K_VEC ? 0 : -(int32_t)(rnd() % 2)) : (int32_t)(rnd() % 17);
        if (kind != K_VEC && n == 0 && rnd() % 2) n = -1;
        stride = minst + 4 * (rnd() % 4); if (rnd() % 8 == 0) stride = minst;
        nv = n >= 0 ? (n + 1) * (n + 1) : 1;
        a0 = allocr(nv * stride + 64);
        if (kind == K_TEX) { float *c = allocr(32 + 16); for (k = 0; k < 8; k++) c[k] = spf(rnd() % 6 ? uf(-1, 2) : rval()); if (rnd() % 8 == 0) for (k = 0; k < 8; k++) c[k] = (float)(k & 1); a1 = (u8 *)c; }
        else { float *c = allocr(256 + 32); mkmat(c, kind == K_COL); a1 = (u8 *)c; }
        cov[n < 0 ? 0 : n == 0 ? 1 : n == 1 ? 2 : 3]++;
      }
      memcpy(snap, arena, an);
      for (side = 0; side < 2; side++) {      /* both sides run on the same arena bytes (restored before each) */
        void *fn = side ? orig : dll; u32 r = 0; unsigned short c;
        memcpy(arena, snap, an);
        cw_set(0x7f);
        switch (kind) {
        case K_BW3: case K_BW4: ((FBw)fn)(a0, a1); break;
        case K_OD3: case K_OD4: if (neg && !side) ((FOd)fn)(a0, a1, sv, su); else ((FOd)fn)(a0, a1, su, sv); break;
        case K_IDX: r = (u32)(uintptr_t)((FIdx)fn)(a0, stride, n); break;
        default: r = (u32)(uintptr_t)((FRef)fn)(a0, stride, a1, n); break;
        }
        c = cw_now(); cw_set(0x7f);
        ret[side] = r ^ ((u32)c << 16 & 0xffff0000u) ^ (c & 0xffff);
        if (!side) memcpy(after0, arena, an);
      }
      if (ret[0] == ret[1] && !memcmp(after0, arena, an)) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x n %d stride %d su %08x sv %08x", T[fi].name, i, ret[0], ret[1], n, stride, *(u32 *)&su, *(u32 *)&sv);
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld; %d of %d special cases differ\n", cov[0], cov[1], cov[2], cov[3], badsp, N / 5); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
