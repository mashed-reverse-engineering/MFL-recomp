/* rw_d3d9nat_check.c: bit-exact check of the librw DLL's d3d9nat exports (group d3d9nat, librw-mfl capi/d3d9nat.cpp) against the ORIGINAL code of MFL.exe, both run
   natively (Wine).
   usage: rw_d3d9nat_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9nat_check.sh)
   Rows: _rpD3D9UsageFlagPluginAttach 0x509470, _rpGeometryNativeSize 0x501a30, _rpWorldSectorNativeSize 0x502130, RpBuildMeshGenerateDefaultTriStrip 0x5021c0.
   Stand-ins (logging, same both sides; their return values come from a per-seed hidden stream): RwEngineRegisterPlugin 0x4d2f20, RpGeometryRegisterPlugin 0x4f9110,
   RpWorldSectorRegisterPlugin 0x500b40, RpWorldSectorGetWorld 0x4f56f0, TriStripMeshGenerate 0x5021e0, and the vertex declaration's GetDeclaration (fake COM object,
   vtbl +0x10, stdcall) which writes a count and that many 8-byte elements (<= 20, the original's 0xa0-byte buffer).  The fake geometries / sectors / worlds / instance
   headers live in a static arena (same addresses both sides) and are re-randomised per call.  Logged: every stand-in's arguments, the return value, both plugin
   offset globals (0x7d54d8 / 0x7d54dc) after every call.  Each call runs at x87 control word 0x7f, 0x27f or 0x37f (integer rows; exercised anyway).  "rev" = original
   first.  va:byte patches the ORIGINAL (must DIFF).  Exit 1 on DIFF / MISSING / TIMEOUT / CRASH. */
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

#define GEOOFS 0x7d54d8u
#define SECOFS 0x7d54dcu
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
static void setcw(u32 cw) { __asm__ volatile("fninit; fldcw %0" :: "m"(cw)); }
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }
#define P(p) ((u32)(uintptr_t)(p))

/* arena: 4 geometries / sectors, 2 worlds, 4 headers, 2 declarations + one vtbl */
static u32 geo[4][0x20], sec[4][0x20], wld[2][0x10], hdr[4][0x20], decl[2][2], vtbl[8];
static u32 retpick(void) {      /* plugin register results: negative (failure), small offsets, wild values */
  u32 r = hrnd();
  switch (r % 6) { case 0: return 0xffffffffu; case 1: return 0x80000000u | (hrnd() & 0xff); case 2: return 0; case 3: return 4 + (hrnd() & 0xfc); default: return hrnd(); }
}
/* ---- stand-ins (same both sides) ---- */
static int __cdecl h_engreg(u32 size, u32 id, u32 ctor, u32 dtor) { u32 r = retpick(); lg(0xa4d2f20); lg(size); lg(id); lg(ctor); lg(dtor); lg(W(GEOOFS)); lg(W(SECOFS)); lg(r); return (int)r; }
static int __cdecl h_georeg(u32 size, u32 id, u32 c, u32 d, u32 cp) { u32 r = retpick(); lg(0xa4f9110); lg(size); lg(id); lg(c); lg(d); lg(cp); lg(W(GEOOFS)); lg(W(SECOFS)); lg(r); return (int)r; }
static int __cdecl h_secreg(u32 size, u32 id, u32 c, u32 d, u32 cp) { u32 r = retpick(); lg(0xa500b40); lg(size); lg(id); lg(c); lg(d); lg(cp); lg(W(GEOOFS)); lg(W(SECOFS)); lg(r); return (int)r; }
static u32 __cdecl h_getworld(u32 sector) { u32 r = P(wld[hrnd() & 1]); lg(0xa4f56f0); lg(sector); lg(r); return r; }
static u32 __cdecl h_tristrip(u32 bm, u32 a, u32 b) { u32 r = hrnd(); lg(0xa5021e0); lg(bm); lg(a); lg(b); lg(r); return r; }
static long __stdcall h_getdecl(u32 self, u8 *elems, u32 *num) {
  u32 n = hrnd() % 8 == 0 ? hrnd() : hrnd() % 21, k, m = n < 20 ? n : 20;
  lg(0xa0d3c1); lg(self);
  for (k = 0; k < m * 8; k++) elems[k] = (u8)hrnd();
  *num = n; lg(n);
  return 0;
}

enum { ATTACH, GEOSIZE, SECSIZE, TRISTRIP, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37__rpD3D9UsageFlagPluginAttach", 0x509470}, {"rw37__rpGeometryNativeSize", 0x501a30},
  {"rw37__rpWorldSectorNativeSize", 0x502130}, {"rw37_RpBuildMeshGenerateDefaultTriStrip", 0x5021c0}};
static void *fn[2][NF]; static long ncalls[NF];

static u32 rfield(void) { u32 r = rnd(); return r % 4 == 0 ? r : r % 4 == 1 ? 0 : r & 0xffff; }
static u32 rflags(void) { u32 r = rnd(); return r % 3 == 0 ? r & ~0x1000000u : r % 3 == 1 ? r | 0x1000000u : 0x1000000u; }
static void randomise(void) {
  int i, k;
  for (i = 0; i < 4; i++) {
    for (k = 0; k < 0x20; k++) { geo[i][k] = rfield(); sec[i][k] = rfield(); hdr[i][k] = rfield(); }
    geo[i][2] = rflags(); geo[i][0x58 / 4] = P(hdr[rnd() & 3]);
    sec[i][0x34 / 4] = rnd() % 4 ? P(hdr[rnd() & 3]) : 0;
    hdr[i][0x4c / 4] = P(decl[rnd() & 1]);
  }
  for (i = 0; i < 2; i++) { for (k = 0; k < 0x10; k++) wld[i][k] = rfield(); wld[i][2] = rflags(); }
}
static void reset(u32 seed) {
  nlg = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u;
  W(GEOOFS) = seed * 0x9e3779b1u; W(SECOFS) = seed * 0x85ebca6bu;
  decl[0][0] = decl[1][0] = P(vtbl); decl[0][1] = 0xdec10000u; decl[1][1] = 0xdec20000u;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % NF, ret, arg;
    lg(0x57e90000u + s); lg(op);
    randomise();
    setcw(pickcw());
    switch (op) {
    case ATTACH: ret = ((u32 (__cdecl *)(void))f[ATTACH])(); break;
    case GEOSIZE: arg = P(geo[rnd() & 3]); ret = ((u32 (__cdecl *)(u32))f[GEOSIZE])(arg); break;
    case SECSIZE: arg = P(sec[rnd() & 3]); ret = ((u32 (__cdecl *)(u32))f[SECSIZE])(arg); break;
    default: arg = rnd(); ret = ((u32 (__cdecl *)(u32, u32))f[TRISTRIP])(arg, rnd()); break;
    }
    ncalls[op]++;
    lg(ret); lg(W(GEOOFS)); lg(W(SECOFS));
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
  /* table order = d3d9nat.cpp enum V */
  static const u32 t[] = {0x4d2f20, 0x4f9110, 0x500b40, 0x509510, 0x509520, 0x54d840, 0x5094f0, GEOOFS, SECOFS, 0x4f56f0, 0x5021e0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 2000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9nat_bind");
  if (!bind) { printf("mflrw_d3d9nat_bind MISSING export\n"); return 1; }
  jmp_patch(0x4d2f20, (void *)h_engreg); jmp_patch(0x4f9110, (void *)h_georeg); jmp_patch(0x500b40, (void *)h_secreg);
  jmp_patch(0x4f56f0, (void *)h_getworld); jmp_patch(0x5021e0, (void *)h_tristrip);
  for (i = 0; i < 8; i++) vtbl[i] = 0xbad00000u + i;
  vtbl[4] = P(h_getdecl);
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
