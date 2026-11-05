/* rw_splanim_check.c: bit-exact check of the librw DLL's spline / HAnim keyframe / RtAnim interpolator kernels (tools/delink/rw_splanim.txt,
   librw-mfl capi/splanim.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_collis_check.c
   (same image mapping: this exe is linked at 0x400000 and MFL.exe's sections are copied into a bss block that spans 0x4b0000..0x94b000, so the
   original functions run in place with their internal calls).
   The original's calls into functions that the game link already takes from the DLL (g0 RwV3dNormalize 0x4d5290 / _rwSqrt 0x4d5410 /
   _rwInvSqrt 0x4d5470, g1 RwMatrixOptimize 0x4d3c00) are pointed at the DLL's exports (jmp patch), as in the game link; the original
   _rwV3dNormalize 0x4d51f0 is part of this group and stays original.  RwErrorSet 0x4ea160 is patched to a logger (both sides call it: the
   DLL gets its address from mflrw_splanim_bind).  The spline tables come from the original SplineOpen 0x556bf0, the sqrt tables from the
   math ctor 0x4d5770.
   usage: rw_splanim_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   Per entry point N finite cases + N/5 special cases (random bits; reported, not counted).  x87 CW 0x7f (the game's), MXCSR 0x9fc0.
   Compared per case: the whole object arena after the call (splines, matrices, keyframes, interpolators), the return value (80 bit for
   float returns), the RwErrorSet reports, the RwMalloc / RwFree sequence (size, hint, which block; injected allocation failures), the
   callback logs (RtAnim keyframe interpolate / anim / loop callbacks: arguments, interpolator state, CW), the control word.
   neg = negative fixture (negtest.sh): the DLL's RpSplineFindPosition / RpSplineFindMatrix get path 10 and 11 swapped (smooth <-> nice
   ends); the check must report DIFF for both. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_NORM, K_FPOS, K_FMAT, K_CREATE, K_KFI, K_SET, K_ADD, K_SUB, NK };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rwV3dNormalize", 0x4d51f0, K_NORM}, {"rw37_RpSplineFindPosition", 0x555ff0, K_FPOS}, {"rw37_RpSplineFindMatrix", 0x556360, K_FMAT},
  {"rw37__splCreateSpline", 0x555970, K_CREATE}, {"rw37_RpHAnimKeyFrameInterpolate", 0x543100, K_KFI},
  {"rw37_RtAnimInterpolatorSetCurrentAnim", 0x53e3c0, K_SET}, {"rw37_RtAnimInterpolatorAddAnimTime", 0x53e640, K_ADD},
  {"rw37_RtAnimInterpolatorSubAnimTime", 0x53e4d0, K_SUB}};
typedef long double (__cdecl *FNorm)(float *out, const float *in);
typedef void *(__cdecl *FPos)(void *spline, int32_t path, float where, float *pos, float *tan);
typedef long double (__cdecl *FMat)(void *spline, int32_t path, float where, float *up, float *m);
typedef void *(__cdecl *FCreate)(void *spline, const float *pts);
typedef void (__cdecl *FKfi)(void *out, void *in1, void *in2, float time, void *data);
typedef int32_t (__cdecl *FSet)(void *ip, void *anim);
typedef int32_t (__cdecl *FTime)(void *ip, float t);
typedef void (__cdecl *Bind)(void *engineVar, void *splineTables, void *errorSet);

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

/* ---- logs: one per side */
static u32 *lg[2]; static int ln[2], side, overflow, logon;
static void logw(u32 x) { if (!logon) return; if (ln[side] < (1 << 16)) lg[side][ln[side]++] = x; else overflow = 1; }
/* RwErrorSet 0x4ea160 (patched): both sides report through it */
static void *__cdecl errlog(const int32_t *e) { logw(0xe4404000u); logw(e[0]); logw(e[1]); logw(cw_now()); return (void *)e; }
/* the engine's memory functions: logged, failure injected at the failAt-th call */
static int nalloc, failAt; static void *blk[64];
static void *__cdecl hmalloc(size_t n, u32 hint) {
  void *p; nalloc++; logw(0xa110c000u | (nalloc & 0xfff)); logw((u32)n); logw(hint);
  if (nalloc == failAt) return NULL;
  p = calloc(1, n + 64); if (nalloc < 64) blk[nalloc] = p; return p;
}
static void __cdecl hfree(void *p) { int k, w = -1; for (k = 1; k < 64 && k <= nalloc; k++) if (blk[k] == p) w = k; logw(0xf4ee0000u); logw(w); free(p); }

/* ---- object arena (both sides start from the same bytes; compared after the call) */
#define ASZ (1u << 20)
static u8 *arena, *snap, *after0;
static u32 an;
static void *alloc(u32 n) { void *p; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; return p; }
static void *allocr(u32 n) { u8 *p = alloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }

/* spline: header 0x38 (+8 numCtrl, +0xc type) + vPts; generous room behind the points */
static float sscale;
static void rpt(float *p) {      /* a point of a random curve */
  int a; for (a = 0; a < 3; a++) p[a] = uf(-1, 1) * sscale;
}
static u8 *mkspline(int n, int type, int fill) {
  u8 *s = allocr(0x38 + 12 * (n + 12)); float *v = (float *)(s + 0x38); int i, a, how = rnd() % 6;
  *(int32_t *)(s + 8) = n; *(int32_t *)(s + 0xc) = type;
  if (fill) for (i = 0; i < n + 3; i++) {
    if (how == 0 && i > 0) for (a = 0; a < 3; a++) v[3 * i + a] = v[3 * i - 3 + a] + (a == 0 ? sscale / 8 : 0);      /* straight line: zero acceleration */
    else if (how == 1 && i > 0 && rnd() % 2) memcpy(v + 3 * i, v + 3 * i - 3, 12);      /* repeated points: zero velocity */
    else if (how == 2 && i > 0) for (a = 0; a < 3; a++) v[3 * i + a] = v[3 * i - 3 + a] + uf(-0.2f, 0.2f) * sscale;      /* random walk */
    else rpt(v + 3 * i);
    for (a = 0; a < 3; a++) v[3 * i + a] = spf(v[3 * i + a]);
  }
  return s;
}
static float rwhere(int n) {
  int k = rnd() % 16; float w;
  if (k < 8) w = uf(-1.3f, 1.3f);
  else if (k == 8) { static const float e[] = {0, 0.5f, 1, -1, 0.25f, 0.75f, -0.5f, 2.0f, 1.25f}; w = e[rnd() % 9]; }
  else if (k == 9) { u32 b = fbits(rnd() % 2 ? 1.0f : 0.5f) + (int)(rnd() % 5) - 2; w = bitsf(b); }      /* ulps around 1 and 0.5 */
  else if (k == 10 && n > 3) w = (float)(rnd() % (n - 2)) / (float)(n - 3);      /* parameter boundaries */
  else if (k == 11) w = uf(0, 1e-6f);
  else w = uf(0, 1);
  return w;      /* finite: a NaN / inf parameter makes both sides index far outside the spline (original behaviour, no check) */
}
/* keyframe {prev, time, q[4], t[3]} */
static void rquat(float *q) { int a; float l = 0; for (a = 0; a < 4; a++) { q[a] = uf(-1, 1); l += q[a] * q[a]; } l = (float)sqrt(l); if (l > 0) for (a = 0; a < 4; a++) q[a] /= l; }
static void mkkfpair(float *k1, float *k2) {
  int a, how = rnd() % 10; float ang, ax[3], l = 0;
  rquat(k1 + 2);
  if (how < 4) {      /* q2 = q1 rotated by a small or large angle */
    float c, s, r[4], *q = k1 + 2;
    ang = how == 0 ? uf(0, 0.1f) : how == 1 ? uf(0, 0.05f) : how == 2 ? uf(0, 3.2f) : uf(3.0f, 3.3f);
    for (a = 0; a < 3; a++) { ax[a] = uf(-1, 1); l += ax[a] * ax[a]; } l = (float)sqrt(l); if (l == 0) l = 1;
    c = (float)cos(ang / 2); s = (float)sin(ang / 2);
    r[0] = ax[0] / l * s; r[1] = ax[1] / l * s; r[2] = ax[2] / l * s; r[3] = c;
    k2[2] = r[3] * q[0] + r[0] * q[3] + r[1] * q[2] - r[2] * q[1];
    k2[3] = r[3] * q[1] - r[0] * q[2] + r[1] * q[3] + r[2] * q[0];
    k2[4] = r[3] * q[2] + r[0] * q[1] - r[1] * q[0] + r[2] * q[3];
    k2[5] = r[3] * q[3] - r[0] * q[0] - r[1] * q[1] - r[2] * q[2];
  } else if (how == 4) { for (a = 0; a < 4; a++) k2[2 + a] = rnd() % 2 ? k1[2 + a] : -k1[2 + a]; if (rnd() % 2) for (a = 0; a < 4; a++) k2[2 + a] = k1[2 + a]; }
  else if (how == 5) { static const float o[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 1}, {0, 0, 1, 0}}; int i = rnd() % 4, j = (i + 1 + rnd() % 3) % 4; memcpy(k1 + 2, o[i], 16); memcpy(k2 + 2, o[j], 16); }
  else if (how == 6) { for (a = 0; a < 4; a++) { k1[2 + a] = uf(-2, 2); k2[2 + a] = uf(-2, 2); } }      /* not unit */
  else rquat(k2 + 2);
  for (a = 0; a < 3; a++) { k1[6 + a] = uf(-100, 100); k2[6 + a] = uf(-100, 100); }
  k1[1] = uf(0, 10); k2[1] = k1[1] + (rnd() % 30 ? uf(0.001f, 3) : 0);
  for (a = 1; a < 9; a++) { k1[a] = spf(k1[a]); k2[a] = spf(k2[a]); }
}

/* ---- RtAnim: animation {interpInfo, numFrames, flags, duration, pFrames, customData}, info {id, interpSize, animSize, apply, blend, interp, add},
   interpolator {anim, cur, next, animCB, animCBData, animCBTime, loopCB, loopCBData, maxSize, interpSize, animSize, numNodes, ..., +0x4c frames} */
static u32 cbseed; static int cbcount;
static u8 *cur_ip;
static void *__cdecl icb2(u8 *out, u8 *k1, u8 *k2, float t, void *data) {
  logw(0x1c0b0000u); logw((u32)(uintptr_t)out); logw((u32)(uintptr_t)k1); logw((u32)(uintptr_t)k2); logw(fbits(t)); logw((u32)(uintptr_t)data); logw(cw_now());
  if (cur_ip) { logw(*(u32 *)(cur_ip + 4)); logw(*(u32 *)(cur_ip + 8)); }
  if (cur_ip && *(int32_t *)(cur_ip + 0x24) >= 12) *(u32 *)(out + 8) = fbits(t);
  return out;
}
static void *__cdecl acb(u8 *ip, void *data) {
  int k = ++cbcount; logw(0xacb00000u | k); logw((u32)(uintptr_t)ip); logw((u32)(uintptr_t)data); logw(*(u32 *)(ip + 4)); logw(*(u32 *)(ip + 8)); logw(cw_now());
  return ((cbseed >> (k % 31)) & 1) ? NULL : ip;
}
static void *__cdecl lcb(u8 *ip, void *data) {
  int k = ++cbcount; logw(0x1cb00000u | k); logw((u32)(uintptr_t)ip); logw((u32)(uintptr_t)data); logw(*(u32 *)(ip + 4)); logw(*(u32 *)(ip + 8)); logw(cw_now());
  return ((cbseed >> ((k + 7) % 31)) & 1) ? NULL : ip;
}
static u8 *anims[2]; static int nodes;
static u8 *mkanim(int nn, int ks, int isz) {
  u8 *info = allocr(0x20), *anim = allocr(0x18), *fr; int n, j, tot = 0, nf[16], idx = 0, i; float dur = rnd() % 4 ? uf(0.1f, 10) : (float)(1 + rnd() % 4), tm[16][9];
  struct { int n, j; float key; } ord[160]; int no = 0;
  *(int32_t *)(info + 4) = isz; *(int32_t *)(info + 8) = ks;
  *(void **)(info + 0x14) = (void *)icb2;
  for (n = 0; n < nn; n++) {
    nf[n] = 2 + rnd() % 7; tot += nf[n];
    tm[n][0] = 0;
    for (j = 1; j < nf[n]; j++) tm[n][j] = rnd() % 6 == 0 ? tm[n][j - 1] : uf(0, dur);
    for (j = 1; j < nf[n]; j++) { int k; for (k = j; k > 1 && tm[n][k] < tm[n][k - 1]; k--) { float x = tm[n][k]; tm[n][k] = tm[n][k - 1]; tm[n][k - 1] = x; } }      /* sorted (frame 1 too) */
    if (rnd() % 4) tm[n][nf[n] - 1] = dur;
  }
  for (n = 0; n < nn; n++) for (j = 1; j < nf[n]; j++) if (tm[n][j] < tm[n][j - 1]) tm[n][j] = tm[n][j - 1];
  fr = allocr(ks * tot);
  /* order: all frame 0, all frame 1, then the rest by (time of the previous frame, j) */
  for (n = 0; n < nn; n++) for (j = 2; j < nf[n]; j++) { ord[no].n = n; ord[no].j = j; ord[no].key = tm[n][j - 1]; no++; }
  for (i = 1; i < no; i++) { int k; for (k = i; k > 0 && (ord[k].key < ord[k - 1].key || (ord[k].key == ord[k - 1].key && ord[k].j < ord[k - 1].j)); k--) { int a = ord[k].n, b = ord[k].j; float c = ord[k].key; ord[k] = ord[k - 1]; ord[k - 1].n = a; ord[k - 1].j = b; ord[k - 1].key = c; } }
  { u8 *where[16][9];
    for (n = 0; n < nn; n++) { where[n][0] = fr + ks * idx; idx++; }
    for (n = 0; n < nn; n++) { where[n][1] = fr + ks * idx; idx++; }
    for (i = 0; i < no; i++) { where[ord[i].n][ord[i].j] = fr + ks * idx; idx++; }
    for (n = 0; n < nn; n++) for (j = 0; j < nf[n]; j++) { *(u8 **)where[n][j] = j ? where[n][j - 1] : NULL; *(float *)(where[n][j] + 4) = tm[n][j]; } }
  *(u8 **)anim = info; *(int32_t *)(anim + 4) = tot; *(float *)(anim + 0xc) = dur; *(u8 **)(anim + 0x10) = fr; *(u32 *)(anim + 0x14) = rnd();
  return anim;
}
static u8 *mkinterp(int nn, int isz, float dur) {
  u8 *ip = allocr(0x4c + nn * isz);
  *(int32_t *)(ip + 0x2c) = nn; *(int32_t *)(ip + 0x24) = isz;
  *(void **)(ip + 0xc) = rnd() % 4 ? (void *)acb : NULL; *(u32 *)(ip + 0x10) = rnd();
  *(float *)(ip + 0x14) = rnd() % 5 == 0 ? -1.0f : rnd() % 6 == 0 ? dur : uf(0, dur * 1.1f);
  *(void **)(ip + 0x18) = rnd() % 4 ? (void *)lcb : NULL; *(u32 *)(ip + 0x1c) = rnd();
  return ip;
}
static float rtime(float dur) {
  int k = rnd() % 12;
  if (k == 0) return 0; if (k == 1) return -uf(0, dur); if (k == 2) return dur; if (k == 3) return dur * (float)(1 + rnd() % 3);
  if (k == 4) return uf(0, 1e-4f); if (k <= 6) return uf(0, 0.3f) * dur;
  return uf(0, 3.5f) * dur;      /* finite: an infinite step makes both sides loop forever (t - dur == t) */
}

/* watchdog and crash report: a hang or a fault ends the run with a message instead of a stuck container */
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; unsigned char *eng; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 16u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); after0 = malloc(ASZ); lg[0] = malloc(4 << 16); lg[1] = malloc(4 << 16); if (!arena || !snap || !after0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  { static const char *g[][2] = {{"RwV3dNormalize", (char *)0x4d5290}, {"_rwSqrt", (char *)0x4d5410}, {"_rwInvSqrt", (char *)0x4d5470}, {"RwMatrixOptimize", (char *)0x4d3c00}}; int k;
    for (k = 0; k < 4; k++) { void *p = (void *)GetProcAddress(m, g[k][0]); if (!p) { printf("%s MISSING export (g0/g1)\n", g[k][0]); return 1; } jmp_patch((u32)(uintptr_t)g[k][1], p); } }
  jmp_patch(0x4ea160, (void *)errlog);
  eng = calloc(1, 0x10000); W(0x7d2128) = (u32)(uintptr_t)eng; W((u32)(uintptr_t)eng + 0x108) = (u32)(uintptr_t)hmalloc; W((u32)(uintptr_t)eng + 0x10c) = (u32)(uintptr_t)hfree;
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  ((void (__cdecl *)(u32, u32, u32))0x4d5770)((u32)(uintptr_t)eng, 0x400, 0x18);      /* math ctor: the sqrt / invsqrt tables */
  if (!((void *(__cdecl *)(void *, u32, u32))0x556bf0)(eng, 0, 0) || !W(0x7da7e8) || !W(0x7da7bc + 40)) { printf("SplineOpen failed\n"); return 2; }
  bind = (Bind)GetProcAddress(m, "mflrw_splanim_bind");
  if (!bind) { printf("mflrw_splanim_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)0x7da7bc, (void *)0x4ea160);
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: FindPosition / FindMatrix get path 10 <-> 11 swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0, badsp = 0, kind = T[fi].kind; long cov[8] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u8 *a0, *a1, *a2, *a3; float fw = 0; int32_t path = 0, ok, k; u32 in[8] = {0};
      cur_fi = fi; cur_case = i;
      special = i >= N; an = 0; logon = 0; cur_ip = NULL; failAt = 0;
      sscale = (float)ldexp(1.0, (int)(rnd() % 20) - 6);
      /* ---- build the case (arena only) */
      if (kind == K_NORM) {
        float *v = alloc(16), *o = alloc(16); int how = rnd() % 8; float s = (float)ldexp(1.0, (int)(rnd() % 160) - 80);
        for (k = 0; k < 3; k++) v[k] = uf(-1, 1) * s;
        if (how == 0) v[rnd() % 3] = 0; if (how == 1) v[0] = v[1] = v[2] = 0; if (how == 2) { v[0] = rnd() % 2 ? 1e19f : 1e-20f; }
        for (k = 0; k < 3; k++) { v[k] = spf(v[k]); o[k] = uf(-1, 1); }
        a0 = rnd() % 2 ? (u8 *)v : (u8 *)o; a1 = (u8 *)v;
      } else if (kind == K_FPOS || kind == K_FMAT) {
        int n = 4 + rnd() % 37, type = 1 + rnd() % 2; float *o = allocr(64), *up = alloc(16);
        a0 = mkspline(n, type, 1); fw = rwhere(n); path = rnd() % 60 ? 10 + rnd() % 2 : (int32_t)(rnd() % 13);
        for (k = 0; k < 3; k++) up[k] = spf(uf(-1, 1)); if (rnd() % 8 == 0) up[0] = up[1] = up[2] = 0;
        a1 = (u8 *)o; a2 = rnd() % 2 ? (u8 *)up : NULL; a3 = kind == K_FPOS ? (rnd() % 4 ? (u8 *)(o + 4) : NULL) : NULL;
        if (kind == K_FPOS) a2 = a3;
        cov[path == 10 || path == 11 ? 0 : 1]++;
      } else if (kind == K_CREATE) {
        int type = rnd() % 40 ? 1 + rnd() % 2 : (rnd() % 2 ? 0 : 3), nn = rnd() % 41, n; float *p;
        if (type == 2 && nn == 0) nn = 1 + rnd() % 3;
        n = nn + (type == 2 ? 3 : 2); a0 = mkspline(n, type, 0);
        p = alloc(12 * (nn + 2)); for (k = 0; k < 3 * nn; k++) p[k] = spf(uf(-1, 1) * sscale);
        if (rnd() % 8 == 0 && nn > 1) memcpy(p + 3, p, 12);
        a1 = (u8 *)p; failAt = rnd() % 8 == 0 ? 1 + rnd() % 3 : 0;
        cov[type == 1 ? 0 : type == 2 ? (nn < 10 ? (nn % 2 ? 1 : 2) : (3)) : 4]++;
      } else if (kind == K_KFI) {
        float *k1 = allocr(0x24), *k2 = allocr(0x24), *o = allocr(0x24); mkkfpair(k1, k2);
        fw = rnd() % 8 == 0 ? k1[1] : rnd() % 8 == 0 ? k2[1] : uf(k1[1], k2[1]); if (rnd() % 30 == 0) fw = uf(-5, 15); fw = spf(fw);
        a0 = (u8 *)o; a1 = (u8 *)k1; a2 = (u8 *)k2; a3 = (u8 *)(uintptr_t)rnd();
        { float d = k1[2] * k2[2] + k1[3] * k2[3] + k1[4] * k2[4] + k1[5] * k2[5]; cov[d < 0 ? 0 : 1]++; d = fabsf(d); cov[d >= 0.999f ? 2 : d < 0.5f ? 3 : 4]++; }
      } else {
        int nn = 1 + rnd() % 12, ks = rnd() % 3 ? 0x24 : 8 + 4 * (rnd() % 8), isz = rnd() % 3 ? 0x24 : 8 + 4 * (rnd() % 10), ops, o;
        anims[0] = mkanim(nn, ks, isz); anims[1] = mkanim(nn, ks, isz); nodes = nn;
        a0 = mkinterp(nn, isz, *(float *)(anims[0] + 0xc)); cur_ip = a0;
        cbseed = rnd(); cbcount = 0;
        /* prep with the original functions (not logged): Set + a few Add / Sub, unless testing Set on a random state */
        if (kind != K_SET || rnd() % 2) {
          ((FSet)0x53e3c0)(a0, anims[0]); ops = rnd() % 4;
          for (o = 0; o < ops; o++) { float t = rtime(*(float *)(anims[0] + 0xc)); if (t != t) t = 0; if (rnd() % 2) ((FTime)0x53e640)(a0, t); else ((FTime)0x53e4d0)(a0, t); }
        } else *(float *)(a0 + 4) = uf(0, 5);
        a1 = anims[rnd() % 2]; fw = rtime(*(float *)(anims[0] + 0xc));
        if ((kind == K_ADD || kind == K_SUB) && rnd() % 3 == 0) {      /* ties: the new time lands on a keyframe time / the callback time */
          float cur = *(float *)(a0 + 4), dur = *(float *)(anims[0] + 0xc), ft = *(float *)(*(u8 **)(anims[0] + 0x10) + (rnd() % *(int32_t *)(anims[0] + 4)) * ks + 4);
          int how = rnd() % 3;
          fw = kind == K_ADD ? ft - cur : cur - ft;
          if (how == 1) *(float *)(a0 + 0x14) = kind == K_ADD ? cur + fw : cur - fw;
          if (how == 2) { fw = kind == K_ADD ? uf(dur - cur, dur) : uf(0, cur + dur);      /* Add: wraps once, the callback time was not reached before */ *(float *)(a0 + 0x14) = kind == K_ADD ? (cur + fw) - dur : (cur - fw) + (cur - fw < 0 ? dur : 0); }
        }
        if (special && rnd() % 3 == 0) fw = bitsf(0x7fc00000);
      }
      memcpy(in, &fw, 4);
      memcpy(snap, arena, an);
      { u32 seed = cbseed;
      for (side = 0; side < 2; side++) {      /* both sides run on the same arena bytes (restored before each) */
        void *fn = side ? orig : dll; int32_t p2 = path;
        memcpy(arena, snap, an); ln[side] = 0; overflow = 0; nalloc = 0; cbcount = 0; cbseed = seed; logon = 1;
        if (neg && !side && (kind == K_FPOS || kind == K_FMAT) && (path == 10 || path == 11)) p2 = 21 - path;
        cw_set(0x7f);
        switch (kind) {
        case K_NORM: { long double r = ((FNorm)fn)((float *)a0, (float *)a1); u32 w[3] = {0}; memcpy(w, &r, 10); logw(w[0]); logw(w[1]); logw(w[2]); break; }
        case K_FPOS: { void *r = ((FPos)fn)(a0, p2, fw, (float *)a1, (float *)a2); logw((u32)(uintptr_t)r); break; }
        case K_FMAT: { long double r = ((FMat)fn)(a0, p2, fw, (float *)a2, (float *)a1); u32 w[3] = {0}; memcpy(w, &r, 10); logw(w[0]); logw(w[1]); logw(w[2]); break; }
        case K_CREATE: { void *r = ((FCreate)fn)(a0, (float *)a1); logw((u32)(uintptr_t)r); break; }
        case K_KFI: ((FKfi)fn)(a0, a1, a2, fw, a3); break;
        case K_SET: logw(((FSet)fn)(a0, a1)); break;
        default: logw(((FTime)fn)(a0, fw)); break;
        }
        logw(0xc0de0000u | cw_now()); cw_set(0x7f);
        if (overflow) logw(0x0f10f1);
        logon = 0;
        if (!side) memcpy(after0, arena, an);
      } }
      if (kind >= K_SET) { int a = 0, b = 0, c = 0; for (k = 0; k < ln[1]; k++) { a |= (lg[1][k] >> 20) == 0xacb; b |= (lg[1][k] >> 20) == 0x1cb; c |= (lg[1][k] >> 16) == 0x1c0b; } cov[0] += a; cov[1] += b; cov[2] += c; cov[3] += ln[1]; }
      ok = ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * ln[0]) && !memcmp(after0, arena, an);
      if (ok) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: %d / %d log words (dll / orig)", T[fi].name, i, ln[0], ln[1]);
        for (k = 0; k < ln[0] && k < ln[1]; k++) if (lg[0][k] != lg[1][k]) { printf(", first log diff at word %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("  where/time %08x path %d\n", in[0], path);
        if (kind == K_FMAT) { printf("    dll :"); for (k = 0; k < 16; k++) printf(" %08x", ((u32 *)after0)[k]); printf("\n    orig:"); for (k = 0; k < 16; k++) printf(" %08x", ((u32 *)arena)[k]); printf("\n    up %p\n", a2); }
      }
    }
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld %ld; %d of %d special cases differ\n", cov[0], cov[1], cov[2], cov[3], cov[4], badsp, N / 5); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
