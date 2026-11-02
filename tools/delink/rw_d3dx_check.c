/* rw_d3dx_check.c: bit-exact check of the librw DLL's D3DX math exports against the ORIGINAL bytes of MFL.exe, both run natively (Wine) on the same CPU, so rcpps agrees.
   usage: rw_d3dx_check.exe <mflrw.dll> <MFL.exe> [N=100000].  The original function bytes are copied from the exe file (file offset = VA - 0x400000 in .text) into executable memory; they are position independent.
   Per function N random cases (mixed magnitudes, zeros, denormals, aligned and unaligned buffers, out aliasing an input) under MXCSR 0x9fc0 (FTZ|DAZ, as the game) and under 0x1f80; then specials (inf/NaN/random bits). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <xmmintrin.h>
typedef uint32_t u32;
typedef void *(__stdcall *F3)(void *, const void *, const void *); typedef void *(__stdcall *F2)(void *, const void *);
static struct { const char *name; u32 va, len; int nargs; int alias_ok; } T[] = {
  {"D3DXMatrixMultiply", 0x510b45, 0x510c79 - 0x510b45, 3, 1}, {"D3DXMatrixMultiplyTranspose", 0x50f872, 0x50faa6 - 0x50f872, 3, 1},
  {"D3DXVec3TransformNormal", 0x50f5d9, 0x50f6ae - 0x50f5d9, 3, 1}, {"D3DXVec3TransformCoord", 0x511050, 0x511153 - 0x511050, 3, 1},
  {"D3DXMatrixTranspose", 0x4fc629, 0x4fc696 - 0x4fc629, 2, 1}};
static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float rf(int special) {
  u32 r = rnd() % 100, b;
  if (special && r < 8) { static const u32 s[] = {0x7f800000, 0xff800000, 0x7fc00000, 0x7f800001, 0x00000001, 0x007fffff, 0x7f7fffff}; b = s[rnd() % 7]; return *(float *)&b; }
  if (special && r < 14) { b = rnd(); return *(float *)&b; }
  if (r < 4) return 0.0f; if (r < 6) return -0.0f; if (r < 8) return 1.0f;
  if (r < 11) { b = rnd() & 0x807fffff; return *(float *)&b; }     /* denormal */
  { float f = (float)(rnd() & 0xffffff) / 16777216.0f * (float)(1 << (rnd() % 24)) / (float)(1 << (rnd() % 24)); return (rnd() & 1) ? -f : f; }
}
int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); FILE *f = fopen(argv[2], "rb"); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0;
  if (!m || !f) { printf("cannot open dll/exe\n"); return 2; }
  for (fi = 0; fi < 5; fi++) {
    unsigned char *code = VirtualAlloc(0, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); void *dll = (void *)GetProcAddress(m, T[fi].name);
    unsigned char *base = VirtualAlloc(0, 65536, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    int mode, i, bad = 0, badsp = 0, badreal = 0;
    if (!dll) { printf("%-30s MISSING export\n", T[fi].name); bad_total++; continue; }
    fseek(f, T[fi].va - 0x400000, SEEK_SET); fread(code, 1, T[fi].len, f);
    for (mode = 0; mode < 4; mode++) for (i = 0; i < N; i++) {
      /* 4 slots of 64 bytes at an offset 0/4/8/16 inside a 128 byte stride: pointers 16-aligned or only 4-aligned */
      unsigned char *p[3]; float *w; unsigned char in[3][64], o1[64], o2[64]; int k, sp = mode >= 2; unsigned csr = (mode & 1) ? 0x1f80 : 0x9fc0;
      u32 al[3]; void *r1, *r2; int alias = T[fi].alias_ok ? (int)(rnd() % 6) : 0;     /* 0..: out aliases arg1 / arg2 / none */
      for (k = 0; k < 3; k++) { al[k] = (rnd() % 3 == 0) ? 4 * (1 + rnd() % 3) : 0; p[k] = base + 128 * 4 * k + al[k]; w = (float *)in[k]; for (int j = 0; j < 16; j++) w[j] = rf(sp); }
      for (int pass = 0; pass < 2; pass++) {
        unsigned char *out = alias == 1 ? p[1] : (alias == 2 && T[fi].nargs == 3) ? p[2] : p[0]; void *res; unsigned save = _mm_getcsr();
        for (k = 0; k < 3; k++) memcpy(p[k], in[k], 64);
        _mm_setcsr(csr);
        if (T[fi].nargs == 3) res = ((F3)(pass ? (void *)code : dll))(out, p[1], p[2]); else res = ((F2)(pass ? (void *)code : dll))(out, p[1]);
        _mm_setcsr(save);
        if (pass == 0) { r1 = res == out ? (void *)1 : res; memcpy(o1, out, 64); } else { r2 = res == out ? (void *)1 : res; memcpy(o2, out, 64); }
      }
      { int ok = r1 == r2 && !memcmp(o1, o2, 64);
        /* Vec3 functions write only 12 bytes: the rest of the matrix-sized buffer must be unchanged too (it is compared whole) */
        if (!ok) { if (sp) { badsp++; for (k = 0; k < 16; k++) { u32 a, b; memcpy(&a, o1 + 4 * k, 4); memcpy(&b, o2 + 4 * k, 4); if (a != b && !(((a & 0x7fffffff) > 0x7f800000) && ((b & 0x7fffffff) > 0x7f800000))) { badreal++; break; } } } else if (++bad <= 2) { printf("  DIFF %s mode %d case %d alias %d: ret %s, bytes ", T[fi].name, mode, i, alias, r1 == r2 ? "equal" : "differ"); for (k = 0; k < 64 && o1[k] == o2[k]; k++); printf("first at %d: dll %02x orig %02x\n", k, o1[k], o2[k]); } }
      }
    }
    printf("%-30s %7d finite cases x2 MXCSR %s", T[fi].name, N * 2, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad); printf("; %d special cases differ (of %d), %d of them other than NaN payload bits\n", badsp, N * 2, badreal); bad_total += bad;
  }
  return bad_total != 0;
}
