/* rw_d3dlights_check.c: behavioural check of the librw DLL's d3dlights exports (tools/delink/rw_d3dlights.txt: the 8 functions of d3d9lights.obj and the 3 light
   setters of d3d9device.obj, librw-mfl capi/d3dlights.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3dlights_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3dlights_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c (SetLight 51, LightEnable 53; any other method = TRAP, the run fails).
   The exe callees outside the group (RwD3D9GetCaps, RwFrameGetLTM, RpLightGetConeAngle (returns a long double: the port must not round st0), RwD3D9SetRenderState)
   get a jmp to a recorder at their original entry, and RwEngineInstance 0x7d2128 points to a fake engine (current world +4, malloc / free / realloc +0x108 / +0x10c /
   +0x110 as recorders on a bump arena, realloc in place while the block's capacity allows); so both sides make the same calls into the same recorders.
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the
   original addresses.  Logged after every op: the device / recorder calls, the return value (not for the void Close), the control word, the module block
   0x7d53d0..0x7d54d0, the ambient colour, MaxNumLights / LightsCache and a hash of the cache, of the index lists (up to their capacity) and of the light extensions.
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

#define DEVVAR 0x7d2240u
#define ENGVAR 0x7d2128u
#define BSS0 0x7d53d0u
#define BSS1 0x7d54d0u
#define LIGHTOFFVAR 0x7d54a0u
#define AMB 0x62e738u
#define NUMLIGHTS 0x7d269cu
#define CACHE 0x7d26a0u
#define LOFF 0x60      /* light plugin offset used by the harness */
#define T 0xc0de0000u  /* recorder tags */

/* log */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }

/* the fake engine's memory table on a bump arena: a 16-byte header {capacity} before each block */
static u8 *arena; static u32 narena;
#define ARENA 0x2000000u
static u32 eng[0x50];
static u8 *blk_new(u32 c) { u8 *p; c = (c + 15) & ~15u; if (narena + c + 16 > ARENA) { printf("ARENA full\n"); fflush(stdout); ExitProcess(6); } p = arena + narena + 16; *(u32 *)(p - 16) = c; narena += c + 16; memset(p, 0xcd, c); return p; }
static void *__cdecl fe_malloc(u32 size, u32 hint) { u8 *p; lg(T | 0x108); lg(size); lg(hint); p = blk_new(size); lg((u32)(uintptr_t)p); return p; }
static void __cdecl fe_free(u32 p) { lg(T | 0x10c); lg(p); }
static void *__cdecl fe_realloc(u8 *p, u32 size, u32 hint) {
  u8 *q; u32 c = *(u32 *)(p - 16);
  lg(T | 0x110); lg((u32)(uintptr_t)p); lg(size); lg(hint);
  if (size <= c) q = p; else { q = blk_new(size > 2 * c ? size : 2 * c); memcpy(q, p, c); }
  lg((u32)(uintptr_t)q); return q;
}
/* recorders at the callee entries */
static u8 *caps;
static u32 __cdecl st_getcaps(void) { lg(T | 1); return (u32)(uintptr_t)caps; }
static u32 __cdecl st_getltm(u32 frame) { lg(T | 2); lg(frame); return frame; }      /* the frame object holds its LTM at +0 */
static long double __cdecl st_cone(u8 *l) { float f; lg(T | 3); lg((u32)(uintptr_t)l); memcpy(&f, l + 0x40, 4); return (long double)f * 7.0L / 3.0L; }      /* not a float */
static u32 __cdecl st_setrs(u32 s, u32 v) { lg(T | 4); lg(s); lg(v); return 0x5e700000u ^ s ^ v; }
static const struct { u32 va; void *fn; } STUB[] = { {0x4dbca0, st_getcaps}, {0x4d0ff0, st_getltm}, {0x4f4a70, st_cone}, {0x4e3c00, st_setrs} };
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

/* entries ([0] DLL export, [1] original) */
enum { DESTR, COPY, OPEN, CLOSE, GLOBAL, DIRE, LOCAL, ENABLE, SETL, ENL, EQ, NALL };
static const struct { const char *dll; u32 va; int isvoid; } F[NALL] = {
  {"rw37_rpD3D9LightDestructor", 0x505180, 0}, {"rw37_rpD3D9LightCopy", 0x505270, 0}, {"rw37__rwD3D9LightsOpen", 0x5052b0, 0},
  {"rw37__rwD3D9LightsClose", 0x505510, 1}, {"rw37__rwD3D9LightsGlobalEnable", 0x5055a0, 0}, {"rw37__rwD3D9LightDirectionalEnable", 0x505780, 0},
  {"rw37__rwD3D9LightLocalEnable", 0x5058b0, 0}, {"rw37__rwD3D9LightsEnable", 0x505b20, 0}, {"rw37_RwD3D9SetLight", 0x4db740, 0},
  {"rw37_RwD3D9EnableLight", 0x4db830, 0}, {"rw37__rwD3D9LightsEqual", 0x4db880, 0}};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL];

static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
/* objects outside the exe state */
#define NL 24
#define NFR 8
static u8 *lights, *frames, *world, *dl;      /* dl: two D3DLIGHT9 for SetLight / LightsEqual */
static u8 *L(int i) { return lights + 0x80 * i; }
static u32 rfloat(void) {
  static const u32 sp[] = { 0x7fc00000, 0x7f800001, 0xff800000, 0x00000000, 0x80000000, 0x3f800000, 0x7fc01234, 0x00000001 };
  float f;
  if (rnd() % 12 == 0) return sp[rnd() % 8];
  f = (float)((int)(rnd() % 20000) - 10000) / (float)(1 + rnd() % 64);
  return *(u32 *)&f;
}
static void newlight(int i) {
  static const u8 st[] = { 1, 2, 0x80, 0x81, 0x82, 0x81, 0x80, 1, 3, 0x83 };
  u8 *l = L(i); int k;
  memset(l, 0, 0x80);
  l[1] = st[rnd() % 10]; l[2] = (u8)(rnd() % 3 ? 1 << (rnd() % 3) : rnd());
  *(u32 *)(l + 4) = (u32)(uintptr_t)(frames + 0x60 * (rnd() % NFR));
  *(u32 *)(l + 0x14) = rnd() % 8 == 0 ? (rnd() % 2 ? 0 : 0x80000000u) : rfloat();      /* radius (0 / -0: no local light) */
  for (k = 0; k < 4; k++) *(u32 *)(l + 0x18 + 4 * k) = rfloat();
  *(u32 *)(l + 0x40) = rnd() % 4 == 0 ? 0x3fc90fdb /* pi / 2 */ : rfloat();      /* cone angle source */
  *(u32 *)(l + LOFF) = rnd() % 6 ? 0xffffffffu : rnd() % 8;
  for (k = 1; k < 4; k++) *(u32 *)(l + LOFF + 4 * k) = rfloat();
}
static void randlight(u8 *d) {      /* a D3DLIGHT9, often equal to a cache entry in the compared fields */
  int k; u32 n = W(NUMLIGHTS);
  if (n && W(CACHE) && rnd() % 2) { memcpy(d, (u8 *)(uintptr_t)W(CACHE) + 0x6c * (rnd() % n), 0x68); if (rnd() % 2) ((u32 *)d)[rnd() % 26] ^= 1 << (rnd() % 32); }
  else { for (k = 0; k < 26; k++) ((u32 *)d)[k] = rnd() % 3 ? rfloat() : rnd() % 4; ((u32 *)d)[0] = rnd() % 5; }
}
static void reset(u32 seed, unsigned short cw) {
  u32 a; int i;
  nlg = 0; rs = seed * 2654435761u + cw + 1;
  for (a = BSS0; a < BSS1; a += 4) W(a) = 0;
  W(LIGHTOFFVAR) = LOFF; W(NUMLIGHTS) = 0; W(CACHE) = 0;
  for (i = 0; i < 4; i++) W(AMB + 4 * i) = rfloat();
  narena = 0; fakedev_hr = 0;
  memset(caps, 0, 0x130);
  for (i = 0; i < NFR * 0x18; i++) ((u32 *)frames)[i] = rfloat();
  for (i = 0; i < NL; i++) newlight(i);
  memset(world, 0, 0x80); *(u32 *)(world + 0x3c) = *(u32 *)(world + 0x40) = (u32)(uintptr_t)(world + 0x3c);
}
static void link_world(void) {      /* a random subset of the lights in the world's light list (link at +0x34) */
  u8 *head = world + 0x3c, *prev = head; int i, n = rnd() % 6 == 0 ? 0 : 1 + rnd() % 10, j = rnd() % NL, st = "\1\5\7\13"[rnd() % 4];      /* step coprime to NL = 24: n distinct lights */
  for (i = 0; i < n; i++, j = (j + st) % NL) { u8 *l = L(j); *(u32 *)prev = (u32)(uintptr_t)(l + 0x34); prev = l + 0x34; }
  *(u32 *)prev = (u32)(uintptr_t)head;
}
static u32 hashw(const u8 *p, u32 n) { u32 h = 0x811c9dc5u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 0x01000193u; return h; }
static u32 lists_hash(void) {
  u32 h = 0, i;
  if (W(NUMLIGHTS) && W(CACHE) && W(NUMLIGHTS) < 0x10000) h ^= hashw((u8 *)(uintptr_t)W(CACHE), W(NUMLIGHTS) * 0x6c);
  if (W(0x7d54b4)) h ^= 3 * hashw((u8 *)(uintptr_t)W(0x7d54b4), W(0x7d54ac) * 4);
  if (W(0x7d54b8)) h ^= 5 * hashw((u8 *)(uintptr_t)W(0x7d54b8), W(0x7d54c0) * 4);
  if (W(0x7d54c4)) h ^= 7 * hashw((u8 *)(uintptr_t)W(0x7d54c4), W(0x7d54cc) * 4);
  for (i = 0; i < NL; i++) h = (h ^ hashw(L(i) + LOFF, 16)) * 0x01000193u;
  return h;
}

static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, a2 = 0, a3 = 0, r; int k, i;
    if (s == 0) k = rnd() % 8 ? OPEN : GLOBAL;
    else {
      u32 q = rnd() % 100;
      k = q < 3 ? DESTR : q < 5 ? COPY : q < 7 ? OPEN : q < 8 ? CLOSE : q < 22 ? GLOBAL : q < 38 ? DIRE : q < 56 ? LOCAL : q < 70 ? ENABLE
        : q < 80 ? SETL : q < 88 ? ENL : EQ;
    }
    fakedev_hr = rnd() % 10 == 0 ? (int32_t)0x8876086c : rnd() % 20 == 0 ? 1 : 0;
    if (rnd() % 40 == 0) newlight(rnd() % NL);      /* a light created / changed */
    if (rnd() % 30 == 0) for (i = 0; i < NFR * 0x18; i++) if (rnd() % 4 == 0) ((u32 *)frames)[i] = rfloat();      /* frames moved */
    switch (k) {
    case DESTR: a0 = rnd() % 10 ? (u32)(uintptr_t)L(rnd() % NL) : 0; a1 = LOFF; a2 = 16; break;
    case COPY: a0 = rnd() % 10 ? (u32)(uintptr_t)L(rnd() % NL) : 0; a1 = rnd() % 10 ? (u32)(uintptr_t)L(rnd() % NL) : 0; a2 = LOFF; a3 = 16; break;
    case OPEN: { static const u32 ml[] = { 0, 1, 2, 3, 4, 8, 100, 0xffffffff }; *(u32 *)(caps + 0xa0) = ml[rnd() % 8]; break; }
    case GLOBAL: link_world(); a0 = rnd() % 3 ? 1 << (rnd() % 3) : rnd(); break;
    case DIRE: case LOCAL: a0 = (u32)(uintptr_t)L(rnd() % NL); break;
    case ENABLE: a0 = rnd() % 4 ? 1 : rnd() % 2 ? 0 : rnd(); break;
    case SETL: a0 = rnd() % 3 ? rnd() % (W(NUMLIGHTS) + 2) : rnd() % 40; randlight(dl); a1 = (u32)(uintptr_t)dl; break;
    case ENL: a0 = rnd() % 5 ? rnd() % (W(NUMLIGHTS) + 2) : rnd() % 40; a1 = rnd() % 4 ? rnd() % 2 : rnd(); break;
    case EQ: randlight(dl); if (rnd() % 2) { memcpy(dl + 0x68, dl, 0x68); if (rnd() % 2) ((u32 *)(dl + 0x68))[rnd() % 26] ^= 1 << (rnd() % 32); } else randlight(dl + 0x68);
      a0 = (u32)(uintptr_t)dl; a1 = (u32)(uintptr_t)(dl + 0x68); break;
    }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1); lg(a2); lg(a3);
    cw_set(cw);
    r = ((G4)f[k])(a0, a1, a2, a3); ncalls[k]++;
    lg(F[k].isvoid ? 0 : r); lg(cw_now());
    cw_set(0x27f);
    for (u32 a = BSS0; a < BSS1; a += 4) lg(W(a));
    for (i = 0; i < 4; i++) lg(W(AMB + 4 * i));
    lg(W(NUMLIGHTS)); lg(W(CACHE)); lg(lists_hash());
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
  static void *table[10];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); lights = malloc(0x80 * NL); frames = malloc(0x60 * NFR); world = malloc(0x80); dl = malloc(0xd0); caps = malloc(0x130);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  eng[1] = (u32)(uintptr_t)world; eng[0x108 / 4] = (u32)(uintptr_t)fe_malloc; eng[0x10c / 4] = (u32)(uintptr_t)fe_free; eng[0x110 / 4] = (u32)(uintptr_t)fe_realloc;
  W(ENGVAR) = (u32)(uintptr_t)eng;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test of the fake device: the original Open + a directional light + LightsEnable give a non-empty device log and no trap */
  reset(1, 0x7f); ((G4)(uintptr_t)F[OPEN].va)(0, 0, 0, 0); L(0)[1] = 1; ((G4)(uintptr_t)F[DIRE].va)((u32)(uintptr_t)L(0), 0, 0, 0); ((G4)(uintptr_t)F[ENABLE].va)(1, 0, 0, 0);
  for (i = 0; i < (int)nlg && LG[i] != (FAKEDEV_TAG | 53); i++);
  printf("fakedev selftest: original Open / directional / enable logged %u words (%s)\n", nlg, i < (int)nlg ? "ok" : "NO LightEnable");
  if (i >= (int)nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3dlights_bind");
  if (!bind) { printf("mflrw_d3dlights_bind MISSING export\n"); return 1; }
  table[0] = (void *)DEVVAR; table[1] = (void *)ENGVAR; table[2] = (void *)LIGHTOFFVAR; table[3] = (void *)AMB; table[4] = (void *)NUMLIGHTS; table[5] = (void *)CACHE;
  for (i = 0; i < 4; i++) table[6 + i] = (void *)(uintptr_t)STUB[i].va;
  if (bind(9, table) || !bind(10, table)) { printf("mflrw_d3dlights_bind count check wrong (DIFF)\n"); return 1; }
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
