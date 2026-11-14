/* rw_d3d9vsgen_check.c: bit-exact check of the librw DLL's d3d9vsgen export (group d3d9vsgen, librw-mfl capi/d3d9vsgen.cpp) against the ORIGINAL code of MFL.exe,
   both run natively (Wine).
   usage: rw_d3d9vsgen_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [narrow] [va:byte ...]   (build / run: tools/delink/rw_d3d9vsgen_check.sh)
   Row: rpD3D9GenerateVertexShader 0x509870 (desc, flags).  It writes vertex shader assembly text into the exe buffer 0x7d68e0 through the engine's sprintf
   (RwEngineInstance + 0xc4), assembles it (D3DXAssembleShader 0x50cf85) and creates the shader (RwD3D9CreateVertexShader 0x4dbc10).  Stand-ins (same both sides):
   the sprintf (real vsprintf; logs dst offset, length, a hash of the format text and of the produced line), D3DXAssembleShader (logs all 7 arguments and the
   whole source text; fails / fails with a buffer / succeeds per a side-local stream), the ID3DXBuffer methods and RwD3D9CreateVertexShader (logs the code pointer,
   writes a handle or nothing).  Inputs per call: the 4 descriptor bytes (half realistic counts, half the full byte space), D3D9UseVersion (0xa / 0x14 / 0x19,
   sometimes another value), the low byte of _rwD3D9VertexShaderMaxConstants, D3D9NumConstantsEffect (64 dwords, seeded per seed, re-drawn per call), the WFOG
   raster cap bit.  Compared: the call log, the 8 flag bytes, the return and a hash per 1 KB of 0x7d68e0..0x7da8e0 (the text overflows the 4 KB buffer for big
   light counts), reset per call.  Each call runs at x87 control word 0x7f, 0x27f or 0x37f (integer code; exercised anyway).  "rev" = original first.  "narrow" =
   realistic descriptors only (first EQUAL).  va:byte patches the ORIGINAL (must DIFF).  Exit 1 on DIFF / MISSING / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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

#define SRCBUF 0x7d68e0u
#define REGION 0x4000u
#define NUMEFFECT 0x7d78e0u
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
static u32 fnv(const void *p, u32 n) { const u8 *b = p; u32 h = 2166136261u; while (n--) h = (h ^ *b++) * 16777619u; return h; }

/* ---- stand-ins (same both sides) ---- */
static int __cdecl h_sprintf(char *dst, const char *fmt, ...) {
  va_list ap; int n;
  va_start(ap, fmt); n = vsprintf(dst, fmt, ap); va_end(ap);
  lg(0x5f000000u); lg((u32)(uintptr_t)dst - SRCBUF); lg((u32)n); lg(fnv(fmt, strlen(fmt))); lg(fnv(dst, n > 0 ? (u32)n : 0));
  return n;
}
static u32 tokbuf[64];
static u32 __stdcall h_release(void *self) { lg(0xa0000008u); lg((u32)(uintptr_t)self); return 0; }
static void *__stdcall h_getptr(void *self) { lg(0xa000000cu); lg((u32)(uintptr_t)self); return tokbuf; }
static void *const bufvt[4] = {0, 0, (void *)h_release, (void *)h_getptr};
static struct { void *const *vt; } fakebuf = {bufvt};
static long nassemble[3], ncreate;
static int __stdcall h_assemble(const char *src, u32 len, const void *defines, void *include, u32 flags, void **shader, void **errors) {
  u32 m = hrnd() % 8, i;
  lg(0xa50cf85u); lg((u32)(uintptr_t)src); lg(len); lg((u32)(uintptr_t)defines); lg((u32)(uintptr_t)include); lg(flags); lg(shader != 0); lg((u32)(uintptr_t)errors);
  if (len < REGION) for (i = 0; i < len; i += 4) lg(*(const u32 *)(src + i));      /* the source text itself, word by word */
  lg(shader ? (u32)(uintptr_t)*shader : 0xdead);
  if (m == 0) { nassemble[0]++; return (int)0x8000ffffu; }                       /* fail, no buffer */
  if (m == 1) { nassemble[1]++; *shader = &fakebuf; return (int)0x80004005u; }    /* fail with a buffer (the original still releases it) */
  nassemble[2]++; *shader = &fakebuf; return 0;
}
static void __cdecl h_create(const void *code, u32 *shader) {
  lg(0xa4dbc10u); lg((u32)(uintptr_t)code); lg(*shader); ncreate++;
  if (hrnd() % 6) *shader = 0x5ade0000u + (hrnd() & 0xffff);                      /* else: leave the handle untouched */
}

static void *fn[2];
static int narrow;
static u32 numeff[0x40];
static u8 descbuf[4], flagbuf[8];
static void reset(u32 seed) {
  u32 k;
  nlg = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; *(void **)(eng + 0xc4) = (void *)h_sprintf;
  for (k = 0; k < 0x40; k++) { u32 h = (seed * 0x9e3779b1u) ^ (k * 0x85ebca6bu); h ^= h >> 15; numeff[k] = h % 3 == 0 ? 0 : h % 9; }
  for (k = 0; k < 0x100; k++) W(CAPS + 4 * k) = 0;
}
static u8 realistic(int which) {
  switch (which) {
  case 0: { u32 a = rnd() % 8, b = rnd() % 8; return (u8)((a < 4 ? a : 0) | (b < 3 ? b : 0) << 4); }     /* dir 0..3, point 0..2 */
  case 1: { u32 a = rnd() % 8, b = rnd() % 5; return (u8)((a < 3 ? a : 0) | b << 4); }                  /* spot 0..2, texcoords 0..4 */
  case 2: { u32 v = rnd() & 0xf1, w = rnd() % 8; return (u8)(v | (w < 5 ? w : 0) << 1); }              /* skin weights 0..4 */
  default: { u32 e = rnd() % 16; return (u8)((rnd() & 3) | (e < 12 ? e : rnd() % 64) << 2); }           /* effects 0..11, sometimes 12..63 */
  }
}
static void run(int side, u32 seed, int steps) {
  int s, k;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 r2 = rnd(), ret, uv;
    lg(0x57e90000u + s);
    if (narrow || r2 % 2) for (k = 0; k < 4; k++) descbuf[k] = realistic(k);
    else { u32 v = rnd(); memcpy(descbuf, &v, 4); }
    { u32 v = rnd() % 16; static const u32 vs[3] = {0xa, 0x14, 0x19}; uv = v < 13 ? vs[v % 3] : v == 13 ? 0 : v == 14 ? 0x1e : rnd(); }
    if (narrow) uv = (r2 >> 8) % 3 == 0 ? 0xa : (r2 >> 8) % 3 == 1 ? 0x14 : 0x19;
    for (k = 0; k < (int)REGION; k += 4) W(SRCBUF + k) = (seed * 0x01000193u) ^ (u32)k;    /* text buffer + everything it can overflow into (incl. the globals) */
    W(USEVER) = uv;
    W(MAXCONST) = rnd() % 4 == 0 ? rnd() : 0x60 + rnd() % 0xa1;
    W(CAPS + 0x24) = (rnd() & ~0x00100000u) | (rnd() % 2 ? 0x00100000u : 0);       /* RasterCaps: every bit drawn (mutation round 1: was bits 20/21 only) */
    if (rnd() % 4 == 0) numeff[rnd() % 0x40] = rnd() % 4 == 0 ? rnd() : rnd() % 12;
    for (k = 0; k < 0x40; k++) W(NUMEFFECT + 4 * k) = numeff[k];
    { u32 a = rnd(), b = rnd(); memcpy(flagbuf, &a, 4); memcpy(flagbuf + 4, &b, 4); }
    lg(descbuf[0] | descbuf[1] << 8 | descbuf[2] << 16 | (u32)descbuf[3] << 24); lg(uv); lg(W(MAXCONST)); lg(W(CAPS + 0x24));
    setcw(pickcw());
    ret = ((u32 (__cdecl *)(const u8 *, u8 *))fn[side])(descbuf, flagbuf);
    lg(ret); lg(*(u32 *)flagbuf); lg(*(u32 *)(flagbuf + 4));
    for (k = 0; k < (int)REGION; k += 0x400) lg(fnv((const void *)(uintptr_t)(SRCBUF + k), 0x400));
  }
}
static u32 *save_log, nsave;
static int quiet;
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if ((i < n || nlg != nsave) && quiet) return 1;
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, desc = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; desc = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u desc %08x): first %08x second %08x (lengths %u / %u)\n", seed, i, st, desc, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 12 ? i - 12 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  return 0;
}
static volatile u32 cur_seed, deadline; static char cur_mut[256];
/* a verdict line in one write (stdout is unbuffered: printf writes in pieces) */
static void mutsay(const char *verdict) { char b[400]; int n = snprintf(b, sizeof b, "\nMUT %s: %s\n", cur_mut, verdict); fwrite(b, 1, n, stdout); }
static DWORD WINAPI watchdog(LPVOID p) {
  if (!deadline) deadline = GetTickCount() + (DWORD)(uintptr_t)p;
  while ((int32_t)(deadline - GetTickCount()) > 0) Sleep(100);
  if (cur_mut[0]) mutsay("TIMEOUT");
  printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  if (cur_mut[0]) { char v[64]; snprintf(v, sizeof v, "CRASH %08lx at %p", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress); mutsay(v); }
  printf("CRASH %08lx at %p seed %u\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }
static double tot;
/* seeds 1..n, both sides, compared; stops after maxbad differing seeds; returns their number */
static int run_seeds(int seeds, int steps, int maxbad) {
  u32 seed; int bad = 0;
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= maxbad) break;
  }
  return bad;
}
/* mutation list mode (mut@<file>, lines `va:byte [va:byte ...]  # comment`; driver agents/k1_w6/mutbatch.py): per line the original is patched, the seeds run until
   the first difference, the bytes are restored and one line `MUT <spec>: DIFF|EQUAL` is printed (CRASH / TIMEOUT lines come from the handlers and end the process:
   the driver starts again behind that line).  After every mutation 3 unpatched seeds must be EQUAL, else MUTSTOP. */
static int run_mutlist(const char *path, int seeds, int steps) {
  FILE *f = fopen(path, "r"); char line[512];
  if (!f) { printf("cannot open mutation list %s\n", path); return 2; }
  quiet = 1;
  while (fgets(line, sizeof line, f)) {
    u32 va[8], by[8], old[8], n = 0, k; char *p = strchr(line, '#'), *q; int bad;
    if (p) *p = 0;
    for (q = strtok(line, " \t\r\n"); q && n < 8; q = strtok(0, " \t\r\n")) if (sscanf(q, "%x:%x", &va[n], &by[n]) == 2) n++;
    if (!n) continue;
    cur_mut[0] = 0; for (k = 0; k < n; k++) sprintf(cur_mut + strlen(cur_mut), "%s%x:%02x", k ? " " : "", va[k], by[k]);
    for (k = 0; k < n; k++) { old[k] = *(u8 *)(uintptr_t)va[k]; *(u8 *)(uintptr_t)va[k] = (u8)by[k]; }
    deadline = GetTickCount() + 120000;
    bad = run_seeds(seeds, steps, 1);
    for (k = 0; k < n; k++) *(u8 *)(uintptr_t)va[k] = (u8)old[k];
    mutsay(bad ? "DIFF" : "EQUAL");
    deadline = GetTickCount() + 120000;
    if (run_seeds(3, steps, 1)) { printf("MUTSTOP: unpatched seeds differ after %s\n", cur_mut); return 7; }
    cur_mut[0] = 0;
  }
  printf("MUTDONE\n");
  return 0;
}

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; const char *mutlist = 0;
  void (__cdecl *bind)(void *); static void *tbl[16];
  /* table order = d3d9vsgen.cpp enum V */
  static const u32 t[] = {ENGVAR, SRCBUF, NUMEFFECT, USEVER, MAXCONST, CAPS, 0x50cf85, 0x4dbc10};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 3000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (!strcmp(argv[i], "narrow")) { narrow = 1; printf("descriptors: realistic only\n"); }
    else if (!strncmp(argv[i], "mut@", 4)) mutlist = argv[i] + 4;
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9vsgen_bind");
  if (!bind) { printf("mflrw_d3d9vsgen_bind MISSING export\n"); return 1; }
  jmp_patch(0x50cf85, (void *)h_assemble); jmp_patch(0x4dbc10, (void *)h_create);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind(tbl);
  fn[rev] = (void *)GetProcAddress(m, "rw37_rpD3D9GenerateVertexShader"); fn[!rev] = (void *)(uintptr_t)0x509870;
  if (!fn[rev]) { printf("rw37_rpD3D9GenerateVertexShader MISSING export\n"); return 1; }
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  if (mutlist) return run_mutlist(mutlist, seeds, steps);
  bad = run_seeds(seeds, steps, 5);
  printf("assemble fail / fail+buffer / ok: %ld / %ld / %ld, create %ld (both sides)\n", nassemble[0], nassemble[1], nassemble[2], ncreate);
  { char line[96]; sprintf(line, "%.0f log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL"); fputs(line, stdout); }      /* one write: the DLL's exit line must not split it */
  printf("DONE\n");
  return bad != 0;
}
