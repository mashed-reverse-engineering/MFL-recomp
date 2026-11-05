/* rw_camsync_check.c: bit-exact check of the librw DLL's camera sync (tools/delink/rw_camsync.txt, librw-mfl capi/rwcamsync.cpp: CameraSync
   0x4d1f70, the frame sync callback RwCameraCreate sets at camera +0x10) against the ORIGINAL code of MFL.exe, both run natively (Wine) on
   the same CPU.  Made from rw_uvanim_check.c (same image mapping and harness heap).  The callees RwMatrixOptimize 0x4d3c00,
   CameraBuildPerspClipPlanes 0x4d2110, CameraBuildParallelViewMatrix 0x4d2bc0, CameraBuildParallelClipPlanes 0x4d27b0 and RwBBoxCalculate
   0x4d18d0 are jmp-patched to loggers that log their arguments and the camera's view matrix and mix them into the camera, so a wrong order /
   argument shows in the log or the arena.  The DLL reaches them through mflrw_camsync_bind with the same exe addresses.  Each side rebuilds the
   case from the same seed (camera + frame LTM in the arena, random floats incl. inf / NaN / -0, projection perspective / parallel / other);
   compared: the used arena incl. slack, the call log, the return value, the x87 control word.  x87 CW 0x7f on even and 0x27f on odd cases.
   usage: rw_camsync_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg]
   neg = negative fixture (negtest.sh): the DLL side gets two input words swapped (restored after the call); every entry must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
static const struct { const char *name; u32 va; } T[] = {{"rw37_CameraSync", 0x4d1f70}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *F1)(u32);
typedef void (__cdecl *Bind)(void *, void *, void *, void *, void *);

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

/* ---- harness heap + call log */
#define HSZ (1u << 20)
#define LGMAX (1u << 12)
static u8 *heap, *save0; static u32 hp, hseed;
static u32 *lg[2]; static u32 ln[2]; static int side;
static void logw(u32 w) { if (ln[side] < LGMAX) lg[side][ln[side]] = w; ln[side]++; }
static void fillpat(u8 *p, u32 n, u32 off) { u32 x = hseed ^ (off * 0x9e3779b1u) ^ 0x5bd1e995u, k; for (k = 0; k < n; k++) { x ^= x << 13; x ^= x >> 17; x ^= x << 5; p[k] = (u8)x; } }
static void *halloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 3) & ~3u;
  if (n > HSZ || hp + sz > HSZ) { printf("harness heap exhausted\n"); ExitProcess(5); }
  p = heap + hp; fillpat(p, n + 16, hp); hp += sz; return p;
}
static u32 *cam_;
static void logcam(u32 tag, const void *a, const void *b, u32 c) { int k; logw(tag); logw((u32)(uintptr_t)a); logw((u32)(uintptr_t)b); logw(c); for (k = 0; k < 16; k++) logw(cam_[8 + k]); cam_[0x94 / 4 + (tag & 7)] ^= tag + ln[side]; }
static void *__cdecl hoptimize(void *m, const void *tol) { logcam(0xa3100001u, m, tol, 0); return m; }
static void *__cdecl hpersp(void *c) { logcam(0xa3200002u, c, 0, 0); return c; }
static void *__cdecl hparview(void *c) { logcam(0xa3300003u, c, 0, 0); return c; }
static void *__cdecl hparclip(void *c) { logcam(0xa3400004u, c, 0, 0); return c; }
static void *__cdecl hbbox(void *bb, const void *v, int n) { logcam(0xa3500005u, bb, v, n); return bb; }

static float rf(void) {
  u32 m = rnd() % 16; float f;
  if (m == 0) return 0.0f;
  if (m == 1 && rnd() % 8 == 0) { static const u32 sp[5] = {0x7f800000u, 0xff800000u, 0x7fc00000u, 0x80000000u, 0x00000001u}; memcpy(&f, &sp[rnd() % 5], 4); return f; }
  if (m == 2 && rnd() % 4 == 0) { u32 b = rnd(); memcpy(&f, &b, 4); return f; }
  return ((float)(int)(rnd() % 200001) - 100000.0f) * (m < 8 ? 0.00001f : 0.01f);
}

static u8 *swapA, *swapB;
static void swapw(void) { u32 t; if (!swapA) return; memcpy(&t, swapA, 4); memcpy(swapA, swapB, 4); memcpy(swapB, &t, 4); }

static u32 run_case(void *fn, int isdll, int neg, u32 seed, int *cov) {
  u32 r, *cam, *frm, k; unsigned short cw = (seed & 1) ? 0x27f : 0x7f; float f;
  rs = seed; hseed = seed * 2654435761u; hp = 0; ln[side] = 0;
  cam = halloc(0x190); frm = halloc(0x90); cam_ = cam;
  for (k = 0; k < 16; k++) { f = rf(); memcpy(frm + 0x50 / 4 + k, &f, 4); }
  cam[1] = (u32)(uintptr_t)frm; cam[5] = rnd() % 8 ? 1 : rnd() % 2 ? 2 : rnd();
  for (k = 0x68 / 4; k < 0x8c / 4; k++) { f = rf(); memcpy(cam + k, &f, 4); }
  swapA = (u8 *)(cam + 0x70 / 4); swapB = (u8 *)(cam + 0x74 / 4);
  cov[0] = cam[5] == 1; cov[1] = 0;
  cw_set(cw);
  if (neg && isdll) swapw();
  r = ((F1)fn)((u32)(uintptr_t)cam);
  if (neg && isdll) swapw();
  logw(cw_now()); cw_set(0x27f);
  return r;
}

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[8192]; int k; for (k = 0; k < 8192; k++) b[k] = 0; }
static volatile int cur_fi, cur_case;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  heap = VirtualAlloc(NULL, HSZ + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); save0 = malloc(HSZ); lg[0] = malloc(4 * LGMAX); lg[1] = malloc(4 * LGMAX);
  if (!heap || !save0 || !lg[0] || !lg[1]) { printf("no memory\n"); return 2; }
  jmp_patch(0x4d3c00, (void *)hoptimize); jmp_patch(0x4d2110, (void *)hpersp); jmp_patch(0x4d2bc0, (void *)hparview); jmp_patch(0x4d27b0, (void *)hparclip); jmp_patch(0x4d18d0, (void *)hbbox);
  bind = (Bind)GetProcAddress(m, "mflrw_camsync_bind");
  if (!bind) { printf("mflrw_camsync_bind MISSING export\n"); return 1; }
  bind((void *)0x4d3c00, (void *)0x4d2110, (void *)0x4d2bc0, (void *)0x4d27b0, (void *)0x4d18d0);
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL gets two input words swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0; long cov[2] = {0}; u32 hp0 = 0, ret[2];
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-40s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N; i++) {
      u32 seed = 0x9e3779b9u * (u32)(i + 1) + 7919u * (u32)fi; int c[2] = {0, 0};
      cur_fi = fi; cur_case = i;
      for (side = 0; side < 2; side++) {
        stackwipe();
        ret[side] = run_case(side ? orig : dll, !side, neg, seed, c);
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
    printf("%-40s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld\n", cov[0], cov[1]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
