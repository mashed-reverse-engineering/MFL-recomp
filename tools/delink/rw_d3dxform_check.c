/* rw_d3dxform_check.c: behavioural check of the librw DLL's d3dxform exports (tools/delink/rw_d3dxform.txt: the transform / stream / declaration caches of
   d3d9device.obj, librw-mfl capi/d3dxform.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3dxform_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3dxform_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c (SetTransform 44, SetVertexDeclaration 87, SetStreamSource 100; any
   other method = TRAP).  RwFreeListCreate / Purge get a jmp to a recorder at their original entry and RwEngineInstance 0x7d2128 points to a fake engine whose
   free-list alloc / free (+0x118 / +0x11c) are recorders on a bump arena; so both sides make the same calls into the same recorders.
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the
   original addresses.  Logged after every op: the device / recorder calls, the return value (RwD3D9SetTransform / SetTransformWorld; the others are void), the
   control word, the 5 last-used words 0x62e760..0x62e770, the stream caches 0x7d21f0..0x7d2230, the world identity flag, the matrix free list, the 0x104 matrix
   pointers and a hash of the matrices behind them, the world D3DMATRIX scratch 0x62e7f0 and the GetTransform output.
   Extra arguments va:byte patch the ORIGINAL's code first (negative fixtures: must give DIFF).  Exit 1 on DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "fakedev.c"
typedef uint32_t u32; typedef uint8_t u8;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
#define LO 0x4b0000u
#define HI 0x94b000u
static unsigned char mfl_area[0x700000] __attribute__((aligned(4096)));
static int load_image(const char *path) {
  FILE *f = fopen(path, "rb"); unsigned char *d; long sz; IMAGE_NT_HEADERS32 *nt; IMAGE_SECTION_HEADER *s; int i; DWORD old;
  if (!f) return 0;
  fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET); d = malloc(sz); fread(d, 1, sz, f); fclose(f);
  if ((uintptr_t)mfl_area > LO || (uintptr_t)mfl_area + sizeof mfl_area < HI) { printf("bss block at %p does not cover %x..%x\n", mfl_area, LO, HI); return 0; }
  if (!VirtualProtect((void *)LO, HI - LO, PAGE_EXECUTE_READWRITE, &old)) { printf("VirtualProtect failed %lu\n", GetLastError()); return 0; }
  nt = (IMAGE_NT_HEADERS32 *)(d + ((IMAGE_DOS_HEADER *)d)->e_lfanew); s = IMAGE_FIRST_SECTION(nt);
  for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
    u32 va = 0x400000 + s[i].VirtualAddress, n = s[i].SizeOfRawData < s[i].Misc.VirtualSize || !s[i].Misc.VirtualSize ? s[i].SizeOfRawData : s[i].Misc.VirtualSize, k;
    for (k = 0; k < n; k++) if (va + k >= LO && va + k < HI) ((unsigned char *)(uintptr_t)(va + k))[0] = d[s[i].PointerToRawData + k];
  }
  return 1;
}

#define DEVVAR 0x7d2240u
#define ENGVAR 0x7d2128u
#define LAST0 0x62e760u      /* FVF, declaration, VS, PS, IB */
#define STREAMS 0x7d21f0u
#define WIDENT 0x7d2284u
#define SLOTS 0x7d2288u
#define MATFL 0x7d2698u
#define IDENT 0x5ea090u
#define WD 0x62e7f0u
#define T 0xc0de0000u

static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }

static u8 *arena; static u32 narena, nlist;
#define ARENA 0x800000u
static u32 eng[0x50];
static void *__cdecl fe_alloc(u32 list, u32 hint) { u8 *p; lg(T | 0x118); lg(list); lg(hint); if (narena + 0x40 > ARENA) { printf("ARENA full\n"); fflush(stdout); ExitProcess(6); } p = arena + narena; narena += 0x40; memset(p, 0xcd, 0x40); lg((u32)(uintptr_t)p); return p; }
static void __cdecl fe_free(u32 list, u32 p) { lg(T | 0x11c); lg(list); lg(p); }
static u32 __cdecl st_flcreate(u32 a, u32 b, u32 c, u32 d) { lg(T | 13); lg(a); lg(b); lg(c); lg(d); return 0xf1000000u + ++nlist; }
static void __cdecl st_flpurge(u32 l) { lg(T | 15); lg(l); }
static const struct { u32 va; void *fn; } STUB[] = { {0x4e90b0, st_flcreate}, {0x4e9510, st_flpurge} };
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

enum { CLRSH, CLRMAT, SETDECL, SSS, STREAMS2, SETT, GETT, SETW, NALL };
static const struct { const char *dll; u32 va; int isvoid; } F[NALL] = {
  {"rw37_D3D9ClearCacheShaders", 0x4d87e0, 1}, {"rw37_D3D9ClearCacheMatrix", 0x4d8820, 1}, {"rw37__rwD3D9SetVertexDeclaration", 0x4db1b0, 1},
  {"rw37_RwD3D9SetStreamSource", 0x4db270, 1}, {"rw37__rwD3D9SetStreams", 0x4db2d0, 1}, {"rw37_RwD3D9SetTransform", 0x4db470, 0},
  {"rw37_RwD3D9GetTransform", 0x4db5d0, 1}, {"rw37_RwD3D9SetTransformWorld", 0x4db600, 0}};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL];

static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 *mats, *strm, *out;      /* 8 candidate matrices / LTMs, 2 streams, GetTransform output */
#define NM 8
static void randmat(u32 *m) {      /* often the identity or a copy of a cached matrix, sometimes one word off */
  u32 q = rnd() % 4, st = rnd() % 0x104, k;
  if (q == 0) memcpy(m, (void *)IDENT, 64);
  else if (q == 1 && W(SLOTS + 4 * st)) memcpy(m, (void *)(uintptr_t)W(SLOTS + 4 * st), 64);
  else if (q == 2) memcpy(m, (void *)WD, 64);
  else for (k = 0; k < 16; k++) m[k] = rnd() % 3 ? rnd() % 5 : rnd();
  if (rnd() % 3 == 0) m[rnd() % 16] ^= 1u << (rnd() % 32);
}
static u32 pickstate(void) { static const u32 s[] = { 0x100, 0x100, 2, 3, 0x10, 0x11, 0x17, 0x101, 0x103, 0x100 }; return rnd() % 4 ? s[rnd() % 10] : rnd() % 0x104; }
static u32 pickvb(void) { return rnd() % 5 == 0 ? 0 : rnd() % 4 == 0 ? 0xffffffffu : 0xb0f00000u + rnd() % 4; }
static void reset(u32 seed, unsigned short cw) {
  u32 a; int i;
  nlg = 0; rs = seed * 2654435761u + cw + 1;
  for (a = LAST0; a <= LAST0 + 0x10; a += 4) W(a) = rnd() % 2 ? 0xffffffffu : rnd() % 4;
  for (a = STREAMS; a < STREAMS + 0x40; a += 4) W(a) = rnd() % 3;
  for (a = WIDENT; a <= MATFL; a += 4) W(a) = 0;
  for (a = WD; a < WD + 0x40; a += 4) W(a) = rnd();
  narena = 0; nlist = 0; fakedev_hr = 0;
  for (i = 0; i < NM; i++) randmat(mats + 16 * i);
}
static u32 hashw(const u8 *p, u32 n) { u32 h = 0x811c9dc5u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 0x01000193u; return h; }
static u32 slots_hash(void) {
  u32 h = 0, i;
  for (i = 0; i < 0x104; i++) { u32 p = W(SLOTS + 4 * i); h = (h ^ p) * 0x01000193u; if (p) h ^= hashw((u8 *)(uintptr_t)p, 64) * (2 * i + 1); }
  return h;
}

static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s, i;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, a2 = 0, a3 = 0, r; int k;
    if (s == 0) k = CLRMAT;
    else if (s == 1) k = CLRSH;
    else {
      u32 q = rnd() % 100;
      k = q < 2 ? CLRSH : q < 4 ? CLRMAT : q < 14 ? SETDECL : q < 26 ? SSS : q < 38 ? STREAMS2 : q < 64 ? SETT : q < 72 ? GETT : SETW;
    }
    fakedev_hr = rnd() % 10 == 0 ? (int32_t)0x8876086c : rnd() % 20 == 0 ? 1 : 0;
    memset(out, 0xee, 64);
    switch (k) {
    case SETDECL: a0 = rnd() % 4 == 0 ? 0xffffffffu : 0xdec10000u + rnd() % 4; break;
    case SSS: a0 = rnd() % 4; a1 = pickvb(); a2 = rnd() % 3 ? 0 : rnd() % 4 * 0x100; a3 = rnd() % 3 ? 0x1c : rnd() % 4 * 4; break;
    case STREAMS2:
      for (i = 0; i < 2; i++) { strm[4 * i] = pickvb(); strm[4 * i + 1] = rnd() % 3 ? 0 : rnd() % 4 * 0x100; strm[4 * i + 2] = rnd() % 3 ? 0x1c : rnd() % 4 * 4; strm[4 * i + 3] = rnd(); }
      if (rnd() % 3 == 0) for (i = 0; i < 2; i++) { strm[4 * i] = W(STREAMS + 16 * i); strm[4 * i + 1] = W(STREAMS + 16 * i + 4); strm[4 * i + 2] = W(STREAMS + 16 * i + 8); }
      a0 = (u32)(uintptr_t)strm; a1 = rnd() % 3 ? rnd() % 2 : rnd(); break;
    case SETT: { u32 *m = mats + 16 * (rnd() % NM); a0 = pickstate(); randmat(m); a1 = rnd() % 6 == 0 ? 0 : (u32)(uintptr_t)m; break; }
    case GETT: a0 = pickstate(); a1 = rnd() % 8 == 0 ? 0 : (u32)(uintptr_t)out; break;
    case SETW: { u32 *m = mats + 16 * (rnd() % NM); randmat(m);
      if (rnd() % 3 == 0) { for (i = 0; i < 16; i++) if (i % 4 != 3) m[i] = W(WD + 4 * i); }      /* the LTM of the last call */
      m[3] = rnd() % 4 == 0 ? 0x20000 | (rnd() & 0x3) : rnd() % 3 ? 0 : rnd() & ~0x20000u;
      a0 = rnd() % 8 == 0 ? 0 : (u32)(uintptr_t)m; break; }
    }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1); lg(a2); lg(a3);
    cw_set(cw);
    r = ((G4)f[k])(a0, a1, a2, a3); ncalls[k]++;
    lg(F[k].isvoid ? 0 : r); lg(cw_now());
    cw_set(0x27f);
    for (u32 a = LAST0; a <= LAST0 + 0x10; a += 4) lg(W(a));
    for (u32 a = STREAMS; a < STREAMS + 0x40; a += 4) lg(W(a));
    lg(W(WIDENT)); lg(W(MATFL)); lg(slots_hash());
    for (u32 a = WD; a < WD + 0x40; a += 4) lg(W(a));
    for (i = 0; i < 16; i++) lg(out[i]);
  }
}

static u32 *save_log, nsave; static int bad_dumps;
static int compare(u32 seed, unsigned short cw) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u cw %x: log differs at entry %u (step %u op %s): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, op < NALL ? F[op].dll : "?",
           i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    if (bad_dumps++ < 2) for (k = i > 24 ? i - 24 : 0; k < i + 8 && k < n; k++) printf("    %6u  %08x %08x%s\n", k, save_log[k], LG[k], save_log[k] != LG[k] ? "  <" : "");
    return 1;
  }
  return 0;
}
static void sink(u32 w) { lg(w); }
static volatile int cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %d\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH %08lx at %p seed %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0, ci; u32 seed; long tot = 0;
  static const unsigned short cws[2] = { 0x7f, 0x27f };
  int (__cdecl *bind)(int, void *const *);
  static void *table[15];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); mats = malloc(64 * NM); strm = malloc(32); out = malloc(64);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  eng[0x118 / 4] = (u32)(uintptr_t)fe_alloc; eng[0x11c / 4] = (u32)(uintptr_t)fe_free; W(ENGVAR) = (u32)(uintptr_t)eng;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test of the fake device: the original ClearCacheMatrix + a world transform + a stream give a non-empty device log and no trap */
  reset(1, 0x7f); ((G4)(uintptr_t)F[CLRMAT].va)(0, 0, 0, 0); ((G4)(uintptr_t)F[SETT].va)(0x100, (u32)(uintptr_t)mats, 0, 0); ((G4)(uintptr_t)F[SSS].va)(0, 0xb0f00001, 0, 0x1c);
  for (i = 0; i < (int)nlg && LG[i] != (FAKEDEV_TAG | 100); i++);
  printf("fakedev selftest: original ClearCacheMatrix / SetTransform / SetStreamSource logged %u words (%s)\n", nlg, i < (int)nlg ? "ok" : "NO SetStreamSource");
  if (i >= (int)nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3dxform_bind");
  if (!bind) { printf("mflrw_d3dxform_bind MISSING export\n"); return 1; }
  table[0] = (void *)DEVVAR; table[1] = (void *)ENGVAR;
  for (i = 0; i < 5; i++) table[2 + i] = (void *)(uintptr_t)(LAST0 + 4 * i);
  table[7] = (void *)STREAMS; table[8] = (void *)WIDENT; table[9] = (void *)SLOTS; table[10] = (void *)MATFL; table[11] = (void *)IDENT; table[12] = (void *)WD;
  table[13] = (void *)(uintptr_t)STUB[0].va; table[14] = (void *)(uintptr_t)STUB[1].va;
  if (bind(14, table) || !bind(15, table)) { printf("mflrw_d3dxform_bind count check wrong (DIFF)\n"); return 1; }
  for (i = 0; i < NALL; i++) {
    fn[1][i] = (void *)(uintptr_t)F[i].va;
    fn[0][i] = (void *)GetProcAddress(m, F[i].dll);
    if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; }
  }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps x CW 0x7f / 0x27f\n", argv[1], seeds, steps);
  for (ci = 0; ci < 2; ci++)
    for (seed = 1; seed <= (u32)seeds; seed++) {
      cur_seed = seed;
      run(0, seed, steps, cws[ci]);
      nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
      run(1, seed, steps, cws[ci]);
      tot += nlg;
      if (compare(seed, cws[ci]) && ++bad >= 5) goto done;
    }
done:
  for (i = 0; i < NALL; i++) printf("%-45s %9ld calls (both sides)\n", F[i].dll, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
