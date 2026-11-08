/* rw_d3drs_check.c: behavioural check of the librw DLL's d3drs exports (tools/delink/rw_d3drs.txt: the 22 functions of d3d9rendst.obj, librw-mfl
   capi/d3d9rs.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).  usage: rw_d3drs_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000]
   [va:byte ...]  (build/run: tools/delink/rw_d3drs_check.sh; image mapping as rw_g3_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c (6 recorders; any other method = TRAP, the run fails).
   The DLL is bound (mflrw_d3d_bind) to the same exe state: the rendst bss 0x7d2810..0x7d4d1c, the caps 0x947600, the raster extension offset 0x947144,
   the ambient colour 0x62e738, 1/255 0x5dff00, the rdata tables 0x5ea148, and the 4 exe C callees (original bytes on both sides).
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through
   the original addresses: Open / Reset first, then the 22 entries mixed with the 7 rendst functions that are C in the exe (RwD3D9SetRenderState /
   GetRenderState / SetSamplerState, SrcBlend, DestBlend, GetRasterStage, IsVertexAlphaEnable; original bytes on both sides), with Reset / Open /
   FlushCache between.  Logged after every op: the fake device's calls, the return value, the out word, the control word and a hash of the whole rendst
   bss block; at the end of a seed the block itself.  Extra arguments va:byte patch the ORIGINAL's code first (negative fixtures: must give DIFF).
   Exit 1 on DIFF. */
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
#define BSS0 0x7d2810u
#define BSS1 0x7d4d1cu
#define CAPS 0x947600u
#define RASEXT 0x947144u
#define AMB 0x62e738u
#define ANISO 0x7d4d00u
#define RASOFF 0x40      /* raster extension offset used by the harness */

/* log */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 bss_hash(void) { u32 h = 0x811c9dc5u, a; for (a = BSS0; a < BSS1; a += 4) h = (h ^ W(a)) * 0x01000193u; return h; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }

/* entries: the 22 under test ([0] DLL export, [1] original) and the 7 C functions (original on both sides) */
enum { FLUSH, TSS, SURF, OPEN, RESET, GETRS, TFILT, RSTAGE, STEX, VA, ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7, SETRS, ZTEST, SHADE, CULL, NF,
       C_SETRS = NF, C_GETRS, C_SETSS, C_SRC, C_DST, C_GETSTAGE, C_ISVA, NALL };
static const struct { const char *dll; u32 va; } F[NALL] = {
  {"rw37__rwD3D9RenderStateFlushCache", 0x4e3b30}, {"rw37_RwD3D9SetTextureStageState", 0x4e3c70}, {"rw37_RwD3D9SetSurfaceProperties", 0x4e3d10},
  {"rw37__rwD3D9RenderStateOpen", 0x4e4320}, {"rw37__rwD3D9RenderStateReset", 0x4e4960}, {"rw37__rwD3D9RWGetRenderState", 0x4e5070},
  {"rw37__rwD3D9RenderStateTextureFilter", 0x4e52f0}, {"rw37__rwD3D9RWSetRasterStage", 0x4e5440}, {"rw37_RwD3D9SetTexture", 0x4e55d0},
  {"rw37__rwD3D9RenderStateVertexAlphaEnable", 0x4e5860}, {"rw37__rwD3D9RenderStateStencilEnable", 0x4e5960}, {"rw37__rwD3D9RenderStateStencilFail", 0x4e59b0},
  {"rw37__rwD3D9RenderStateStencilZFail", 0x4e5a00}, {"rw37__rwD3D9RenderStateStencilPass", 0x4e5a50}, {"rw37__rwD3D9RenderStateStencilFunction", 0x4e5aa0},
  {"rw37__rwD3D9RenderStateStencilFunctionRef", 0x4e5af0}, {"rw37__rwD3D9RenderStateStencilFunctionMask", 0x4e5b40},
  {"rw37__rwD3D9RenderStateStencilFunctionWriteMask", 0x4e5b90}, {"rw37__rwD3D9RWSetRenderState", 0x4e5be0}, {"rw37_rwD3D9RenderStateZTestEnable", 0x4e6220},
  {"rw37__rwD3D9RenderStateShadeMode", 0x4e6320}, {"rw37__rwD3D9RenderStateCullMode", 0x4e6370},
  {"RwD3D9SetRenderState", 0x4e3c00}, {"RwD3D9GetRenderState", 0x4e3c50}, {"RwD3D9SetSamplerState", 0x4e3cd0}, {"rwD3D9RenderStateSrcBlend", 0x4e53a0},
  {"rwD3D9RenderStateDestBlend", 0x4e53f0}, {"rwD3D9RWGetRasterStage", 0x4e55c0}, {"rwD3D9RenderStateIsVertexAlphaEnable", 0x4e5950}};
typedef u32 (__cdecl *G3)(u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL];

static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
/* objects outside the exe state (not written by the functions under test) */
static u8 *rasters, *pals, *texs; static float *fogv;
#define NRAS 12
#define NTEX 16
static u32 ras(int i) { return i < 0 ? 0 : (u32)(uintptr_t)(rasters + 0x60 * i); }
static u32 pickras(void) { return rnd() % 5 == 0 ? 0 : ras(rnd() % NRAS); }

static u32 rsval(u32 st) {      /* a value for rwD3D9RWSetRenderState: in range for the table-indexed states, small sets for repeats */
  static const u32 flt[] = { 0x3f800000, 0x00000000, 0x80000000, 0x3c23d70a, 0x7fc00000, 0x7f800001, 0x00000001, 0x3f800001 };
  switch (st) {
  case 1: return pickras();
  case 2: case 3: case 4: return rnd() % 5;
  case 7: return rnd() % 3;
  case 9: return rnd() % 8 ? rnd() % 7 : rnd() % 14;
  case 10: case 11: return rnd() % 12;
  case 16: case 20: return rnd() % 4;
  case 17: { float *p = fogv + rnd() % 64; if (rnd() % 3 == 0) *(u32 *)p = flt[rnd() % 8]; else if (rnd() % 2) *p = (float)(rnd() % 100) / 37.0f; return (u32)(uintptr_t)p; }
  case 22: case 23: case 24: case 25: case 29: return rnd() % 9;
  case 13: case 15: case 26: case 27: case 28: return rnd() % 3 ? rnd() % 4 : rnd();
  default: return rnd() % 3 ? rnd() % 2 : rnd();      /* booleans, perspective, alpha ref, out of range */
  }
}

static void reset(u32 seed, unsigned short cw) {
  u32 a; int i;
  nlg = 0; rs = seed * 2654435761u + cw;
  for (a = BSS0; a < BSS1; a += 4) W(a) = 0;
  W(ANISO) = rnd() % 3 == 0 ? 0 : rnd() % 4 == 0 ? 0xfffffff0u : 0x70;      /* texture anisotropy plugin offset: none / negative / 0x70 */
  W(RASEXT) = RASOFF;
  memset((void *)CAPS, 0, 0x130);
  W(CAPS + 0x24) = (rnd() % 2 ? 0x100 : 0) | (rnd() % 2 ? 0x100000 : 0) | (rnd() % 2 ? 0x80 : 0) | (rnd() % 2);      /* RasterCaps: fog table / W fog / fog vertex / dither */
  W(CAPS + 0x40) = (rnd() % 2 ? 0x200 : 0) | (rnd() % 2 ? 0x4000000 : 0) | (rnd() & 0x1010100);      /* TextureFilterCaps: MINFLINEAR, MAGFANISOTROPIC */
  for (i = 0; i < 3; i++) { float f = rnd() % 4 == 0 ? 1.0f : (float)(rnd() % 1000) / 999.0f; memcpy((void *)(uintptr_t)(AMB + 4 * i), &f, 4); }
  for (i = 0; i < NRAS; i++) {
    u8 *r = rasters + 0x60 * i; memset(r, 0x5a + i, 0x60);
    *(u32 *)(r + RASOFF) = 0xd3d70000u + i;      /* the D3D9 texture */
    *(u32 *)(r + RASOFF + 4) = i % 3 == 0 ? (u32)(uintptr_t)(pals + 0x404 * (i / 3)) : 0;
    r[RASOFF + 8] = (u8)(i % 2 ? 0 : 1 + i);      /* alpha */
  }
  for (i = 0; i < 4; i++) *(u32 *)(pals + 0x404 * i + 0x400) = 0x9a10 + i;
  for (i = 0; i < NTEX; i++) {
    u8 *t = texs + 0x80 * i; memset(t, 0, 0x80);
    *(u32 *)t = pickras();
    *(u32 *)(t + 0x50) = (rnd() % 7) | (rnd() % 5) << 8 | (rnd() % 5) << 12 | (rnd() & 0xffff0000u);
    t[0x70] = (u8)(rnd() % 3 == 0 ? rnd() : rnd() % 3);      /* anisotropy */
  }
  for (i = 0; i < 64; i++) fogv[i] = (float)(rnd() % 8) / 4.0f;
  fakedev_hr = 0;
}

static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, a2 = 0, r, out = 0xcccccccc; int k; u32 *pout = 0; u8 *t;
    float sp[3]; u8 col[4];
    if (s == 0) k = rnd() % 8 ? OPEN : RESET;
    else {
      u32 q = rnd() % 100;
      k = q < 30 ? SETRS : q < 36 ? GETRS : q < 44 ? STEX : q < 48 ? RSTAGE : q < 54 ? SURF : q < 58 ? TSS : q < 60 ? FLUSH : q < 61 ? (rnd() % 2 ? OPEN : RESET)
        : q < 63 ? TFILT : q < 65 ? VA : q < 69 ? ST0 + (int)(rnd() % 8) : q < 71 ? ZTEST : q < 72 ? SHADE : q < 73 ? CULL
        : q < 82 ? C_SETRS : q < 84 ? C_GETRS : q < 90 ? C_SETSS : q < 92 ? C_SRC : q < 94 ? C_DST : q < 96 ? C_GETSTAGE : q < 97 ? C_ISVA : FLUSH;
      if (q < 6 && rnd() % 4 == 0) k = FLUSH;
    }
    fakedev_hr = rnd() % 10 == 0 ? (int32_t)0x8876086c : rnd() % 20 == 0 ? 1 : 0;
    if (rnd() % 50 == 0) { int i = rnd() % 3; float v = (float)(rnd() % 1000) / 999.0f; memcpy((void *)(uintptr_t)(AMB + 4 * i), &v, 4); }      /* ambient light change */
    switch (k) {
    case TSS: a0 = rnd() % 8; a1 = rnd() % 33; a2 = rnd() % 3 ? rnd() % 4 : rnd(); break;
    case SURF: {
      static const float fv[] = { 1.0f, 0.0f, 0.5f, 0.75f, 1.0f, 0.2f, 2.0f, 1.0f };
      int i; for (i = 0; i < 3; i++) sp[i] = fv[rnd() % 8];
      if (rnd() % 20 == 0) { u32 b = rnd(); memcpy(&sp[rnd() % 3], &b, 4); }
      if (rnd() % 30 == 0) { u32 b = 0x7f800001u; memcpy(&sp[2], &b, 4); }      /* sNaN diffuse */
      if (rnd() % 3 == 0) memset(col, 0xff, 4); else for (i = 0; i < 4; i++) col[i] = (u8)(rnd() % 3 ? rnd() : rnd() % 2 ? 0xff : 0);
      a0 = (u32)(uintptr_t)sp; a1 = (u32)(uintptr_t)col; a2 = (rnd() % 2 ? 0x40 : 0) | (rnd() % 2 ? 8 : 0) | (rnd() % 4 == 0 ? rnd() : 0);
      if (rnd() % 3 == 0) { a0 = (u32)(uintptr_t)fogv; a1 = (u32)(uintptr_t)(fogv + 8); }      /* repeat inputs (same bits as an earlier call) */
      break; }
    case GETRS: a0 = rnd() % 34; pout = &out; a1 = (u32)(uintptr_t)pout; break;
    case TFILT: a0 = rnd() % 8 ? rnd() % 7 : rnd() % 14; break;
    case RSTAGE: a0 = pickras(); a1 = rnd() % 4 ? 0 : rnd() % 8; break;
    case STEX: t = texs + 0x80 * (rnd() % NTEX); a0 = rnd() % 6 == 0 ? 0 : (u32)(uintptr_t)t; a1 = rnd() % 3 ? 0 : rnd() % 8;
      if (t && rnd() % 8 == 0) *(u32 *)(t + 0x50) = (rnd() % 7) | (rnd() % 5) << 8 | (rnd() % 5) << 12;      /* the game changes filter / addressing */
      break;
    case VA: case ZTEST: a0 = rnd() % 3 ? rnd() % 2 : rnd(); break;
    case ST0: case ST5: case ST6: case ST7: a0 = rnd() % 3 ? rnd() % 3 : rnd(); break;
    case ST1: case ST2: case ST3: case ST4: a0 = rnd() % 9; break;
    case SETRS: a0 = rnd() % 33; a1 = rsval(a0); break;
    case SHADE: a0 = rnd() % 3; break;
    case CULL: a0 = rnd() % 4; break;
    case C_SETRS: a0 = rnd() % 0xd2; a1 = rnd() % 3 ? rnd() % 4 : rnd(); break;
    case C_GETRS: a0 = rnd() % 0xd2; pout = &out; a1 = (u32)(uintptr_t)pout; break;
    case C_SETSS: a0 = rnd() % 8; a1 = rnd() % 14; a2 = rnd() % 3 ? rnd() % 4 : rnd(); break;
    case C_SRC: case C_DST: a0 = rnd() % 12; break;
    case C_GETSTAGE: a0 = rnd() % 8; break;
    }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1); lg(a2);
    cw_set(cw);
    r = ((G3)f[k])(a0, a1, a2); ncalls[k]++;
    lg(r); lg(cw_now()); lg(out); lg(bss_hash());
    cw_set(0x27f);
  }
  for (u32 a = BSS0; a < BSS1; a += 4) lg(W(a));
}

static u32 *save_log, nsave;
static int compare(u32 seed, unsigned short cw) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u cw %x: log differs at entry %u (step %u op %s): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, op < NALL ? F[op].dll : "?",
           i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
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
  void (__cdecl *bind)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps / 2), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  rasters = malloc(0x60 * NRAS); pals = malloc(0x404 * 4); texs = malloc(0x80 * NTEX); fogv = malloc(64 * 4);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test of the fake device: the original Open against it gives a non-empty call log and no trap */
  reset(1, 0x7f); ((G3)(uintptr_t)F[OPEN].va)(0, 0, 0);
  printf("fakedev selftest: original Open logged %u words (%s)\n", nlg, nlg ? "ok" : "EMPTY");
  if (!nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3d_bind");
  if (!bind) { printf("mflrw_d3d_bind MISSING export\n"); return 1; }
  bind((void *)DEVVAR, (void *)BSS0, (void *)CAPS, (void *)RASEXT, (void *)AMB, (void *)0x5dff00, (void *)0x5ea148, (void *)0x4e3c00, (void *)0x4e3cd0, (void *)0x4e53a0, (void *)0x4e53f0);
  for (i = 0; i < NALL; i++) {
    fn[1][i] = (void *)(uintptr_t)F[i].va;
    fn[0][i] = i < NF ? (void *)GetProcAddress(m, F[i].dll) : fn[1][i];
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
  for (i = 0; i < NALL; i++) printf("%-50s %9ld calls (both sides)\n", F[i].dll, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
