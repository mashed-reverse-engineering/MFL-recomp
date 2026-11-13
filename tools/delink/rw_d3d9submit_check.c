/* rw_d3d9submit_check.c: bit-exact check of the librw DLL's d3d9submit exports (group d3d9submit, librw-mfl capi/d3d9submit.cpp) against the ORIGINAL code of MFL.exe,
   both run natively (Wine).
   usage: rw_d3d9submit_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [row:<n>] [va:byte ...] [mut@<file>]   (build / run: tools/delink/rw_d3d9submit_check.sh)
   Rows: see F[] below.  Device = tools/delink/fakedev.c (87 SetVertexDeclaration, 92 SetVertexShader, 104 SetIndices, 107 SetPixelShader, index buffer objects with
   Release / Lock / Unlock; every other slot = TRAP + exit) plus the slots of this file: 27 CreateIndexBuffer (may fail and leave the old pointer), 81 DrawPrimitive and
   82 DrawIndexedPrimitive (HRESULT drawn per call: fail 1/4, S_FALSE), 83 DrawPrimitiveUP, 84 DrawIndexedPrimitiveUP.  Every exe callee of the rows is a logging
   stand-in (vertex alpha, world transform, render state, raster stage, texture stage state, dynamic vertex buffer lock / unlock, stream source, cache flush,
   declaration create / delete).  The Im3D globals 0x7d522c..0x7d524b, the five "last used" caches 0x62e760..0x62e773, the device pointer and DevCaps are preset
   per step and logged after every call; the vertex memory the rows fill is arena memory (compared).  "rev" runs the original first; row:<n> restricts the steps to
   row n of F[].  va:byte patches the ORIGINAL's code first (mutation test: must give DIFF); mut@<file> runs a list of mutations in one process (driver:
   agents/k1_w6/mutbatch.py).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "fakedev.c"
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
#define B(va) (*(volatile u8 *)(uintptr_t)(va))
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

#define DEVVAR 0x7d2240u
#define CAPS 0x947600u
#define IM3D 0x7d522cu      /* stash, base index, current vertex buffer, index buffer offset, index buffer, declarations old / Im3D / no texture */
#define LAST 0x62e760u      /* last FVF, declaration, vertex shader, pixel shader, index buffer */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 23)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))
static void *dev;

/* device slots of this check */
static int ib_keep;      /* a failing CreateIndexBuffer leaves the pointer alone (else it writes 0) */
static long __stdcall d_createib(void *self, u32 len, u32 usage, u32 fmt, u32 pool, void **pp, void *share) {
  lg(FAKEDEV_TAG | 27); lg(P(share)); lg(P(pp));
  if (fakedev_createhr != 0 && ib_keep) { lg(len); lg(usage); lg(fmt); lg(pool); lg((u32)fakedev_createhr); return fakedev_createhr; }
  return fd_create(len, usage, fmt, pool, pp);
}
static long drawhr(void) { u32 r = hrnd() % 8; return r < 2 ? (long)0x8876086c : r == 2 ? 1 : 0; }
static long __stdcall d_draw(void *self, u32 type, u32 start, u32 count) { long h = drawhr(); lg(FAKEDEV_TAG | 81); lg(type); lg(start); lg(count); lg((u32)h); return h; }
static long __stdcall d_drawidx(void *self, u32 type, u32 base, u32 minIdx, u32 nv, u32 start, u32 count) {
  long h = drawhr(); lg(FAKEDEV_TAG | 82); lg(type); lg(base); lg(minIdx); lg(nv); lg(start); lg(count); lg((u32)h); return h;
}
static long __stdcall d_drawup(void *self, u32 type, u32 count, u32 verts, u32 stride) { lg(FAKEDEV_TAG | 83); lg(type); lg(count); lg(verts); lg(stride); return (long)hrnd(); }
static long __stdcall d_drawidxup(void *self, u32 type, u32 minIdx, u32 nv, u32 count, u32 idx, u32 fmt, u32 verts, u32 stride) {
  lg(FAKEDEV_TAG | 84); lg(type); lg(minIdx); lg(nv); lg(count); lg(idx); lg(fmt); lg(verts); lg(stride); return (long)hrnd();
}

/* stand-ins */
static void __cdecl h_valpha(u32 on) { lg(0xa4e5860); lg(on); }
static void __cdecl h_xformworld(u32 m) { lg(0xa4db600); lg(m); }
static void __cdecl h_setrs(u32 s, u32 v) { lg(0xa4e3c00); lg(s); lg(v); }
static u32 __cdecl h_rasterstage(u32 stage) { u32 r = hrnd() % 2 ? 0x7a57e000u + hrnd() % 16 : 0; lg(0xa4e55c0); lg(stage); lg(r); return r; }
static void __cdecl h_settss(u32 a, u32 b, u32 c) { lg(0xa4e3c70); lg(a); lg(b); lg(c); }
static u32 __cdecl h_dynlock(u32 stride, u32 n, u32 *pvb, u32 *pptr, u32 *pbase) {
  u32 r = hrnd() % 6 != 0; lg(0xa4eccd0); lg(stride); lg(n); lg(P(pvb)); lg(P(pbase)); lg(r);
  if (r) { *pvb = 0xdb0f0000u + hrnd() % 256; *pptr = P(amalloc(stride * n + 64)); *pbase = hrnd() % 1000; lg(*pvb); lg(*pptr); lg(*pbase); }
  return r;
}
static void __cdecl h_dynunlock(u32 vb) { lg(0xa4ece50); lg(vb); }
static void __cdecl h_setstream(u32 s, u32 vb, u32 off, u32 stride) { lg(0xa4db270); lg(s); lg(vb); lg(off); lg(stride); }
static void __cdecl h_flush(void) { lg(0xa4e3b30); }
static u32 ndecl;
static u32 __cdecl h_createdecl(u8 *el, u32 *pout) {
  u32 n = 0, r = hrnd() % 8 != 0;
  lg(0xa4db9e0); lg(P(pout));
  for (;;) { lg(*(u32 *)(el + 8 * n)); lg(*(u32 *)(el + 8 * n + 4)); n++; if (*(u16 *)(el + 8 * n - 8) == 0xff || n == 8) break; }
  *pout = 0xdec10000u + ndecl++; lg(r); return r;
}
static void __cdecl h_deletedecl(u32 d) { lg(0xa4dbbc0); lg(d); }

enum { IMM, CLOSE, OPEN, SUBMIT, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_D3D9ImmInstanceNode", 0x4f1390}, {"rw37_rwD3D9Im3DRenderClose", 0x4f09d0}, {"rw37_rwD3D9Im3DRenderOpen", 0x4f0a40},
  {"rw37_rxD3D9SubmitNode", 0x4f0c90}};
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G2)(u32, u32);
static void *fn[2][NF]; static long ncalls[NF]; static int only = -1;

static void reset(u32 seed) {
  u32 k;
  nlg = 0; ap = 0; ndecl = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  fakedev_reset_bufs(); fakedev_hr = 0;
  W(DEVVAR) = P(dev); W(CAPS + 0x1c) = 0; for (k = 0; k < 8; k++) W(IM3D + 4 * k) = 0; for (k = 0; k < 5; k++) W(LAST + 4 * k) = 0;
}
static u32 mkib(void) { void *b = 0; fakedev_createhr = 0; fd_create(0x1fffe, 0x208, 0x65, 0, &b); return P(b); }
static void logglobals(void) { u32 k; for (k = 0; k < 8; k++) lg(W(IM3D + 4 * k)); for (k = 0; k < 5; k++) lg(W(LAST + 4 * k)); }
/* "last used" index buffer: 0, the current one, another one, or a small value (never dereferenced; a mutated read of the word one byte up must show) */
static u32 some(u32 a) { u32 c = rnd() % 8; return c < 3 ? 0 : c < 6 ? a : c == 6 ? 0xdec1f000u + rnd() % 8 : 1 + rnd() % 255; }
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = only >= 0 ? (u32)only : rnd() % 8, k, r = 0;
    if (op > SUBMIT) op = SUBMIT;
    lg(0x57e90000u + s); lg(op);
    fakedev_createhr = 0; fakedev_lockhr = 0;
    if (op == IMM) {
      u32 *params = (u32 *)rblock(8);
      r = ((G2)f[IMM])(rnd(), P(params)); ncalls[IMM]++; lg(r);
    } else if (op == CLOSE || op == OPEN) {
      W(IM3D + 0x10) = rnd() % 3 ? mkib() : 0; W(IM3D + 0xc) = rnd();
      for (k = 0; k < 3; k++) W(IM3D + 0x14 + 4 * k) = rnd() % 3 ? 0xdec1e000u + rnd() % 16 : 0;
      if (op == CLOSE) { ((G0)f[CLOSE])(); ncalls[CLOSE]++; }      /* void: eax is whatever the last callee left */
      else {
        W(CAPS + 0x1c) = rnd(); fakedev_createhr = rnd() % 3 ? 0 : rnd() % 2 ? (int32_t)0x8876086c : 1; ib_keep = rnd() % 2;
        r = ((G0)f[OPEN])(); ncalls[OPEN]++; lg(r); fakedev_createhr = 0;
      }
    } else {      /* rxD3D9SubmitNode */
      static const u32 nvs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 24, 40};
      u32 nv = rnd() % 10 == 0 ? 0 : rnd() % 12 == 0 ? 100 + rnd() % 200 : nvs[rnd() % 12], ni, ib = rnd() % 6 ? mkib() : 0, d1 = 0xdec1e001u, d2 = 0xdec1e002u, d0 = 0xdec1e000u;
      u8 *stash = rblock(0x40), *verts = rblock((nv ? nv : 1) * 0x24); u16 *idx = 0;
      switch (rnd() % 8) {
      case 0: ni = 0; break;
      case 1: ni = 0x10000 + rnd() % 100; break;      /* too many for the index buffer: never copied */
      case 2: ni = rnd() % 40 == 0 ? 0xffff : 9 + rnd() % 60; break;
      default: ni = 1 + rnd() % 9; break;      /* around the 8 index limit, below and above the vertex count */
      }
      if (rnd() % 4) { u32 na = ni > 0xffff ? 16 : ni ? ni : 4; idx = (u16 *)amalloc(na * 2); for (k = 0; k < na; k++) idx[k] = (u16)(nv ? rnd() % nv : 0); }
      *(u16 *)stash = (u16)nv; *(u32 *)(stash + 4) = P(verts); *(u32 *)(stash + 0x30) = 1 + rnd() % 5 /* 0 (and 6) give an unset primitive count in the original */;
      *(u32 *)(stash + 0x34) = P(idx); *(u32 *)(stash + 0x38) = ni;
      if (rnd() % 16 == 0) d1 = d2;
      if (rnd() % 8 == 0) { if (rnd() % 2) d1 = d0; else d2 = d0; }      /* the "old" declaration already set when the fallback path runs */
      W(IM3D) = P(stash); W(IM3D + 4) = rnd(); W(IM3D + 8) = rnd(); W(IM3D + 0x10) = ib; W(IM3D + 0x14) = d0; W(IM3D + 0x18) = d1; W(IM3D + 0x1c) = d2;
      { static const u32 o[] = {0, 1, 5, 0xfff0, 0xffff, 0x10000}; u32 c = rnd() % 4;
        W(IM3D + 0xc) = c == 0 ? o[rnd() % 6] : c == 1 ? 0xffff - ni : c == 2 ? 0x10000 - ni : rnd() % 0xff00; }
      W(LAST) = rnd(); W(LAST + 4) = rnd() % 4 == 0 ? d0 : rnd() % 3 == 0 ? d1 : rnd() % 2 ? d2 : rnd(); W(LAST + 8) = rnd() % 2 ? 0 : rnd(); W(LAST + 0xc) = rnd() % 2 ? 0 : rnd();
      W(LAST + 0x10) = some(ib);
      fakedev_lockhr = rnd() % 4 ? 0 : rnd() % 2 ? 1 : (int32_t)0x8876086c;
      r = ((G0)f[SUBMIT])(); ncalls[SUBMIT]++; lg(r); fakedev_lockhr = 0;
    }
    logglobals();
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena;
static int quiet;      /* mutation list mode: only the verdict lines */
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  if (quiet) return nlg != nsave || memcmp(save_log, LG, nlg * 4) || memcmp(save_arena, arena, ARENA);
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 30 ? i - 30 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  if (memcmp(save_arena, arena, ARENA)) { for (i = 0; i < ARENA && save_arena[i] == arena[i]; i++); printf("  seed %u: arena differs at +0x%x\n", seed, i); return 1; }
  return 0;
}
static volatile u32 cur_seed, deadline; static char cur_mut[256];
/* a verdict line in one write (stdout is unbuffered: printf writes in pieces, and the DLL's exit message can land inside a line) */
static void mutsay(const char *verdict) { char b[400]; int n = snprintf(b, sizeof b, "\nMUT %s: %s\n", cur_mut, verdict); fwrite(b, 1, n, stdout); }
static DWORD WINAPI watchdog(LPVOID p) {
  if (!deadline) deadline = GetTickCount() + (DWORD)(uintptr_t)p;
  while ((int32_t)(deadline - GetTickCount()) > 0) Sleep(100);
  if (cur_mut[0]) mutsay("TIMEOUT");
  printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  if (cur_mut[0]) { char v[64]; snprintf(v, sizeof v, "CRASH %08lx at %p", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress); mutsay(v); }
  printf("CRASH %08lx at %p seed %u; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  fflush(stdout); ExitProcess(4); return 0; }
static long tot;
static int run_seeds(int seeds, int steps, int maxbad) {
  u32 seed; int bad = 0;
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= maxbad) break;
  }
  return bad;
}
/* mutation list mode: see rw_noded3d9i_check.c */
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
    for (k = 0; k < n; k++) { old[k] = B(va[k]); B(va[k]) = (u8)by[k]; }
    deadline = GetTickCount() + 120000;
    bad = run_seeds(seeds, steps, 1);
    for (k = 0; k < n; k++) B(va[k]) = (u8)old[k];
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
  void (__cdecl *bind)(void *); static void *tbl[27];
  /* table order = enum E of d3d9submit.cpp */
  static const u32 t[27] = {DEVVAR, CAPS, IM3D, IM3D + 4, IM3D + 8, IM3D + 0xc, IM3D + 0x10, IM3D + 0x14, IM3D + 0x18, IM3D + 0x1c,
                            LAST, LAST + 4, LAST + 8, LAST + 0xc, LAST + 0x10, 0x5ea2a8,
                            0x4e5860, 0x4db600, 0x4e3c00, 0x4e55c0, 0x4e3c70, 0x4eccd0, 0x4ece50, 0x4db270, 0x4e3b30, 0x4db9e0, 0x4dbbc0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  fakedev_log = lg; dev = fakedev_init();
  fakedev_vtbl[27] = (void *)d_createib; fakedev_vtbl[81] = (void *)d_draw; fakedev_vtbl[82] = (void *)d_drawidx; fakedev_vtbl[83] = (void *)d_drawup; fakedev_vtbl[84] = (void *)d_drawidxup;
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (!strncmp(argv[i], "row:", 4)) { only = atoi(argv[i] + 4); printf("rows: %s only\n", F[only].dll + 5); }
    else if (!strncmp(argv[i], "mut@", 4)) mutlist = argv[i] + 4;
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9submit_bind");
  if (!bind) { printf("mflrw_d3d9submit_bind MISSING export\n"); return 1; }
  jmp_patch(0x4e5860, (void *)h_valpha); jmp_patch(0x4db600, (void *)h_xformworld); jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4e55c0, (void *)h_rasterstage);
  jmp_patch(0x4e3c70, (void *)h_settss); jmp_patch(0x4eccd0, (void *)h_dynlock); jmp_patch(0x4ece50, (void *)h_dynunlock); jmp_patch(0x4db270, (void *)h_setstream);
  jmp_patch(0x4e3b30, (void *)h_flush); jmp_patch(0x4db9e0, (void *)h_createdecl); jmp_patch(0x4dbbc0, (void *)h_deletedecl);
  for (i = 0; i < 27; i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind(tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  if (mutlist) return run_mutlist(mutlist, seeds, steps);
  bad = run_seeds(seeds, steps, 5);
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  { char b[96]; int n = snprintf(b, sizeof b, "%ld log words compared; %s\nDONE\n", tot, bad ? "DIFF" : "EQUAL"); fwrite(b, 1, n, stdout); }      /* one write */
  return bad != 0;
}
