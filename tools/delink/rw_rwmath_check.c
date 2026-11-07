/* rw_rwmath_check.c: bit-exact check of the librw DLL's rwmath kernels (tools/delink/rw_rwmath.txt, librw-mfl capi/rwmath.cpp: RwFrame
   transforms, camera / bbox tests, CameraBuildParallelViewMatrix, the SSE and x87 vector module routines, RpMorphTargetCalcBoundingSphere)
   against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_hanimcam_check.c (same image mapping: this
   exe is linked at 0x400000 and MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000, so the originals run in place).
   As in the game link, the original's calls into functions the link takes from the DLL (g0 _rwSqrt 0x4d5410 / _rwInvSqrt 0x4d5470, g1
   RwMatrixMultiply 0x4d3cd0 / Translate 0x4d4870 / Scale 0x4d46e0 / Transform 0x4d49c0 / Rotate 0x4d43f0 / OrthoNormalize 0x4d3d50 /
   Optimize 0x4d3c00) are pointed at the DLL's exports (jmp patch).  RwBBoxCalculate 0x4d18d0 stays original and both sides call it (the DLL
   through mflrw_rwmath_bind).  RwEngineInstance 0x7d2128 points at a harness block (z buffer range +0x18 / +0x1c, dirty frame list +0xbc).
   usage: rw_rwmath_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N finite cases + N/5 special cases (inf / NaN / random bits in the inputs; reported, not counted).  x87 CW 0x7f (the
   game's); MXCSR 0x9fc0 (FTZ / DAZ) on even cases, 0x1f80 on odd ones.  Compared per case: the whole object arena after the call (inputs,
   outputs, random bytes around them), the engine's dirty list head, the return value (not for the matrix product: the original returns
   the caller's eax) and the control word / MXCSR.
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (vector x <-> y, near <-> far, bbox sup.x <-> inf.x, ...;
   for SetIdentity the root's flag bit 0 flipped); the check must report DIFF for every entry.
   R1 (2026-11-06): every call goes through an asm trampoline that loads sentinels into ebx / esi / edi / ebp and reads them back after the call,
   together with the x87 TOP (status word bits 11-13) and the tag word (fnstenv).  Per entry, over all cases (finite and special): the four registers
   must come back unchanged on both sides and TOP / tag must equal the original's, else REGDIFF.  fld = planted fixture: an unbalanced fld1 runs in
   front of the DLL entry; the check must report REGDIFF for every entry. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_FTR, K_FSC, K_FTF, K_FRO, K_FID, K_FON, K_BBP, K_ZSS, K_FTS, K_ISS, K_IBB, K_PVM, K_VPS, K_VVS, K_VP, K_VV, K_MM, K_BSP, NK0 };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_RwFrameTranslate", 0x4d1460, K_FTR}, {"rw37_RwFrameScale", 0x4d1500, K_FSC}, {"rw37_RwFrameTransform", 0x4d15a0, K_FTF},
  {"rw37_RwFrameRotate", 0x4d1640, K_FRO}, {"rw37_RwFrameSetIdentity", 0x4d16e0, K_FID}, {"rw37_RwFrameOrthoNormalize", 0x4d17a0, K_FON},
  {"rw37_RwBBoxContainsPoint", 0x4d1a50, K_BBP}, {"rw37_CameraUpdateZShiftScale", 0x4d1c00, K_ZSS}, {"rw37_RwCameraFrustumTestSphere", 0x4d1cd0, K_FTS},
  {"rw37_RwD3D9CameraIsSphereFullyInsideFrustum", 0x4dbcb0, K_ISS}, {"rw37_RwD3D9CameraIsBBoxFullyInsideFrustum", 0x4dbd10, K_IBB},
  {"rw37_CameraBuildParallelViewMatrix", 0x4d2bc0, K_PVM},
  {"rw37__rwIntelSSEV3dTransformPoints", 0x4ec280, K_VPS}, {"rw37__rwIntelSSEV3dTransformVectors", 0x4ec310, K_VVS},
  {"rw37__rwIntelSSEV3dTransformPoint", 0x4ec3a0, K_VP}, {"rw37__rwIntelSSEV3dTransformVector", 0x4ec420, K_VV},
  {"rw37__rwIntelSSEP4V3dTransformPoints", 0x4ec490, K_VPS}, {"rw37__rwIntelSSEP4V3dTransformVectors", 0x4ec530, K_VVS},
  {"rw37__rwIntelSSEP4V3dTransformPoint", 0x4ec5c0, K_VP}, {"rw37__rwIntelSSEP4V3dTransformVector", 0x4ec650, K_VV},
  {"rw37__rwIntelx86AsmV3dTransformPoints", 0x5dc000, K_VPS}, {"rw37__rwIntelx86AsmV3dTransformVectors", 0x5dc07f, K_VVS},
  {"rw37__rwIntelx86AsmV3dTransformPoint", 0x5dc0ef, K_VP}, {"rw37__rwIntelx86AsmV3dTransformVector", 0x5dc160, K_VV},
  {"rw37__rwIntelx86AsmMatrixMultiply", 0x5dc2a0, K_MM}, {"rw37_RpMorphTargetCalcBoundingSphere", 0x4f8800, K_BSP}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(void *);
typedef u32 (__cdecl *F2)(void *, void *);
typedef u32 (__cdecl *F3)(void *, void *, void *);
typedef u32 (__cdecl *F3i)(void *, void *, int32_t);
typedef u32 (__cdecl *FRot)(void *, void *, float, int32_t);
typedef u32 (__cdecl *F4)(void *, void *, int32_t, void *);
typedef void (__cdecl *Bind)(void *engineVar, void *bboxCalculate);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float ext;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float spf(float f) {      /* range cases scale by ext; in special mode a few values become inf / NaN / random bits */
  u32 r;
  if (ext != 1 && rnd() % 2) f *= ext;
  if (!special || rnd() % 8) return f;
  r = rnd() % 4; return bitsf(r == 0 ? 0x7f800000 : r == 1 ? 0xff800000 : r == 2 ? 0x7fc00000 : rnd());
}
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static u32 mx_now(void) { u32 x; __asm__ volatile("stmxcsr %0" : "=m"(x)); return x; }
static void mx_set(u32 x) { __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
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
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }

/* ---- object arena (both sides start from the same bytes; compared after the call) */
#define ASZ (1u << 18)
static u8 *arena, *snap, *after0;
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; return p; }
static void *allocr(u32 n) { u8 *p = alloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
static u32 aoff(const void *p) { return (const u8 *)p >= arena && (const u8 *)p < arena + ASZ ? (u32)((const u8 *)p - arena) : 0xbad00000u ^ (u32)(uintptr_t)p; }
static u8 *eng;

static void rmatrix(float *m, int kind) {      /* RwMatrix: rotation * scale + pos (kind 0), random (1), identity-flagged (2) */
  int a;
  if (kind == 1) { for (a = 0; a < 16; a++) m[a] = spf(uf(-2, 2)); }
  else {
    float q[4], l = 0, x, y, z, w, s = kind == 2 ? 1 : (rnd() % 3 ? 1 : uf(0.2f, 3));
    for (a = 0; a < 4; a++) { q[a] = uf(-1, 1); l += q[a] * q[a]; } l = (float)sqrt(l); if (l == 0) { q[3] = 1; l = 1; }
    x = q[0] / l; y = q[1] / l; z = q[2] / l; w = q[3] / l;
    if (kind == 2) { x = y = z = 0; w = 1; }
    m[0] = s * (1 - 2 * (y * y + z * z)); m[1] = s * 2 * (x * y + w * z); m[2] = s * 2 * (x * z - w * y);
    m[4] = s * 2 * (x * y - w * z); m[5] = s * (1 - 2 * (x * x + z * z)); m[6] = s * 2 * (y * z + w * x);
    m[8] = s * 2 * (x * z + w * y); m[9] = s * 2 * (y * z - w * x); m[10] = s * (1 - 2 * (x * x + y * y));
    for (a = 0; a < 12; a++) if ((a & 3) != 3 && kind != 2) m[a] = spf(m[a]);
  }
  for (a = 12; a < 15; a++) m[a] = kind == 2 ? 0 : spf(uf(-100, 100));
  ((u32 *)m)[3] = kind == 2 ? 0x20003 : rnd() % 4 ? 3 : rnd() % 2 ? 0 : rnd();
}
static float *rv3(int n, float lo, float hi) { float *v = allocr(12 * n + 8); int k; for (k = 0; k < 3 * n; k++) v[k] = spf(rnd() % 16 ? uf(lo, hi) : (float)(int)uf(lo, hi)); return v; }
static u8 *mkframe(void) {      /* RwFrame (0xa4) with the root at +0xa0: itself or a second frame */
  u8 *f = allocr(0xc0), *root = rnd() % 2 ? f : allocr(0xc0);
  rmatrix((float *)(f + 0x10), rnd() % 6 ? 0 : rnd() % 2 ? 1 : 2);
  *(u8 **)(f + 0xa0) = root;
  root[3] = rnd() % 3 ? (u8)(rnd() & ~3u) : (u8)rnd();
  return f;
}
static u8 *mkcamera(void) {      /* camera: frame with LTM, projection, recip view window, view offset, near / far, 6 planes */
  u8 *c = allocr(0x124 + 96 + 32), *fr = allocr(0xc0); int k, how = rnd() % 8;
  *(u8 **)(c + 4) = fr; rmatrix((float *)(fr + 0x50), rnd() % 8 ? 0 : 1);
  *(int32_t *)(c + 0x14) = rnd() % 8 ? 1 + rnd() % 2 : (int32_t)rnd() % 4;
  *(float *)(c + 0x70) = spf(how == 0 ? uf(100, 1000) : uf(0.2f, 10)); *(float *)(c + 0x74) = spf(rnd() % 4 ? *(float *)(c + 0x70) * uf(1, 2) : uf(0.2f, 10));
  *(float *)(c + 0x78) = spf(rnd() % 3 ? 0 : uf(-1, 1)); *(float *)(c + 0x7c) = spf(rnd() % 3 ? 0 : uf(-1, 1));
  *(float *)(c + 0x80) = spf(rnd() % 4 ? uf(0.01f, 2) : uf(1e-4f, 1e-2f));
  *(float *)(c + 0x84) = spf(rnd() % 6 ? *(float *)(c + 0x80) + uf(1, 5000) : *(float *)(c + 0x80) * uf(0, 2));
  for (k = 0; k < 6; k++) {
    float *p = (float *)(c + 0x94 + 0x14 * k), l; u8 *b = (u8 *)(p + 4);
    p[0] = uf(-1, 1); p[1] = uf(-1, 1); p[2] = uf(-1, 1); l = (float)sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]); if (l > 0 && rnd() % 4) { p[0] /= l; p[1] /= l; p[2] /= l; }
    p[0] = spf(p[0]); p[1] = spf(p[1]); p[2] = spf(p[2]); p[3] = spf(uf(-50, 50));
    b[0] = p[0] >= 0; b[1] = p[1] >= 0; b[2] = p[2] >= 0; if (rnd() % 8 == 0) b[rnd() % 3] ^= 1;
  }
  return c;
}

/* call trampoline: g_fn(g_args[0..3]) with ebx / esi / edi / ebp = g_sent; afterwards g_out = those registers, g_ret = eax, g_env = fnstenv image
   (then fninit + the call's control word: an unbalanced callee does not leave the harness with a dirty x87 stack).  plant_fld: the fld fixture's unbalanced push in front of g_fn2. */
u32 g_fn, g_fn2, g_args[4], g_ret, g_sent[4], g_out[4], g_savesp; u16 g_env[14];
u32 tramp(void); extern char plant_fld[];
__asm__(".text\n.globl _tramp\n_tramp:\n"
        " pushl %ebx\n pushl %esi\n pushl %edi\n pushl %ebp\n movl %esp, _g_savesp\n"
        " pushl _g_args+12\n pushl _g_args+8\n pushl _g_args+4\n pushl _g_args\n"
        " movl _g_sent, %ebx\n movl _g_sent+4, %esi\n movl _g_sent+8, %edi\n movl _g_sent+12, %ebp\n"
        " call *_g_fn\n"
        " movl %ebx, _g_out\n movl %esi, _g_out+4\n movl %edi, _g_out+8\n movl %ebp, _g_out+12\n movl %eax, _g_ret\n"
        " fnstenv _g_env\n fninit\n fldcw _g_env\n"
        " movl _g_savesp, %esp\n popl %ebp\n popl %edi\n popl %esi\n popl %ebx\n ret\n"
        ".globl _plant_fld\n_plant_fld:\n fld1\n jmp *_g_fn2\n");
static u32 call4(void *fn, u32 a, u32 b, u32 c, u32 d) { g_fn = (u32)(uintptr_t)fn; g_args[0] = a; g_args[1] = b; g_args[2] = c; g_args[3] = d; return tramp(); }
#define P(x) ((u32)(uintptr_t)(x))

static void swap32(void *a, void *b) { u32 t; memcpy(&t, a, 4); memcpy(a, b, 4); memcpy(b, &t, 4); }
static void *nega, *negb; static int negflip;

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[4096]; int k; for (k = 0; k < 4096; k++) b[k] = 0; }
static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"), fld = argc > 5 && !strcmp(argv[5], "fld"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 40u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ + 64); snap = malloc(ASZ); after0 = malloc(ASZ); if (!arena || !snap || !after0) { printf("no memory\n"); return 2; }
  arena = (u8 *)(((uintptr_t)arena + 15) & ~(uintptr_t)15);
  { static const char *g[][2] = {{"_rwInvSqrt", (char *)0x4d5470}, {"_rwSqrt", (char *)0x4d5410}, {"RwMatrixMultiply", (char *)0x4d3cd0}, {"RwMatrixTranslate", (char *)0x4d4870},
      {"RwMatrixScale", (char *)0x4d46e0}, {"RwMatrixTransform", (char *)0x4d49c0}, {"RwMatrixRotate", (char *)0x4d43f0}, {"RwMatrixOrthoNormalize", (char *)0x4d3d50}, {"RwMatrixOptimize", (char *)0x4d3c00}};
    int k; for (k = 0; k < 9; k++) { void *p = (void *)GetProcAddress(m, g[k][0]); if (!p) { printf("%s MISSING export (g0/g1)\n", g[k][0]); return 1; } jmp_patch((u32)(uintptr_t)g[k][1], p); } }
  eng = calloc(1, 0x1000); W(0x7d2128) = (u32)(uintptr_t)eng;
  bind = (Bind)GetProcAddress(m, "mflrw_rwmath_bind");
  if (!bind) { printf("mflrw_rwmath_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x4d18d0);
  cw_set(0x7f); mx_set(0x9fc0);
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : fld ? "  PLANTED FIXTURE: unbalanced fld1 in front of the DLL entry" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, badreg = 0, kind = T[fi].kind; long cov[4] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-46s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      void *a0 = 0, *a1 = 0, *a2 = 0; int32_t n = 0, op = 0; float ang = 0; u32 ret[2], eh[2], x87[2], regbad = 0, mxc = i % 2 ? 0x1f80 : 0x9fc0, lnk, angb;
      cur_fi = fi; cur_case = i; special = i >= N; an = 0; nega = negb = 0; negflip = 0;
      ext = rnd() % 6 ? 1.0f : (float)ldexp(1.0, (rnd() % 2 ? 1 : -1) * (int)(40 + rnd() % 80));
      lnk = rnd() % 3 ? 0 : (u32)(uintptr_t)allocr(16);      /* the engine's dirty list: empty (head -> itself) or one foreign link */
      *(float *)(eng + 0x18) = spf(rnd() % 4 ? 0 : uf(0, 0.5f)); *(float *)(eng + 0x1c) = spf(rnd() % 4 ? 1 : uf(0.5f, 70000));
      switch (kind) {
      case K_FTR: case K_FSC: case K_FRO: case K_FTF: case K_FID: case K_FON:
        a0 = mkframe(); op = rnd() % 3; cov[op]++;
        if (kind == K_FTF) { a1 = allocr(64); rmatrix(a1, rnd() % 4 ? 0 : rnd() % 2 ? 1 : 2); nega = a1; negb = (u8 *)a1 + 4; }
        else if (kind == K_FTR || kind == K_FSC || kind == K_FRO) { a1 = rv3(1, -3, 3); nega = a1; negb = (u8 *)a1 + 4; ang = spf(rnd() % 8 ? uf(-360, 360) : (float)(90 * (int)(rnd() % 5))); }
        else if (kind == K_FON) { nega = (u8 *)a0 + 0x10; negb = (u8 *)a0 + 0x14; }
        else negflip = 1;
        cov[3] += !(((u8 *)*(u8 **)((u8 *)a0 + 0xa0))[3] & 3);
        break;
      case K_BBP:
        a0 = rv3(2, -10, 10); a1 = rv3(1, -12, 12);
        if (rnd() % 4 == 0) { k = rnd() % 3; ((float *)a1)[k] = ((float *)a0)[k + 3 * (rnd() % 2)]; }
        if (rnd() % 2) for (k = 0; k < 3; k++) { float lo = ((float *)a0)[k + 3], hi = ((float *)a0)[k]; if (lo > hi) { ((float *)a0)[k] = lo; ((float *)a0)[k + 3] = hi; } }
        nega = a1; negb = (u8 *)a1 + 4; break;
      case K_ZSS: a0 = mkcamera(); nega = (u8 *)a0 + 0x80; negb = (u8 *)a0 + 0x84; cov[*(int32_t *)((u8 *)a0 + 0x14) == 2]++; break;
      case K_FTS: case K_ISS: case K_IBB:
        a0 = mkcamera();
        if (kind == K_IBB) { a1 = rv3(2, -60, 60); nega = a1; negb = (u8 *)a1 + 12; }
        else { a1 = rv3(2, -60, 60); ((float *)a1)[3] = spf(rnd() % 8 ? uf(0, 40) : uf(-5, 0)); nega = a1; negb = (u8 *)a1 + 4; }
        break;
      case K_PVM: a0 = mkcamera(); nega = (u8 *)a0 + 0x70; negb = (u8 *)a0 + 0x74; break;
      case K_VPS: case K_VVS: case K_VP: case K_VV: {
        int one = kind == K_VP || kind == K_VV;
        n = one ? 1 : 1 + rnd() % 24;
        a1 = rv3(n, -100, 100); a0 = rnd() % 4 == 0 ? a1 : allocr(12 * n + 8);
        a2 = (u8 *)allocr(64 + 16) + (rnd() % 2 ? 0 : 4 * (1 + rnd() % 3)); rmatrix(a2, rnd() % 6 ? 0 : 1);
        cov[a0 == a1]++; cov[2 + (((u32)(uintptr_t)a2 & 15) != 0)]++;
        nega = a1; negb = (u8 *)a1 + 4; break; }
      case K_MM:
        a1 = allocr(64); a2 = allocr(64); rmatrix(a1, rnd() % 3 ? 0 : 1); rmatrix(a2, rnd() % 3 ? 0 : 1);
        k = rnd() % 4; a0 = k == 0 ? a1 : k == 1 ? a2 : allocr(64); cov[k < 2 ? k : 2]++;
        nega = a1; negb = (u8 *)a1 + 4; break;
      case K_BSP: {
        u8 *g = allocr(0x40); n = rnd() % 4 == 0 ? 1 : 1 + (int32_t)(rnd() % 40);      /* n = 0: RwBBoxCalculate loops ~2^32 times in the original (never called so) */
        *(int32_t *)(g + 0x14) = n; a0 = allocr(0x20); *(u8 **)a0 = g; *(float **)((u8 *)a0 + 0x14) = rv3(n > 0 ? n : 1, -50, 50);
        a1 = allocr(16 + 16); cov[n == 0 ? 0 : n == 1 ? 1 : 2]++;
        nega = *(float **)((u8 *)a0 + 0x14); negb = (u8 *)nega + 4; break; }
      }
      memcpy(snap, arena, an);
      for (side = 0; side < 2; side++) {      /* both sides run on the same arena bytes (restored before each) */
        void *fn = side ? orig : dll; u32 r = 0;
        memcpy(arena, snap, an);
        *(u32 *)(eng + 0xbc) = lnk ? lnk : (u32)(uintptr_t)(eng + 0xbc); *(u32 *)(eng + 0xc0) = (u32)(uintptr_t)(eng + 0xbc);
        if (neg && !side) { if (nega) swap32(nega, negb); if (negflip) ((u8 *)*(u8 **)((u8 *)a0 + 0xa0))[3] ^= 1; }
        cw_set(0x7f); mx_set(mxc);
        stackwipe();
        if (fld && !side) { g_fn2 = (u32)(uintptr_t)fn; fn = plant_fld; }
        for (k = 0; k < 4; k++) g_sent[k] = 0x5e000000u ^ (u32)i * 0x9e3779b1u ^ (u32)k << 28 ^ (u32)side << 20;
        memcpy(&angb, &ang, 4);
        switch (kind) {      /* every entry is cdecl with up to 4 word arguments; the trampoline pushes 4 (the extra ones are harmless) */
        case K_FTR: case K_FSC: case K_FTF: r = call4(fn, P(a0), P(a1), (u32)op, 0); break;
        case K_FRO: r = call4(fn, P(a0), P(a1), angb, (u32)op); break;
        case K_FID: case K_FON: case K_ZSS: case K_PVM: r = call4(fn, P(a0), 0, 0, 0); break;
        case K_BBP: case K_FTS: case K_ISS: case K_IBB: case K_BSP: r = call4(fn, P(a0), P(a1), 0, 0); break;
        case K_VPS: case K_VVS: r = call4(fn, P(a0), P(a1), (u32)n, P(a2)); break;
        case K_VP: case K_VV: r = call4(fn, P(a0), P(a1), P(a2), 0); break;
        case K_MM: call4(fn, P(a0), P(a1), P(a2), 0); r = 0; break;
        }
        for (k = 0; k < 4; k++) regbad |= g_out[k] != g_sent[k];
        x87[side] = (u32)(g_env[2] >> 11 & 7) << 16 | g_env[4];      /* TOP, tag word */
        if (side && (kind == K_BBP || kind == K_FTS || kind == K_ISS || kind == K_IBB)) cov[r & 3]++;      /* coverage: the original's answers */
        if (side && kind == K_PVM) cov[(*(u32 *)((u8 *)a0 + 0x2c) & 3)]++;
        r = aoff((void *)(uintptr_t)r) ^ ((u32)cw_now() << 16) ^ (mx_now() & 0xffff ^ mxc); cw_set(0x7f); mx_set(0x9fc0);
        if (neg && !side && nega) swap32(nega, negb);      /* inputs back: only the outputs can differ */
        ret[side] = r; eh[side] = aoff(*(u8 **)(eng + 0xbc)) ^ aoff(*(u8 **)(eng + 0xc0)) << 1;
        if (lnk) eh[side] ^= aoff(*(u8 **)(uintptr_t)(lnk + 4)) * 3;
        if (!side) memcpy(after0, arena, an);
      }
      if (regbad || x87[0] != x87[1]) {      /* counted in finite and special cases alike */
        if (++badreg <= 3) printf("  REGDIFF %s case %d: callee-saved %s, x87 TOP/tag dll %05x orig %05x\n", T[fi].name, i, regbad ? "CHANGED" : "kept", x87[0], x87[1]);
      }
      if (ret[0] == ret[1] && eh[0] == eh[1] && !memcmp(after0, arena, an)) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, engine list %08x / %08x", T[fi].name, i, ret[0], ret[1], eh[0], eh[1]);
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x (a0 +0x%x): dll %08x orig %08x", k, k - aoff(a0), *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-46s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld; %d of %d special cases differ; regs/x87 %s", cov[0], cov[1], cov[2], cov[3], badsp, N / 5, badreg ? "REGDIFF" : "EQUAL");
    if (badreg) printf(" (%d cases)", badreg);
    printf("\n"); bad_total += bad != 0 || badreg != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
