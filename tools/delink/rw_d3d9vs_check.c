/* rw_d3d9vs_check.c: bit-exact check of the librw DLL's d3d9vs exports (group d3d9vs, librw-mfl capi/d3d9vs.cpp) against the ORIGINAL code of MFL.exe, both run
   natively (Wine).
   usage: rw_d3d9vs_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9vs_check.sh)
   Rows: _rpD3D9VertexShaderCacheOpen 0x50b4e0, _rpD3D9VertexShaderCachePurge 0x509620, rpD3D9GetVertexShader 0x509690, GetMaterialShaderCallBack 0x50b5f0.
   The shader generator 0x509870 (K3b), the shader free 0x4dbc40 and _memmove 0x4bdba0 are stand-ins (memmove runs the real exe code: deterministic, same both sides).
   The cache (0x7d54e0), count / last / version / const globals, the constants table (0x7d78e0), the engine frame word (0x7d2128) and the device caps (0x947600)
   are exe state: reset per seed, then they evolve across the 300 steps; the full cache array + globals + log are compared.  Each call runs at x87 control word 0x7f,
   0x27f or 0x37f (these rows are integer, but the fixture exercises the control words anyway).  "rev" = original first.  va:byte patches the ORIGINAL (must DIFF).
   Exit 1 on DIFF / MISSING / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
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

#define CACHE 0x7d54e0u
#define NUMEFFECT 0x7d78e0u
#define COUNT 0x7d79e0u
#define LASTHEAD 0x7d79e4u
#define USEVER 0x7d79e8u
#define MAXCONST 0x7d79ecu
#define ENGVAR 0x7d2128u
#define CAPS 0x947600u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
static void setcw(u32 cw) { __asm__ volatile("fninit; fldcw %0" :: "m"(cw)); }
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }

/* ---- stand-ins (same both sides) ---- */
static u32 __cdecl h_generate(u32 desc, u32 flags) { u32 r = 0x5ade0000u + (hrnd() & 0xffff); lg(0xa509870); lg(W(desc)); lg(flags ? W(flags) : 0); lg(flags ? W(flags + 4) : 0); lg(r); return r; }
static void __cdecl h_delete(u32 shader) { lg(0xa4dbc40); lg(shader); }

enum { OPEN, PURGE, GET, GETMAT, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37__rpD3D9VertexShaderCacheOpen", 0x50b4e0}, {"rw37__rpD3D9VertexShaderCachePurge", 0x509620},
  {"rw37_rpD3D9GetVertexShader", 0x509690}, {"rw37_rxD3D9VertexShaderDefaultGetMaterialShaderCallBack", 0x50b5f0}};
static void *fn[2][NF]; static long ncalls[NF];

static void lgcache(void) {
  u32 i, n = W(COUNT); lg(n); lg(W(LASTHEAD)); lg(W(USEVER)); lg(W(MAXCONST));
  for (i = 0; i < (n < 260 ? n : 260) * 5; i++) lg(W(CACHE + 4 * i));            /* the live entries */
  for (i = 0; i < 0x40; i++) lg(W(NUMEFFECT + 4 * i));
}
static void reset(u32 seed) {
  u32 k;
  nlg = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; *(u16 *)(eng + 8) = 0;
  for (k = 0; k < 0x100 * 5; k++) W(CACHE + 4 * k) = 0;                           /* a clean 256-entry cache region */
  for (k = 0; k < 0x40; k++) W(NUMEFFECT + 4 * k) = seed * 0x9e3779b1u ^ k * 0x85ebca6bu;      /* exactly the 64-dword table (COUNT etc. start right after it at
                                     0x7d79e0); nonzero so a misaligned clear by CacheOpen shows (mutation round 1); not from rnd(): every other input stays as before */
  W(COUNT) = 0; W(LASTHEAD) = CACHE; W(USEVER) = 0; W(MAXCONST) = 0;                /* set after the table so nothing clobbers them */
  for (k = 0; k < 0x100; k++) W(CAPS + 4 * k) = 0;
}
static int wide;      /* per seed: 0 = small key pool + frequent Open/Purge (sorted cache, many reuse / found hits); 1 = lookups only with a wide pool, so the
                         count passes 256 and the eviction path runs (it overwrites a key in place and breaks the sort order; the kernel's search is the original's
                         exact probe sequence, so both stay in step on the unsorted array too) */
static u32 keybits(void) {      /* 0xffffffff = the post-Open/Purge edge key */
  if (wide) return rnd() % 16 == 0 ? 0x1000 + rnd() % 64 : 0x1000 + rnd() % 4000;
  return rnd() % 8 == 0 ? 0xffffffffu : 0x1000 + rnd() % 220;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  wide = seed % 4 == 0;
  /* open once at the start of most seeds (some start from the zeroed cache to vary; wide seeds always open, then never reset) */
  if (wide || seed % 3) { setcw(pickcw()); ((void (__cdecl *)(void))f[OPEN])(); ncalls[OPEN]++; }
  for (s = 0; s < steps; s++) {
    u32 op, key, k, flags[2];
    u32 r2 = rnd() % 100;
    op = wide ? (r2 < 85 ? GET : GETMAT) : r2 < 70 ? GET : r2 < 82 ? GETMAT : r2 < 92 ? OPEN : PURGE;
    lg(0x57e90000u + s); lg(op);
    *(u16 *)(eng + 8) = (u16)(s * 7 + 1);                                         /* the engine frame word advances */
    if (op == OPEN) {
      static const u32 vs[] = {0, 0x100, 0x101, 0x200, 0x201, 0x300, 0x301, 0xfffe0300u, 0x55};
      W(CAPS + 0xc4) = vs[rnd() % 9]; W(CAPS + 0xc8) = rnd() % 4 == 0 ? rnd() : 0x40 + rnd() % 0x200;
      W(CAPS + 0xec) = rnd(); W(CAPS + 0xfc) = rnd() % 3 ? rnd() % 4 : rnd();
      setcw(pickcw()); ((void (__cdecl *)(void))f[OPEN])(); ncalls[OPEN]++;
    } else if (op == PURGE) {
      setcw(pickcw()); ((void (__cdecl *)(void))f[PURGE])(); ncalls[PURGE]++;
    } else {
      static u32 descbuf[4], flagbuf[2];
      key = keybits(); descbuf[0] = key; descbuf[1] = rnd(); flags[0] = rnd(); flags[1] = rnd();
      flagbuf[0] = flags[0]; flagbuf[1] = flags[1];
      u32 ret;
      setcw(pickcw());
      if (op == GET) ret = ((u32 (__cdecl *)(u32, u32))f[GET])((u32)(uintptr_t)descbuf, (u32)(uintptr_t)flagbuf), ncalls[GET]++;
      else ret = ((u32 (__cdecl *)(u32, u32, u32))f[GETMAT])(rnd(), (u32)(uintptr_t)descbuf, (u32)(uintptr_t)flagbuf), ncalls[GETMAT]++;
      lg(ret); lg(flagbuf[0]); lg(flagbuf[1]);      /* the returned shader handle is the row's main output (mutation round 1: was not logged) */
    }
    lgcache();
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
  /* table order = d3d9vs.cpp enum V */
  static const u32 t[] = {CACHE, NUMEFFECT, COUNT, LASTHEAD, USEVER, MAXCONST, ENGVAR, CAPS, 0x509870, 0x4dbc40, 0x4bdba0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 2000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9vs_bind");
  if (!bind) { printf("mflrw_d3d9vs_bind MISSING export\n"); return 1; }
  jmp_patch(0x509870, (void *)h_generate); jmp_patch(0x4dbc40, (void *)h_delete);      /* 0x4bdba0 memmove stays the real exe code */
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
  for (i = 0; i < NF; i++) printf("%-52s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  { char line[96]; sprintf(line, "%.0f log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL"); fputs(line, stdout); }      /* one write: the DLL's exit line must not split it */
  printf("DONE\n");
  return bad != 0;
}
