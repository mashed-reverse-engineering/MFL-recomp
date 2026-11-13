/* rw_d3d9vsu_check.c: bit-exact check of the librw DLL's d3d9vsu exports (group d3d9vsu, librw-mfl capi/d3d9vsu.cpp) against the ORIGINAL code of MFL.exe, both
   run natively (Wine).
   usage: rw_d3d9vsu_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9vsu_check.sh)
   Rows: _rwD3D9VSGetNormalInLocalSpace 0x50c1a0, _rwD3D9VSGetPointInLocalSpace 0x50c2e0, _rwD3D9VSGetRadiusInLocalSpace 0x50c3d0.  Every exe callee (orthonormal
   inverse 0x50c110, RwMatrixInvert 0x4d4490, transpose thunk 0x4d3d40, the D3DX thunks 0x4fbc73 / 0x4fbc49 (stdcall), _rwInvSqrt 0x4d5470) is a logging stand-in
   that writes pseudo-random results.  The exe state the rows touch (world matrix 0x7d7a30, caches 0x7d7a34 / 0x7d7a38, buffers 0x62ff90 / 0x62ffd0) is preset per call
   and logged after it, with the output vector.  Each call runs at x87 control word 0x7f, 0x27f or 0x37f.  "rev" runs the original first.  va:byte patches the
   ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
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

#define WORLD 0x7d7a30u
#define INV 0x7d7a34u
#define INVT 0x7d7a38u
#define INVBUF 0x62ff90u
#define TBUF 0x62ffd0u
#define IDENT 0x5ec460u
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void lgwords(const void *p, u32 n) { u32 k; for (k = 0; k < n; k++) lg(((const u32 *)p)[k]); }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 16)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) { u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); } p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p; }
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))
static void setcw(u32 cw) { __asm__ volatile("fninit; fldcw %0" :: "m"(cw)); }
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }
static u32 getcw(void) { u32 cw = 0; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw & 0xffff; }
static u32 fbits(void) {      /* plain values, zeros, infinities, quiet and signalling NaNs, denormals, any bit pattern */
  static const u32 v[] = {0x3f800000, 0xbf800000, 0x3f000000, 0x40490fdb, 0, 0x80000000, 0x7f800000, 0xff800000, 0x7fc00000, 0x7f800001, 0xffa00000,
                          0x00000001, 0x41200000, 0x3eaaaaab, 0x3f13cd3a, 0x42c80000};
  return rnd() % 4 == 0 ? rnd() : v[rnd() % 16];
}
static u32 hfbits(void) { static const u32 v[] = {0x3f800000, 0x3e800000, 0x40000000, 0, 0x7fc00000, 0x7f800001, 0x3f13cd3a}; return hrnd() % 3 ? hrnd() : v[hrnd() % 7]; }

/* ---- stand-ins (same on both sides) ---- */
static void fill(u32 dst, u32 n) { u32 k; for (k = 0; k < n; k++) W(dst + 4 * k) = hrnd(); }
static void __cdecl h_orthoinv(u32 dst, u32 src) { lg(0xa50c110); lg(dst); lg(src); lgwords((void *)(uintptr_t)src, 16); fill(dst, 16); }
static void __cdecl h_invert(u32 dst, u32 src) { lg(0xa4d4490); lg(dst); lg(src); lgwords((void *)(uintptr_t)src, 16); fill(dst, 16); }
static void __cdecl h_transpose(u32 dst, u32 src) { lg(0xa4d3d40); lg(dst); lg(src); lgwords((void *)(uintptr_t)src, 16); fill(dst, 16); }
static u32 aptr(u32 p) { return (p >= P(arena) && p < P(arena) + ARENA) || (p >= LO && p < HI) ? p : 0x5ac00000u; }      /* a stack local's address differs per side */
static u32 __stdcall h_xfnormal(u32 out, u32 in, u32 m) { lg(0xa4fbc73); lg(aptr(out)); lg(aptr(in)); lg(out == in); lg(m); lgwords((void *)(uintptr_t)in, 3); lgwords((void *)(uintptr_t)m, 16);
  W(out) = hfbits(); W(out + 4) = hfbits(); W(out + 8) = hfbits(); return hrnd(); }
static u32 __stdcall h_xfcoord(u32 out, u32 in, u32 m) { lg(0xa4fbc49); lg(aptr(out)); lg(aptr(in)); lg(out == in); lg(m); lgwords((void *)(uintptr_t)in, 3); lgwords((void *)(uintptr_t)m, 16);
  W(out) = hfbits(); W(out + 4) = hfbits(); W(out + 8) = hfbits(); return hrnd(); }
static float __cdecl h_invsqrt(u32 bits) { union { u32 u; float f; } r; lg(0xa4d5470); lg(bits); r.u = hfbits(); return r.f; }

enum { R_NORMAL, R_POINT, RADIUS, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37__rwD3D9VSGetNormalInLocalSpace", 0x50c1a0}, {"rw37__rwD3D9VSGetPointInLocalSpace", 0x50c2e0},
  {"rw37__rwD3D9VSGetRadiusInLocalSpace", 0x50c3d0}};
typedef void (__cdecl *G2)(u32, u32);
static void *fn[2][NF]; static long ncalls[NF];

static u32 pickcache(u32 other) {      /* 0, the identity marker, the inverse buffer, the transpose buffer, an arena matrix, or the other cache's value */
  switch (rnd() % 6) { case 0: case 1: return 0; case 2: return IDENT; case 3: return INVBUF; case 4: return rnd() % 2 ? TBUF : other; default: return P(rblock(0x40)); }
}
static void lgstate(void) { lg(W(WORLD)); lg(W(INV)); lg(W(INVT)); lgwords((void *)(uintptr_t)INVBUF, 32); }
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  nlg = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u;
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % NF, k, world = 0, *in, *out;
    lg(0x57e90000u + s); lg(op);
    ap = 0; memset(arena, 0, ARENA);
    if (rnd() % 6) {
      u8 *m = rblock(0x40); u32 fl = rnd();
      switch (rnd() % 4) { case 0: fl |= 0x20000; break; case 1: fl = (fl & ~0x20000u) | 3; break; case 2: fl &= ~0x20003u; fl |= rnd() % 3; break; default: break; }
      *(u32 *)(m + 0xc) = fl; world = P(m);
    }
    W(WORLD) = world; W(INV) = pickcache(0); W(INVT) = pickcache(W(INV));
    for (k = 0; k < 32; k++) W(INVBUF + 4 * k) = rnd();
    in = (u32 *)rblock(16); for (k = 0; k < 3; k++) in[k] = fbits();
    out = rnd() % 5 == 0 ? in : (u32 *)rblock(16);
    setcw(pickcw());
    if (op == RADIUS) { union { u32 u; float f; } r; r.u = fbits(); ((void (__cdecl *)(float, u32 *))f[op])(r.f, out); }
    else ((G2)f[op])(P(in), P(out));
    ncalls[op]++; lg(getcw()); setcw(0x27f);
    lgwords(out, 4); lgwords(in, 4); lgstate(); if (world) lgwords((void *)(uintptr_t)world, 16);
  }
}
static u32 *save_log, nsave;
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 12 ? i - 12 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  return 0;
}
static volatile u32 cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH %08lx at %p seed %u\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; double tot = 0;
  void (__cdecl *bind)(void *); static void *tbl[16];
  /* table order = d3d9vsu.cpp enum U */
  static const u32 t[] = {WORLD, INV, INVT, INVBUF, TBUF, IDENT, 0x50c110, 0x4d4490, 0x4d3d40, 0x4fbc73, 0x4fbc49, 0x4d5470};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 2000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA);
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9vsu_bind");
  if (!bind) { printf("mflrw_d3d9vsu_bind MISSING export\n"); return 1; }
  jmp_patch(0x50c110, (void *)h_orthoinv); jmp_patch(0x4d4490, (void *)h_invert); jmp_patch(0x4d3d40, (void *)h_transpose);
  jmp_patch(0x4fbc73, (void *)h_xfnormal); jmp_patch(0x4fbc49, (void *)h_xfcoord); jmp_patch(0x4d5470, (void *)h_invsqrt);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind(tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  { char line[96]; sprintf(line, "%.0f log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL"); fputs(line, stdout); }      /* one write: the DLL's exit line must not split it */
  printf("DONE\n");
  return bad != 0;
}
