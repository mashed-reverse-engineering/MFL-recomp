/* rw_hanimcam_check.c: bit-exact check of the librw DLL's hanimcam kernels (tools/delink/rw_hanimcam.txt, librw-mfl capi/hanimcam.cpp:
   CameraBuildPerspClipPlanes, CameraBuildParallelClipPlanes, RpHAnimHierarchyUpdateMatrices) against the ORIGINAL code of MFL.exe, both run
   natively (Wine) on the same CPU.  Made from rw_splanim_check.c (same image mapping: this exe is linked at 0x400000 and MFL.exe's sections
   are copied into a bss block that spans 0x4b0000..0x94b000, so the original functions run in place).
   As in the game link, the original's calls into functions the link takes from the DLL (g0 _rwInvSqrt 0x4d5470, g1 RwMatrixMultiply
   0x4d3cd0) are pointed at the DLL's exports (jmp patch).  The exe functions the DLL reaches through mflrw_hanimcam_bind (RwFrameDirty
   0x4d0c30, RwFrameGetLTM 0x4d0ff0, RwFrameUpdateObjects 0x4d0f70, RwV3dTransformPoints 0x4d5670) are patched to loggers with a
   deterministic effect; both sides call them.  RwEngineInstance 0x7d2128 points at a harness block (its dirty frame list at +0xbc).
   usage: rw_hanimcam_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N finite cases + N/5 special cases (inf / NaN / random bits in the inputs; reported, not counted).  x87 CW 0x7f (the
   game's), MXCSR 0x9fc0.  Compared per case: the whole object arena after the call (camera / frames / hierarchy / matrices / keyframes with
   random bytes around them), the engine's dirty list head, the callback log (frame calls, transform calls, custom apply callbacks with
   their arguments and the CW), the return value (HAnim) and the control word.  The stack below the caller is zeroed before each call on both
   sides (the original reads a few never-written stack words: identity matrix flags, local matrix pads; the DLL models them as 0).
   neg = negative fixture (negtest.sh): the DLL side gets the camera's near and far planes swapped and the hierarchy's first keyframe
   quaternion x <-> y; the check must report DIFF for all three entries. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_PERSP, K_PARA, K_HANIM, NK };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_CameraBuildPerspClipPlanes", 0x4d2110, K_PERSP}, {"rw37_CameraBuildParallelClipPlanes", 0x4d27b0, K_PARA},
  {"rw37_RpHAnimHierarchyUpdateMatrices", 0x544450, K_HANIM}};
typedef void (__cdecl *FCam)(void *cam);
typedef int32_t (__cdecl *FHan)(void *h);
typedef void (__cdecl *Bind)(void *engineVar, void *kfApply, void *frameDirty, void *frameGetLTM, void *frameUpdateObjects, void *v3dTransformPoints);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static u32 fbits(float f) { u32 b; memcpy(&b, &f, 4); return b; }
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
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
static void jmp_patch(u32 va, void *to) { *(u8 *)(uintptr_t)va = 0xe9; *(int32_t *)(uintptr_t)(va + 1) = (int32_t)((u32)(uintptr_t)to - (va + 5)); }

/* ---- object arena (both sides start from the same bytes; compared after the call) */
#define ASZ (1u << 20)
static u8 *arena, *snap, *after0;
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; return p; }
static void *allocr(u32 n) { u8 *p = alloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
static u32 aoff(const void *p) { return (const u8 *)p >= arena && (const u8 *)p < arena + ASZ ? (u32)((const u8 *)p - arena) : 0xbad00000u ^ (u32)(uintptr_t)p; }

/* ---- logs: one per side */
static u32 *lg[2]; static int ln[2], side, overflow, logon;
static void logw(u32 x) { if (!logon) return; if (ln[side] < (1 << 16)) lg[side][ln[side]++] = x; else overflow = 1; }
static u8 *eng;
/* RwFrameDirty: the answer is bit 0 of the harness word at frame+0xa4 */
static int32_t __cdecl hdirty(u8 *f) { logw(0xd1270000u); logw(aoff(f)); logw(cw_now()); return *(u32 *)(f + 0xa4) & 1; }
static u8 *__cdecl hltm(u8 *f) { logw(0x17a00000u); logw(aoff(f)); logw(cw_now()); return f + 0x50; }
/* RwFrameUpdateObjects: logs and leaves a mark in the harness word */
static u8 *__cdecl hupd(u8 *f) { logw(0x0bd00000u); logw(aoff(f)); logw(cw_now()); *(u32 *)(f + 0xa8) += 0x101; return f; }
/* RwV3dTransformPoints: out = M * in (float, as a plain loop), logged */
static u8 *__cdecl htp(u8 *out, const u8 *in, int32_t n, const u8 *m) {
  int i; const float *M = (const float *)m;
  logw(0x7e900000u); logw(aoff(out)); logw(aoff(in)); logw(n); logw(aoff(m)); logw(cw_now());
  for (i = 0; i < n; i++) {
    float x = ((const float *)in)[3 * i], y = ((const float *)in)[3 * i + 1], z = ((const float *)in)[3 * i + 2];
    ((float *)out)[3 * i] = x * M[0] + y * M[4] + z * M[8] + M[12];
    ((float *)out)[3 * i + 1] = x * M[1] + y * M[5] + z * M[9] + M[13];
    ((float *)out)[3 * i + 2] = x * M[2] + y * M[6] + z * M[10] + M[14];
  }
  return out;
}
/* a custom keyframe apply callback (interpolator +0x3c != RpHAnimKeyFrameApply): a matrix from the frame words, sometimes with the identity
   flag or written pads */
static u32 cbseed;
static void *__cdecl happly(u8 *m, u8 *f) {
  int k; u32 s = cbseed ^ *(u32 *)(f + 8);
  logw(0xa9910000u); logw(m >= arena && m < arena + ASZ ? aoff(m) : 0x57ac0000u); logw(aoff(f)); logw(cw_now());      /* m: the caller's local matrix (a stack address on both sides) */
  for (k = 0; k < 16; k++) if (k != 3 && (k != 7 && k != 11 && k != 15 || (s & 1))) ((float *)m)[k] = ((float *)f)[2 + k % 7] * (float)(1 + k);
  ((u32 *)m)[3] = (s & 6) == 0 ? 0x20003 : (s & 6) == 2 ? 3 : s;
  cbseed = cbseed * 1103515245u + 12345u;
  return m;
}

/* ---- builders */
static float scale, ext;
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
    for (a = 0; a < 16; a++) if (a < 12 && (a & 3) != 3) m[a] = spf(m[a]);
  }
  for (a = 12; a < 15; a++) m[a] = kind == 2 ? 0 : spf(uf(-1, 1) * scale);
  if (ext != 1 && kind != 2 && rnd() % 3 == 0) for (a = 0; a < 15; a++) if ((a & 3) != 3) m[a] *= ext;
  ((u32 *)m)[3] = kind == 2 ? 0x20003 : rnd() % 4 ? 3 : rnd() % 2 ? 0 : rnd();
}
static u8 *mkframe(u8 *parent) {      /* RwFrame (0xa4) + 2 harness words: dirty answer, update mark */
  u8 *f = allocr(0xc0);
  *(u8 **)(f + 4) = parent;
  rmatrix((float *)(f + 0x10), rnd() % 8 ? 0 : rnd() % 2 ? 1 : 2);
  rmatrix((float *)(f + 0x50), rnd() % 8 ? 0 : rnd() % 2 ? 1 : 2);
  *(u32 *)(f + 0xa8) = 0;
  return f;
}
static u8 *mkchain(int depth, u8 **root) {      /* a frame with depth ancestors; root = the top one */
  u8 *f = NULL; int k;
  for (k = 0; k <= depth; k++) f = mkframe(f);
  { u8 *r = f; while (*(u8 **)(r + 4)) r = *(u8 **)(r + 4); *root = r; }
  return f;
}

/* camera: object (frame at +4), view window / offset, near / far, 6 planes, 8 corners, random bytes around */
static u8 *mkcamera(void) {
  u8 *c = allocr(0x124 + 96 + 32), *root, *fr = mkchain(0, &root); int how = rnd() % 8;
  *(u8 **)(c + 4) = fr;
  *(float *)(c + 0x68) = spf(how == 0 ? uf(0.001f, 0.01f) : how == 1 ? uf(1, 50) : uf(0.1f, 2));
  *(float *)(c + 0x6c) = spf(rnd() % 4 ? *(float *)(c + 0x68) * uf(0.5f, 1) : uf(0.05f, 3));
  *(float *)(c + 0x70) = 1.0f / *(float *)(c + 0x68); *(float *)(c + 0x74) = 1.0f / *(float *)(c + 0x6c);
  *(float *)(c + 0x78) = spf(rnd() % 3 ? 0 : uf(-1, 1)); *(float *)(c + 0x7c) = spf(rnd() % 3 ? 0 : uf(-1, 1));
  *(float *)(c + 0x80) = spf(rnd() % 4 ? uf(0.01f, 2) : rnd() % 2 ? uf(1e-4f, 1e-2f) : uf(2, 100));
  *(float *)(c + 0x84) = spf(rnd() % 6 ? *(float *)(c + 0x80) + uf(1, 5000) : *(float *)(c + 0x80) * uf(0, 2));
  if (ext != 1) {      /* range case: products leave the float exponent range (values the original keeps on the x87 stack differ from stored ones) */
    static const int fo[] = {0x68, 0x6c, 0x78, 0x7c, 0x80, 0x84}; int k;
    for (k = 0; k < 6; k++) if (rnd() % 2) *(float *)(c + fo[k]) *= ext;
    for (k = 0; k < 16; k++) if (k % 4 != 3 && rnd() % 2) ((float *)(fr + 0x50))[k] *= ext;
  }
  return c;
}

/* hierarchy {flags, numNodes, pMatrixArray, unaligned, pNodeInfo, parentFrame, parentHierarchy, rootParentOffset, currentAnim} */
static u8 *mkhier(int *covk) {
  u8 *h = allocr(0x40), *ip, *ni, *root, *pf, *mp, *ph; int n, k, ks, depth = 0;
  static const u32 bits[] = {1, 2, 0x1000, 0x2000, 0x4000}; u32 fl = 0;
  for (k = 0; k < 5; k++) if (rnd() % 2) fl |= bits[k];
  if (rnd() % 4 == 0) fl |= rnd() & ~0x7003u;
  n = rnd() % 16 == 0 ? -(int)(rnd() % 3) : (int)(rnd() % 14);
  ks = rnd() % 4 ? 0x24 : 0x24 + 4 * (rnd() % 6);
  *(u32 *)h = fl; *(int32_t *)(h + 4) = n;
  mp = allocr(64 * (n > 0 ? n : 1) + 32); *(u8 **)(h + 8) = mp; *(u8 **)(h + 0xc) = mp;
  ni = allocr(16 * (n > 0 ? n : 1)); *(u8 **)(h + 0x10) = ni;
  for (k = 0; k < n; k++) {
    u32 f = rnd() % 3 == 0 ? 0 : rnd() % 4;
    if (f == 1 && depth == 0) f = 0;
    if (f == 2 && depth >= 12) f = 0;
    depth += f == 2 ? 1 : f == 1 ? -1 : 0;
    *(u32 *)(ni + 16 * k + 8) = f | (rnd() % 4 == 0 ? rnd() & ~3u : 0);
    *(u8 **)(ni + 16 * k + 12) = rnd() % 6 ? mkframe(rnd() % 3 ? NULL : (u8 *)arena) : NULL;
  }
  pf = mkchain(rnd() % 4, &root);
  if (rnd() % 8 == 0) *(u8 **)(pf + 4) = NULL;
  *(u8 **)(pf + 0xa0) = root; *(u8 **)(h + 0x14) = pf;
  ((u8 *)root)[3] = rnd() % 3 ? (u8)(rnd() & ~3u) : (u8)rnd();
  ph = allocr(0x24); *(u8 **)(ph + 8) = allocr(64 * 6); { u8 *r2, *p2 = mkchain(rnd() % 3, &r2); if (rnd() % 6 == 0) p2 = NULL; *(u8 **)(ph + 0x14) = p2; }
  for (k = 0; k < 6; k++) rmatrix((float *)(*(u8 **)(ph + 8) + 64 * k), rnd() % 6 ? 0 : 2);
  *(u8 **)(h + 0x18) = ph; *(int32_t *)(h + 0x1c) = rnd() % 4 == 0 ? -1 : (int)(rnd() % 6);
  ip = allocr(0x4c + ks * (n > 0 ? n : 1)); *(u8 **)(h + 0x20) = ip;
  *(int32_t *)(ip + 0x24) = ks;
  *(u32 *)(ip + 0x3c) = rnd() % 5 ? 0x542fe0u : (u32)(uintptr_t)happly;
  for (k = 0; k < (n > 0 ? n : 0); k++) {
    float *q = (float *)(ip + 0x4c + ks * k + 8), l = 0; int a, how = rnd() % 6;
    for (a = 0; a < 4; a++) { q[a] = uf(-1, 1); l += q[a] * q[a]; } l = (float)sqrt(l);
    if (how && l > 0) for (a = 0; a < 4; a++) q[a] /= l;
    if (how == 1) { q[0] = q[1] = q[2] = 0; q[3] = 1; }
    for (a = 0; a < 4; a++) q[a] = spf(ext != 1 && rnd() % 2 ? q[a] * ext : q[a]);
    for (a = 4; a < 7; a++) q[a] = spf(uf(-1, 1) * scale);
  }
  covk[0] = (fl & 2) ? 0 : (fl & 1) && *(int32_t *)(h + 0x1c) != -1 ? 1 : (fl & 0x4000) ? 2 : 3;
  covk[1] = *(u32 *)(ip + 0x3c) == 0x542fe0u ? 4 : 5;
  return h;
}

/* negative fixture: near <-> far of the camera, x <-> y of the first keyframe quaternion (applied before the DLL's call and undone after it) */
static void negswap(int kind, u8 *obj) {
  u8 *a, *b; u32 t;
  if (kind != K_HANIM) { a = obj + 0x80; b = obj + 0x84; }
  else if (*(int32_t *)(obj + 4) > 0) { a = *(u8 **)(obj + 0x20) + 0x4c + 8; b = a + 4; }
  else return;
  t = *(u32 *)a; *(u32 *)a = *(u32 *)b; *(u32 *)b = t;
}

/* zero the stack below the caller (the region the callee's frame uses) */
static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[8192]; int k; for (k = 0; k < 8192; k++) b[k] = 0; }

/* watchdog and crash report */
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  u32 eng0[2][4];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 16u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); after0 = malloc(ASZ); lg[0] = malloc(4 << 16); lg[1] = malloc(4 << 16); if (!arena || !snap || !after0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  { static const char *g[][2] = {{"_rwInvSqrt", (char *)0x4d5470}, {"RwMatrixMultiply", (char *)0x4d3cd0}}; int k;
    for (k = 0; k < 2; k++) { void *p = (void *)GetProcAddress(m, g[k][0]); if (!p) { printf("%s MISSING export (g0/g1)\n", g[k][0]); return 1; } jmp_patch((u32)(uintptr_t)g[k][1], p); } }
  jmp_patch(0x4d0c30, (void *)hdirty); jmp_patch(0x4d0ff0, (void *)hltm); jmp_patch(0x4d0f70, (void *)hupd); jmp_patch(0x4d5670, (void *)htp);
  eng = calloc(1, 0x1000); W(0x7d2128) = (u32)(uintptr_t)eng;
  bind = (Bind)GetProcAddress(m, "mflrw_hanimcam_bind");
  if (!bind) { printf("mflrw_hanimcam_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x542fe0, (void *)0x4d0c30, (void *)0x4d0ff0, (void *)0x4d0f70, (void *)0x4d5670);
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets near <-> far and the first keyframe's quat x <-> y" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, kind = T[fi].kind; long cov[6] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u8 *obj; int covk[2] = {0, 0}; u32 seed;
      cur_fi = fi; cur_case = i;
      special = i >= N; an = 0; logon = 0;
      scale = (float)ldexp(1.0, (int)(rnd() % 20) - 6);
      ext = rnd() % 6 ? 1.0f : (float)ldexp(1.0, (rnd() % 2 ? 1 : -1) * (int)(40 + rnd() % 80));
      if (kind == K_HANIM) { obj = mkhier(covk); cov[covk[0]]++; cov[covk[1]]++; }
      else { obj = mkcamera(); cov[*(float *)(obj + 0x78) != 0 || *(float *)(obj + 0x7c) != 0]++; }
      cbseed = seed = rnd();
      memcpy(snap, arena, an);
      for (side = 0; side < 2; side++) {      /* both sides run on the same arena bytes (restored before each) */
        void *fn = side ? orig : dll; u32 r = 0;
        memcpy(arena, snap, an); ln[side] = 0; overflow = 0; cbseed = seed; logon = 1;
        *(u32 *)(eng + 0xbc) = (u32)(uintptr_t)(eng + 0xbc); *(u32 *)(eng + 0xc0) = (u32)(uintptr_t)(eng + 0xbc);
        if (neg && !side) negswap(kind, obj);
        cw_set(0x7f);
        stackwipe();
        if (kind == K_HANIM) r = (u32)((FHan)fn)(obj); else ((FCam)fn)(obj);
        logw(r); logw(0xc0de0000u | cw_now()); cw_set(0x7f);
        if (neg && !side) negswap(kind, obj);      /* inputs back: only the outputs can differ */
        if (overflow) logw(0x0f10f1);
        logon = 0;
        eng0[side][0] = aoff(*(u8 **)(eng + 0xbc)); eng0[side][1] = aoff(*(u8 **)(eng + 0xc0));
        if (!side) memcpy(after0, arena, an);
      }
      if (ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * ln[0]) && !memcmp(after0, arena, an) && eng0[0][0] == eng0[1][0] && eng0[0][1] == eng0[1][1]) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: %d / %d log words (dll / orig), engine list %x/%x vs %x/%x", T[fi].name, i, ln[0], ln[1], eng0[0][0], eng0[0][1], eng0[1][0], eng0[1][1]);
        for (k = 0; k < ln[0] && k < ln[1]; k++) if (lg[0][k] != lg[1][k]) { printf(", first log diff at word %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x (obj +0x%x): dll %08x orig %08x", k, k - aoff(obj), *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld %ld %ld; %d of %d special cases differ\n", cov[0], cov[1], cov[2], cov[3], cov[4], cov[5], badsp, N / 5); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
