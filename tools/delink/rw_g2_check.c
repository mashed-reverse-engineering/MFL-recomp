/* rw_g2_check.c: bit-exact check of the librw DLL's S4 g2 exports (tools/delink/rw_g2.txt) against the ORIGINAL code of MFL.exe, both run natively (Wine) on the same CPU.
   usage: rw_g2_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name]   (build/run: tools/delink/rw_g2_check.sh; made from rw_g1_check.c)
   The exe's sections are copied to their own addresses (see load_image), so the original functions run in place with their internal calls; nothing of the game is initialised except
   what the functions read: the engine instance pointer 0x7d2128 -> a zeroed block with the matrix module data (free list pointer at +0x200) and memory functions (+0x118 / +0x11c free list
   alloc / free = a calloc stand-in here), the _rwSqrt/_rwInvSqrt tables (the original ctor 0x4d5770 builds them).  RwError helpers return at once.
   Per function N finite cases (x87 CW 0x7f = the game's 24 bit precision, MXCSR 0x9fc0) + N/5 special cases (inf/NaN/random bits; reported, not counted).  Compares the result (st0 bits, eax or which
   buffer is returned), all 8 x 64 byte buffers the pointer arguments point into (pointers may alias) and the control word after the call.  Exit 1 on any finite DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32;
typedef u32 (__cdecl *FI0)(void); typedef u32 (__cdecl *FI1)(u32); typedef u32 (__cdecl *FI2)(u32, u32); typedef u32 (__cdecl *FI3)(u32, u32, u32); typedef u32 (__cdecl *FI4)(u32, u32, u32, u32);
typedef u32 (__cdecl *FI5)(u32, u32, u32, u32, u32); typedef u32 (__cdecl *FI6)(u32, u32, u32, u32, u32, u32); typedef float (__cdecl *FF1)(u32);
/* arg kinds: p vector buffer, s sphere (radius >= 0 mostly), b bbox {sup, inf} (sup >= inf per axis mostly, sometimes shrunk).  result: f st0, i eax */
static const struct { const char *name; u32 va; const char *args; char res; } T[] = {
  {"_rwInvSqrt", 0x4d5470, "f", 'f'},      /* g0 function the sphere test calls */
  {"RtIntersectionSphereTriangle", 0x557a80, "sppppp", 'i'}, {"RtIntersectionBBoxTriangle", 0x558220, "bppp", 'i'}};
#define NB 8
static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static double ud(void) { return (rnd() & 0xffffff) / 16777216.0; }
static u32 fb(float f) { u32 b; memcpy(&b, &f, 4); return b; }
static u32 rf(int special) {      /* one float as bits */
  u32 r = rnd() % 100;
  if (special && r < 8) { static const u32 s[] = {0x7f800000, 0xff800000, 0x7fc00000, 0x7f800001, 0x00000001, 0x007fffff, 0x7f7fffff}; return s[rnd() % 7]; }
  if (special && r < 14) return rnd();
  if (r < 3) return 0; if (r < 4) return 0x80000000; if (r < 6) return 0x3f800000; if (r < 7) return 0xbf800000;
  if (r < 55) return fb((float)(ud() * 2 - 1));      /* geometry scale: hits, near-parallel cases */
  { float f = (float)(ud() * ldexp(1.0, (int)(rnd() % 25) - 12)); return fb((rnd() & 1) ? -f : f); }
}
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static void *__cdecl hmalloc(size_t n, u32 hint) { (void)hint; return calloc(1, n + 64); }
static void __cdecl hfree(void *p) { free(p); }
static void *__cdecl hflalloc(void *fl, u32 hint) { (void)fl; (void)hint; return calloc(1, 64); }      /* engine +0x118 / +0x11c: RwFreeListAlloc / Free */
static void __cdecl hflfree(void *fl, void *p) { (void)fl; free(p); }
static u32 call(void *fn, const char *ak, u32 *a, char rk) {
  u32 got = 0; int n = (int)strlen(ak);
  if (rk == 'f') { float r = ((FF1)fn)(a[0]); memcpy(&got, &r, 4); return got; }
  switch (n) { case 0: got = ((FI0)fn)(); break; case 1: got = ((FI1)fn)(a[0]); break; case 2: got = ((FI2)fn)(a[0], a[1]); break; case 3: got = ((FI3)fn)(a[0], a[1], a[2]); break;
               case 4: got = ((FI4)fn)(a[0], a[1], a[2], a[3]); break; case 5: got = ((FI5)fn)(a[0], a[1], a[2], a[3], a[4]); break; default: got = ((FI6)fn)(a[0], a[1], a[2], a[3], a[4], a[5]); }
  return got;
}
/* Wine puts its heaps / stacks right above a relocated main exe, so this exe is linked AT 0x400000 (MFL.exe's base) and carries a bss block that spans 0x4b0000..0x94b000: MFL.exe's sections are copied
   into that block (the RW code, its .rdata constants and .data/.bss globals all lie in it); this harness's own code sits below 0x4b0000. */
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
int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0; const char *only = argc > 4 ? argv[4] : 0; unsigned char *eng;
  static unsigned char buf[NB][64] __attribute__((aligned(64))), in[NB][64], o1[NB][64];
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  *(unsigned char *)0x4ea160 = 0xc3; memcpy((void *)0x53eac0, "\x31\xc0\xc3", 3);      /* RwError helpers of the error paths */
  eng = calloc(1, 0x10000); W(0x7d2128) = (u32)(uintptr_t)eng; W((u32)(uintptr_t)eng + 0x108) = (u32)(uintptr_t)hmalloc; W((u32)(uintptr_t)eng + 0x10c) = (u32)(uintptr_t)hfree;
  W((u32)(uintptr_t)eng + 0x118) = (u32)(uintptr_t)hflalloc; W((u32)(uintptr_t)eng + 0x11c) = (u32)(uintptr_t)hflfree;
  W(0x7d2150) = 0x200;
  cw_set(0x7f); { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }      /* the game sets CW 0x7f in its plugin attach (0x5dc404), before the engine ctors run */
  ((FI3)0x4d5770)((u32)(uintptr_t)eng, 0x400, 0x18);      /* math ctor: sqrt / invsqrt tables (the inverse table depends on the precision control) */
  printf("control word 0x%x, exe mapped, DLL %s\n", cw_now(), argv[1]);
  /* RwMatrixCreate / RwMatrixDestroy: N create (all words but the pads 0x1c/0x2c/0x3c) + destroy (result) pairs */
  if (!only || strstr(only, "RwMatrix")) {
    FI0 dc = (FI0)GetProcAddress(m, "RwMatrixCreate"); FI1 dd = (FI1)GetProcAddress(m, "RwMatrixDestroy"); int i, k, bad = 0;
    if (!dc || !dd) { printf("RwMatrixCreate/RwMatrixDestroy MISSING export\n"); bad_total++; }
    else {
      for (i = 0; i < N; i++) {
        u32 *p = (u32 *)(uintptr_t)dc(), *q = (u32 *)(uintptr_t)((FI0)0x4d4e70)();
        if (!p || !q) { bad++; continue; }
        for (k = 0; k < 16; k++) if ((k & 3) != 3 || k == 3) if (p[k] != q[k]) { bad++; break; }
        if (dd((u32)(uintptr_t)p) != ((FI1)0x4d4e40)((u32)(uintptr_t)q)) bad++;
        if (cw_now() != 0x7f) { bad++; cw_set(0x7f); }
      }
      printf("%-34s %7d create + destroy pairs: %s\n", "RwMatrixCreate/RwMatrixDestroy", N, bad ? "DIFF" : "EQUAL"); bad_total += bad != 0;
    }
  }
  for (fi = 0; fi < (int)(sizeof T / sizeof T[0]); fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; const char *ak = T[fi].args; int nargs = (int)strlen(ak), i, k, bad = 0, badsp = 0, nz = 0, nal = 0;
    if (only && !strstr(only, T[fi].name)) continue;
    if (!dll) { printf("%-34s MISSING export\n", T[fi].name); bad_total++; continue; }
    for (i = 0; i < N + N / 5; i++) {
      int sp = i >= N, bi[6], used = 0; u32 a[6], r1, r2; unsigned short c1, c2; int ok;
      for (k = 0; k < NB; k++) { u32 *w = (u32 *)in[k]; int j; for (j = 0; j < 16; j++) w[j] = rf(sp); }
      for (k = 0; k < nargs; k++) {
        char c = ak[k];
        if (c == 'f') { a[k] = rf(sp); continue; }
        bi[k] = (used && rnd() % 6 == 0) ? bi[rnd() % k] : k; if (bi[k] != k) nal++; used = 1;
        a[k] = (u32)(uintptr_t)buf[bi[k]];
        if (bi[k] != k || sp) continue;
        if (c == 's' && rnd() % 8) { float *w = (float *)in[k]; w[3] = fabsf(w[3]) * (rnd() % 2 ? 1.0f : 0.25f); }
        if (c == 's' && rnd() % 2) {      /* centre near the triangle in[1..3] (barycentric weights may leave it, so edge / vertex regions are hit), small radius */
          float *w = (float *)in[k], *p = (float *)in[1], *q = (float *)in[2], *r = (float *)in[3]; float u = (float)ud() * 2 - 0.5f, v = (float)ud() * 2 - 0.5f; int j;
          for (j = 0; j < 3; j++) w[j] = p[j] + u * (q[j] - p[j]) + v * (r[j] - p[j]) + ((float)ud() - 0.5f) * 0.2f;
          w[3] = (float)ud() * 0.6f;
        }
        if (c == 'b') {      /* sup >= inf on most axes; sometimes a small box so that all three vertices are outside */
          float *w = (float *)in[k]; int j; float sc = rnd() % 2 ? 1.0f : (float)ud() * 0.5f;
          for (j = 0; j < 3; j++) { float s = w[j], t = w[j + 3]; if (rnd() % 8 && s < t) { w[j] = t; w[j + 3] = s; } w[j] *= sc; w[j + 3] *= sc; }
        }
      }
      /* near-parallel pairs (slerp lerp branch, degenerate triangles): sometimes the 2nd buffer = the 1st, perturbed or negated */
      if (nargs >= 2 && rnd() % 4 == 0) { u32 *x = (u32 *)in[0], *y = (u32 *)in[1]; int j, neg = rnd() % 2; for (j = 0; j < 16; j++) y[j] = (x[j] + (rnd() % 5) - 2) ^ (neg ? 0x80000000u : 0); }
      for (int pass = 0; pass < 2; pass++) {
        u32 r; unsigned short c;
        memcpy(buf, in, sizeof buf); cw_set(0x7f);
        r = call(pass ? orig : dll, ak, a, T[fi].res); c = cw_now(); cw_set(0x7f);
        if (T[fi].res == 'p') { u32 g = r == 0 ? 0 : 0xffffffffu; for (k = 0; k < NB; k++) if (r == (u32)(uintptr_t)buf[k]) g = k + 1; r = g; }
        if (T[fi].res == 'v') r = 0;
        if (pass == 0) { r1 = r; c1 = c; memcpy(o1, buf, sizeof buf); } else { r2 = r; c2 = c; }
      }
      ok = r1 == r2 && c1 == c2 && !memcmp(o1, buf, sizeof buf);
      if (!sp && T[fi].res == 'i' && r2) nz++;
      if (ok) continue;
      if (sp) { badsp++; continue; }
      if (++bad <= 3) {
        int b, o; printf("  DIFF %s case %d: result dll %08x orig %08x, cw %x/%x", T[fi].name, i, r1, r2, c1, c2);
        for (b = 0; b < NB; b++) for (o = 0; o < 64; o += 4) if (memcmp(o1[b] + o, buf[b] + o, 4)) { printf(", buf %d +0x%x dll %08x orig %08x", b, o, *(u32 *)(o1[b] + o), *(u32 *)(buf[b] + o)); b = NB; break; }
        printf("  args"); for (k = 0; k < nargs; k++) printf(" %c=%08x", ak[k], strchr("psb", ak[k]) ? (u32)bi[k] : a[k]); printf("\n");
      }
    }
    printf("%-34s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    if (T[fi].res == 'i') printf(", %d nonzero results", nz);
    printf("; %d aliased args; %d of %d special cases differ\n", nal, badsp, N / 5); bad_total += bad != 0;
  }
  printf("DONE\n");      /* the .sh requires this line: a crash in the middle must not pass */
  return bad_total != 0;
}
