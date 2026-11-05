/* rw_rwleaf2_check.c: bit-exact check of the librw DLL's leafs of group rwleaf2 (tools/delink/rw_rwleaf2.txt, librw-mfl capi/rwleaf2.cpp:
   Rt2dBrushSetRGBA 0x564b10, BMPImageSetSpan 0x53d6c0, PVSCalculateSectorBounds 0x548920, RpLightGetConeAngle 0x4f4a70, FUN_00542fe0
   (keyframe -> matrix)) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_rwleaf_check.c (same
   image mapping and harness arena).  The DLL is bound (mflrw_rwleaf2_bind) to the exe's RwErrorSet 0x4ea160 (jmp-patched to a logger that
   writes its two words into the arena), _rwSqrt 0x4d5410 and RwV3dLength 0x4d53a0 (original code; their sqrt table hangs off a harness engine
   block: RwEngineInstance 0x7d2128, table offset 0x7d2158) and the PVS plugin offset 0x947ae8 (set per case).  Each side rebuilds the case from
   the same seed; compared: the used arena incl. slack, the return value (RpLightGetConeAngle: the 80-bit st0), the x87 control word (CW 0x7f on
   even and 0x27f on odd cases).  Inputs: brush colours equal / one byte apart / random; BMP rows for every depth / bpp key (and invalid ones),
   widths 0..40, both 24-bit orders; sector trees of depth 0..4 with plane types 0 / 4 / 8 / other, boxes with inf / NaN / sNaN; cone cosines
   around 0, +-0.5, +-1, tiny, inf / NaN / random bits; keyframes with unit / random / special quaternions.
   usage: rw_rwleaf2_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets a changed input; every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_BRUSH, K_BMP, K_PVS, K_CONE, K_HANIM };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_Rt2dBrushSetRGBA", 0x564b10, K_BRUSH}, {"rw37_BMPImageSetSpan", 0x53d6c0, K_BMP}, {"rw37_PVSCalculateSectorBounds", 0x548920, K_PVS},
  {"rw37_RpLightGetConeAngle", 0x4f4a70, K_CONE}, {"rw37_HAnimKeyFrameToMatrix", 0x542fe0, K_HANIM}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F2)(u32, u32);
typedef u32 (__cdecl *F5)(u32, u32, u32, u32, u32);
typedef long double (__cdecl *FX)(u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
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
static void cw_set(unsigned short cw) { __asm__ volatile("fldcw %0" : : "m"(cw)); }
static unsigned short cw_now(void) { unsigned short cw; __asm__ volatile("fnstcw %0" : "=m"(cw)); return cw; }
#define W32(va) (*(u32 *)(uintptr_t)(va))

#define HSZ (1u << 20)
static u8 *heap, *save0; static u32 hp, hseed;
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static u32 *elog;      /* RwErrorSet logger: count, then the words of each error */
static void *__cdecl herr(int32_t *e) { u32 n = elog[0]++; if (n < 4) { elog[1 + 2 * n] = e[0]; elog[2 + 2 * n] = e[1]; } return e; }
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float rf(void) {
  u32 m = rnd() % 16;
  if (m == 0) return 0.0f;
  if (m == 1) { static const u32 sp[7] = {0x7f800000u, 0xff800000u, 0x7fc00000u, 0x7fa00000u, 0x80000000u, 0x00000001u, 0x7f7fffffu}; return bitsf(sp[rnd() % 7]); }
  if (m == 2) return bitsf(rnd());
  return ((float)(int)(rnd() % 20001) - 10000.0f) * (m < 8 ? 0.0001f : 0.37f);
}
static void putf(void *p, float f) { memcpy(p, &f, 4); }
static u32 eng[64], sqtab[4096];

/* PVS tree: plane sector {type, value, left, right}, atomic sector {type < 0, ..., bbox +0x48..+0x5c, PVS data at the plugin offset} */
static u32 pvsoff;
static u32 mktree(int depth) {
  if (depth == 0 || rnd() % 4 == 0) {
    u8 *s = halloc(pvsoff + 0x50); int32_t t = rnd() % 4 ? -1 : -(int32_t)(1 + rnd() % 1000); int k;
    memcpy(s, &t, 4); for (k = 0; k < 6; k++) putf(s + 0x48 + 4 * k, rf());
    return (u32)(uintptr_t)s;
  } else {
    u32 *s = halloc(16); static const int32_t ty[6] = {0, 4, 8, 0, 4, 12};
    s[0] = rnd() % 8 ? (u32)ty[rnd() % 6] : rnd() % 2 ? 1 : 0x7fffffff; putf(s + 1, rf());
    s[2] = mktree(depth - 1); s[3] = mktree(depth - 1);
    return (u32)(uintptr_t)s;
  }
}

static u32 cwlog[2]; static u8 xret[2][10];
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, k; unsigned short cw = (seed & 1) ? 0x27f : 0x7f; int dneg = neg && isdll;
  rs = seed; cov[0] = cov[1] = 0;
  elog = halloc(40); memset(elog, 0, 40);
  cw_set(cw);
  switch (kind) {
  case K_BRUSH: {
    u8 *b = halloc(0x68), *c[4]; int how = rnd() % 3;
    for (k = 0; k < 4; k++) { c[k] = halloc(4); if (how == 0 && k) memcpy(c[k], c[0], 4); }
    if (how == 0 && rnd() % 2) c[rnd() % 4][rnd() % 4] ^= (u8)(1 << (rnd() % 8));
    if (dneg) { u8 *t = c[0]; c[0] = c[1]; c[1] = t; if (!memcmp(c[0], c[1], 4)) c[1][0] ^= 1; }
    cov[0] = how == 0;
    r = ((F5)fn)((u32)(uintptr_t)b, (u32)(uintptr_t)c[0], (u32)(uintptr_t)c[1], (u32)(uintptr_t)c[2], (u32)(uintptr_t)c[3]);
    break; }
  case K_BMP: {
    static const int32_t dep[4] = {4, 8, 32, 32}, bp[8] = {1, 4, 8, 15, 16, 24, 32, 32};
    u32 *im = halloc(28), w = rnd() % 8 == 0 ? 0 : rnd() % 41, y = rnd() % 3, stride, bgr = rnd() % 4 ? rnd() % 2 : rnd();
    int32_t depth = rnd() % 16 ? dep[rnd() % 4] : (int32_t)(rnd() % 40), bpp = rnd() % 16 ? bp[rnd() % 8] : (int32_t)(rnd() % 40);
    u8 *src;
    if (bpp == 8 && depth != 8 && depth != 32 && rnd() % 2) depth = 8;
    stride = w * 4 + 4 * (rnd() % 3);
    im[0] = rnd(); im[1] = w; im[2] = y + 1; im[3] = depth; im[4] = stride; im[5] = (u32)(uintptr_t)halloc(stride * (y + 1) + 4); im[6] = (u32)(uintptr_t)halloc(1024);
    src = halloc(w * 4 + 8);
    if (dneg) { if (w) { src[0] ^= 0x5a; src[1] ^= 0xa5; } else bpp ^= 0x40; }
    cov[0] = ((u32)depth | (u32)bpp << 8) == 0x1820; cov[1] = elog[0] == 0;
    r = ((F5)fn)((u32)(uintptr_t)im, y, (u32)(uintptr_t)src, (u32)bpp, bgr);
    cov[1] = elog[0] != 0;
    break; }
  case K_PVS: {
    u32 root; float *bb = halloc(24);
    pvsoff = 0x60 + 4 * (rnd() % 8); W32(0x947ae8) = pvsoff;
    for (k = 0; k < 6; k++) bb[k] = rf();
    root = mktree(rnd() % 5);
    if (dneg) { u32 t; memcpy(&t, bb, 4); memcpy(bb, bb + 3, 4); memcpy(bb + 3, &t, 4); if (!memcmp(bb, bb + 3, 4)) ((u8 *)bb)[0] ^= 1; }
    cov[0] = *(int32_t *)(uintptr_t)root >= 0;
    r = ((F2)fn)(root, (u32)(uintptr_t)bb);
    break; }
  case K_CONE: {
    u8 *l = halloc(0x30); u32 m = rnd() % 12, b; float x; long double v;
    if (m == 0) x = rf();
    else if (m == 1) { static const u32 sp[10] = {0x3f000000u, 0xbf000000u, 0x3effffffu, 0xbeffffffu, 0x3f800000u, 0xbf800000u, 0x23000000u, 0xa3000000u, 0x23000001u, 0x3f7fffffu}; x = bitsf(sp[rnd() % 10]); }
    else if (m == 2) { b = (rnd() % 0x23800000u) | (rnd() % 2 ? 0x80000000u : 0); x = bitsf(b); }
    else if (m < 6) x = (float)((int)(rnd() % 2000001) - 1000000) * 1e-6f;
    else if (m < 9) x = ((float)(rnd() % 1000001) * 0.5e-6f + 0.5f) * (rnd() % 2 ? 1 : -1);
    else x = (float)((int)(rnd() % 1000001) - 500000) * 1e-6f;
    putf(l + 0x28, x);
    if (dneg) l[0x28] ^= 1;
    cov[0] = x < -0.5f || x > 0.5f; cov[1] = x != x;
    v = ((FX)fn)((u32)(uintptr_t)l);
    memcpy(xret[!isdll], &v, 10);
    break; }
  default: {
    float *kf = halloc(0x24), *mm = halloc(0x40); int how = rnd() % 4;
    for (k = 0; k < 9; k++) kf[k] = rf();
    if (how) { float s = 0; for (k = 2; k < 6; k++) { kf[k] = (float)((int)(rnd() % 20001) - 10000) * 1e-4f; s += kf[k] * kf[k]; } if (how == 1 && s > 0) for (k = 2; k < 6; k++) kf[k] /= s; }
    if (dneg) { float t = kf[2]; kf[2] = kf[3]; kf[3] = t; if (!memcmp(kf + 2, kf + 3, 4)) ((u8 *)kf)[8] ^= 1; }
    cov[0] = how != 0;
    ((F2)fn)((u32)(uintptr_t)mm, (u32)(uintptr_t)kf);
    break; }
  }
  cwlog[!isdll] = cw_now(); cw_set(0x27f);
  return r;
}

static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind; u32 k;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ);
  if (!heap || !save0) { printf("no memory\n"); return 2; }
  rs = 777; for (k = 0; k < 4096; k++) sqtab[k] = rnd() & 0x007fffffu;
  eng[0x10] = (u32)(uintptr_t)sqtab; W32(0x7d2128) = (u32)(uintptr_t)eng; W32(0x7d2158) = 0x40;
  jmp_patch(0x4ea160, (void *)herr);
  bind = (Bind)GetProcAddress(m, "mflrw_rwleaf2_bind");
  if (!bind) { printf("mflrw_rwleaf2_bind MISSING export\n"); return 1; }
  bind((void *)0x4ea160, (void *)0x4d5410, (void *)0x4d53a0, (void *)0x947ae8);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets a changed input" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, bad = 0; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-44s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      memset(xret, 0, sizeof xret);
      for (side = 0; side < 2; side++) {
        hseed = seed * 2654435761u; hp = 0;
        ret[side] = run_case(T[fi].kind, side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (ret[0] == ret[1] && hp0 == hp && cwlog[0] == cwlog[1] && !memcmp(xret[0], xret[1], 10) && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        int k; printf("  DIFF %s case %d: ret dll %08x orig %08x, cw %04x / %04x", T[fi].name, i, ret[0], ret[1], cwlog[0], cwlog[1]);
        if (T[fi].kind == K_CONE) { printf(", st0 dll "); for (k = 9; k >= 0; k--) printf("%02x", xret[0][k]); printf(" orig "); for (k = 9; k >= 0; k--) printf("%02x", xret[1][k]); }
        for (k = 0; k + 4 <= (int)hp + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-44s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
