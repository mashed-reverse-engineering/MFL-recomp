/* rw_uvanim_check.c: bit-exact check of the librw DLL's UV animation kernels (tools/delink/rw_uvanim.txt, librw-mfl capi/rwuvanim.cpp:
   _rpUVAnimCreateConsistantMatrices, RpMaterialUVAnimApplyUpdate, RpUVAnimParamKeyFrameInterpolate, RpUVAnimLinearKeyFrameInterpolate)
   against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.  Made from rw_image_check.c (same image mapping and
   harness heap).  The matrix callees (RwMatrixCreate 0x4d4e70 / Destroy 0x4d4e40 / Update 0x4d3cc0 / Translate 0x4d4870 / Rotate 0x4d43f0 /
   Transform 0x4d49c0) and RpMatFXMaterialSetUVTransformMatrices 0x550860 are jmp-patched to loggers: each logs its arguments (pointers into
   the arena as values, the local matrix and the rotation axis by content: only the words the original writes, its flags and pad words
   are uninitialised stack in the original) and mixes them into its output matrix, so a wrong order / argument shows in the log or the arena.
   The DLL reaches them through mflrw_uvanim_bind with the same exe addresses.  Each side rebuilds the case from the same seed (material,
   UV extension, 0..8 interpolators with random node -> channel maps, keyframe strides and apply callbacks among the exe's linear 0x562a90 /
   param 0x562620 and a harness callback); compared: the used arena incl. slack, the call log, the return value, the x87 control word.
   x87 CW 0x7f on even and 0x27f on odd cases (the angle product, the two interpolators).
   usage: rw_uvanim_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (restored after the call); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_CONSIST, K_APPLY, K_PINTERP, K_LINTERP };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37__rpUVAnimCreateConsistantMatrices", 0x562020, K_CONSIST}, {"rw37_RpMaterialUVAnimApplyUpdate", 0x562310, K_APPLY},
  {"rw37_RpUVAnimParamKeyFrameInterpolate", 0x5626c0, K_PINTERP}, {"rw37_RpUVAnimLinearKeyFrameInterpolate", 0x562ae0, K_LINTERP}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef void (__cdecl *FI)(u32, u32, u32, float);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *, void *);

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
#define LO 0x4b0000u
#define HI 0x94b000u
#define W(a) (*(u32 *)(uintptr_t)(a))
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

/* ---- harness heap + call log */
#define HSZ (4u << 20)
#define LGMAX (1u << 16)
static u8 *heap, *save0; static u32 hp, nalloc, failAt, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
/* the words of a local matrix the original writes (flags +0xc and the pads +0x1c / +0x2c / +0x3c are uninitialised stack) */
static const int mw[12] = {0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14};
static u32 mixv = 0;
static void logm(const u32 *m) { int k; for (k = 0; k < 12; k++) logw(m[mw[k]]); }
static void mixm(u32 *m, u32 salt) { int k; for (k = 0; k < 12; k++) { mixv = mixv * 0x9e3779b1u + m[mw[k]] + salt; m[mw[k]] = (m[mw[k]] ^ mixv) & 0xbfffffffu; } }      /* & keeps the floats finite */
static void *__cdecl hcreate(void) { logw(0xa2100000u); if (++nalloc == failAt) { logw(0); return 0; } { u32 *p = halloc(64); logw((u32)(uintptr_t)p); return p; } }
static int __cdecl hdestroy(u32 *m) { logw(0xa2200000u); logw((u32)(uintptr_t)m); return 1; }
static u32 *__cdecl hupdate(u32 *m) { logw(0xa2300000u); logm(m); m[3] &= 0xfffdfffcu; mixm(m, 1); return m; }
static u32 *__cdecl htranslate(u32 *m, const u32 *v, int op) { logw(0xa2400000u); logw((u32)(uintptr_t)v); logw(op); logm(m); mixm(m, v[0] ^ v[1] ^ v[2]); return m; }
static u32 *__cdecl hrotate(u32 *m, const u32 *ax, float angle, int op) { u32 a; memcpy(&a, &angle, 4); logw(0xa2500000u); logw(ax[0]); logw(ax[1]); logw(ax[2]); logw(a); logw(op); logm(m); mixm(m, a); return m; }
static u32 *__cdecl htransform(u32 *d, const u32 *t, int op) { int k; logw(0xa2600000u); logw((u32)(uintptr_t)d); logw(op); logm(t); for (k = 0; k < 16; k++) d[k] = d[k] * 3u + t[mw[k % 12]]; return d; }
static void *__cdecl hsetuv(void *mat, void *m0, void *m1) { logw(0xa2700000u); logw((u32)(uintptr_t)mat); logw((u32)(uintptr_t)m0); logw((u32)(uintptr_t)m1); return mat; }
static void __cdecl hcb(u32 *m, const u32 *kf) { int k; logw(0xa2800000u); logw((u32)(uintptr_t)kf); for (k = 0; k < 16; k++) m[k] = kf[k % 8] ^ (u32)k; }

static float rf(void) {
  u32 m = rnd() % 16; float f;
  if (m == 0) return 0.0f;
  if (m == 1) { static const u32 sp[6] = {0x7f800000u, 0xff800000u, 0x7fc00000u, 0x80000000u, 0x40490fdbu, 0xc0490fdbu}; memcpy(&f, &sp[rnd() % 6], 4); return f; }
  if (m == 2) { u32 b = rnd(); memcpy(&f, &b, 4); return f; }
  return ((float)(int)(rnd() % 200001) - 100000.0f) * (m < 8 ? 0.0001f : 0.01f);
}

static u8 *swapA, *swapB;
static void swapw(void) { u32 t; if (!swapA) return; memcpy(&t, swapA, 4); memcpy(swapA, swapB, 4); memcpy(swapB, &t, 4); }

#define MATOFF 0x40
static u32 run_case(int kind, void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r = 0, *ext, *mat, k, i; unsigned short cw = (seed & 1) ? 0x27f : 0x7f;
  rs = seed; hseed = seed * 2654435761u; hp = 0; nalloc = 0; failAt = 0; ln[side] = 0; swapA = swapB = 0; mixv = seed;
  cov[0] = cov[1] = 0;
  if (kind == K_PINTERP || kind == K_LINTERP) {
    u32 *out = halloc(32), *a = halloc(32), *b = halloc(32); float t;
    for (k = 0; k < 8; k++) { float x = rf(), y = rnd() % 4 ? x + rf() * 0.1f : rf(); memcpy(a + k, &x, 4); memcpy(b + k, &y, 4); }
    { float ta = (float)(rnd() % 100) * 0.03125f, tb = ta + (rnd() % 8 ? (float)(1 + rnd() % 50) * 0.03125f : rf()); memcpy(a + 1, &ta, 4); memcpy(b + 1, &tb, 4);
      t = rnd() % 8 ? ta + (tb - ta) * (float)(rnd() % 65) / 64.0f : rf(); }
    if (kind == K_PINTERP && rnd() % 2) { float x = (float)((int)(rnd() % 2001) - 1000) * 0.01f; memcpy(b + 2, &x, 4); }      /* angles around the +-pi wrap */
    swapA = (u8 *)(a + 2); swapB = (u8 *)(b + 2);
    cov[0] = (t != t); cov[1] = 0;
    cw_set(cw);
    if (neg && isdll) swapw();
    ((FI)fn)((u32)(uintptr_t)out, (u32)(uintptr_t)a, (u32)(uintptr_t)b, t);
    if (neg && isdll) swapw();
    logw(cw_now()); cw_set(0x27f);
    return 0;
  }
  mat = halloc(MATOFF + 0x30); ext = (u32 *)((u8 *)mat + MATOFF);
  for (k = 0; k < 2; k++) ext[k] = rnd() % 4 ? (u32)(uintptr_t)halloc(64) : 0;
  for (i = 0; i < 8; i++) {
    u32 *ip, *anim, *cd, nn, stride, *map;
    if (rnd() % 3 == 0) { ext[2 + i] = 0; continue; }
    nn = rnd() % 8 == 0 ? 0 : 1 + rnd() % 6; stride = rnd() % 2 ? 0x20 : 0x20 + 4 * (rnd() % 8);
    cd = halloc(0x20 + 4 * (nn + 1)); map = cd + 8;
    for (k = 0; k <= nn; k++) map[k] = rnd() % 8 == 0 ? 2 + rnd() % 3 : rnd() % 4 == 0 ? 0xffffffffu : rnd() % 2;
    anim = halloc(0x20); anim[5] = (u32)(uintptr_t)cd;
    ip = halloc(0x4c + nn * stride + 4); ip[0] = (u32)(uintptr_t)anim; ip[0x24 / 4] = stride; ip[0x2c / 4] = nn;
    k = rnd() % 8; ip[0x3c / 4] = k < 3 ? 0x562a90 : k < 6 ? 0x562620 : (u32)(uintptr_t)hcb;
    for (k = 0; k < nn * stride / 4; k++) { float x = rf(); memcpy(ip + 0x4c / 4 + k, &x, 4); }
    ext[2 + i] = (u32)(uintptr_t)ip;
  }
  W(0x947b58) = MATOFF;
  if (kind == K_CONSIST) {
    if (rnd() % 10 == 0) failAt = 1 + rnd() % 2;
    swapA = (u8 *)ext; swapB = (u8 *)(ext + 1);
    cov[0] = failAt != 0; cov[1] = ext[0] && ext[1];
    cw_set(cw);
    if (neg && isdll) swapw();
    r = ((F1)fn)((u32)(uintptr_t)ext);
    if (neg && isdll) swapw();
  } else {
    for (i = 0; i < 8; i++) if (ext[2 + i]) { u32 *ip = (u32 *)(uintptr_t)ext[2 + i]; if (ip[0x2c / 4]) { swapA = (u8 *)(ip + 0x4c / 4 + 2); swapB = swapA + 4; cov[0] |= ip[0x3c / 4] == 0x562620; cov[1] |= ip[0x3c / 4] == (u32)(uintptr_t)hcb; break; } }
    cw_set(cw);
    if (neg && isdll) swapw();
    r = ((F1)fn)((u32)(uintptr_t)mat);
    if (neg && isdll) swapw();
  }
  logw(cw_now()); cw_set(0x27f);
  return r;
}

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[8192]; int k; for (k = 0; k < 8192; k++) b[k] = 0; }
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  u32 *sp = (u32 *)(uintptr_t)e->ContextRecord->Esp; int k;
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx; stack", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  for (k = 0; k < 16; k++) printf(" %08x", sp[k]);
  printf("\n"); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ); lg[0] = malloc(4 * LGMAX); lg[1] = malloc(4 * LGMAX);
  if (!heap || !save0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  jmp_patch(0x4d4e70, (void *)hcreate); jmp_patch(0x4d4e40, (void *)hdestroy); jmp_patch(0x4d3cc0, (void *)hupdate); jmp_patch(0x4d4870, (void *)htranslate);
  jmp_patch(0x4d43f0, (void *)hrotate); jmp_patch(0x4d49c0, (void *)htransform); jmp_patch(0x550860, (void *)hsetuv);
  bind = (Bind)GetProcAddress(m, "mflrw_uvanim_bind");
  if (!bind) { printf("mflrw_uvanim_bind MISSING export\n"); return 1; }
  bind((void *)0x947b58, (void *)0x63a3c0, (void *)0x63a3f0, (void *)0x5f5d8c, (void *)0x5f5d98, (void *)0x4d4e70, (void *)0x4d4e40, (void *)0x4d3cc0, (void *)0x4d4870,
       (void *)0x4d43f0, (void *)0x4d49c0, (void *)0x550860);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, kind = T[fi].kind; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-44s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        ret[side] = run_case(kind, side ? orig : dll, !side, neg, seed, c);
        if (!side) { hp0 = hp; memcpy(save0, heap, hp + 16); }
      }
      cov[0] += c[0]; cov[1] += c[1];
      if (ret[0] == ret[1] && hp0 == hp && ln[0] == ln[1] && !memcmp(lg[0], lg[1], 4 * (ln[0] < LGMAX ? ln[0] : LGMAX)) && !memcmp(save0, heap, hp + 16)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap %u / %u, log %u / %u", T[fi].name, i, ret[0], ret[1], hp0, hp, ln[0], ln[1]);
        for (k = 0; k < (int)(ln[0] < ln[1] ? ln[0] : ln[1]) && k < (int)LGMAX; k++) if (lg[0][k] != lg[1][k]) { printf(", first log diff at %d: dll %08x orig %08x", k, lg[0][k], lg[1][k]); break; }
        for (k = 0; k + 4 <= (int)(hp0 < hp ? hp0 : hp) + 16; k += 4) if (*(u32 *)(save0 + k) != *(u32 *)(heap + k)) { printf(", first heap diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(save0 + k), *(u32 *)(heap + k)); break; }
        printf("\n");
      }
    }
    printf("%-44s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
