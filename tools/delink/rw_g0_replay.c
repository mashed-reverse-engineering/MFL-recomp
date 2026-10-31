/* rw_g0_replay.c: replays the vectors of rw_g0_check.py gen against a librw facade DLL (mingw, runs under Wine).  usage: rw_g0_replay.exe <mflrw.dll> <vectors.bin>
   Per function: EQUAL | DIFF xN.  Compares the result word (st0 bits, eax or the returned buffer) and the 4 x 64 buffer bytes bit for bit, and the x87 control word after each call.  Exit 1 on any DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32;
typedef u32 (__cdecl *FI0)(void); typedef u32 (__cdecl *FI1)(u32); typedef u32 (__cdecl *FI2)(u32, u32); typedef u32 (__cdecl *FI3)(u32, u32, u32); typedef u32 (__cdecl *FI4)(u32, u32, u32, u32);
typedef u32 (__cdecl *FI5)(u32, u32, u32, u32, u32); typedef u32 (__cdecl *FI6)(u32, u32, u32, u32, u32, u32);
typedef float (__cdecl *FF1)(u32); typedef float (__cdecl *FF2)(u32, u32);
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
int main(int argc, char **argv) {
  FILE *f = fopen(argv[2], "rb"); HMODULE m = LoadLibraryA(argv[1]); char hd[12]; u32 cw, nf, bad_total = 0; int fi;
  if (!f || !m) { printf("cannot open %s / %s\n", argv[1], argv[2]); return 2; }
  fread(hd, 1, 12, f); memcpy(&cw, hd + 4, 4); memcpy(&nf, hd + 8, 4);
  { unsigned short c = (unsigned short)cw; __asm__ volatile("fldcw %0" : : "m"(c)); }
  printf("control word set to 0x%x, after LoadLibrary 0x%x\n", cw, cw_now());
  for (fi = 0; fi < (int)nf; fi++) {
    char name[33] = {0}, ak[9] = {0}, rk[5] = {0}; u32 nc, nfin, i, bad = 0, badsp = 0, nargs; void *fn;
    fread(name, 1, 32, f); fread(&nc, 4, 1, f); fread(&nfin, 4, 1, f); fread(ak, 1, 8, f); fread(rk, 1, 4, f); nargs = (u32)strlen(ak);
    fn = (void *)GetProcAddress(m, name);
    if (!fn) { printf("%-30s MISSING export\n", name); bad_total++; fseek(f, (long)nc * (28 + 512), SEEK_CUR); continue; }
    for (i = 0; i < nc; i++) {
      u32 w[6], res, got = 0, k; static unsigned char in[256] __attribute__((aligned(64))), fin[256], buf[256] __attribute__((aligned(64))); u32 a[6]; int ok;
      fread(w, 4, 6, f); fread(&res, 4, 1, f); fread(in, 1, 256, f); fread(fin, 1, 256, f);
      memcpy(buf, in, 256);
      for (k = 0; k < nargs; k++) a[k] = ak[k] == 'p' ? (u32)(uintptr_t)(buf + 64 * w[k]) : w[k];
      if (rk[0] == 'f') { float r = nargs == 1 ? ((FF1)fn)(a[0]) : ((FF2)fn)(a[0], a[1]); memcpy(&got, &r, 4); }
      else switch (nargs) { case 0: got = ((FI0)fn)(); break; case 1: got = ((FI1)fn)(a[0]); break; case 2: got = ((FI2)fn)(a[0], a[1]); break; case 3: got = ((FI3)fn)(a[0], a[1], a[2]); break;
                            case 4: got = ((FI4)fn)(a[0], a[1], a[2], a[3]); break; case 5: got = ((FI5)fn)(a[0], a[1], a[2], a[3], a[4]); break; default: got = ((FI6)fn)(a[0], a[1], a[2], a[3], a[4], a[5]); }
      if (rk[0] == 'p') { u32 g = got == 0 ? 0 : 0xffffffffu; for (k = 0; k < 4; k++) if (got == (u32)(uintptr_t)(buf + 64 * k)) g = k + 1; got = g; }
      if (rk[0] == 'v') got = 0;
      ok = got == res && !memcmp(buf, fin, 256) && cw_now() == cw;
      if (!ok && i >= nfin) badsp++; else if (!ok && bad < 2 && memcmp(buf, fin, 256)) { for (k = 0; k < 256 && buf[k] == fin[k]; k++); printf("  first byte diff at buf %u off %u: got %02x%02x%02x%02x want %02x%02x%02x%02x (input word %08x)\n", k / 64, k % 64 & ~3, buf[k|3&~0], buf[(k&~3)+2], buf[(k&~3)+1], buf[k&~3], 0,0,0,0, *(u32 *)(in + (k & ~3))); { u32 g_, w_; memcpy(&g_, buf + (k & ~3), 4); memcpy(&w_, fin + (k & ~3), 4); printf("    got %08x want %08x\n", g_, w_); } }
      if (!ok && i < nfin && ++bad <= 2) printf("  DIFF %s case %u: result got %08x want %08x, buffers %s, cw %x\n", name, i, got, res, memcmp(buf, fin, 256) ? "differ" : "equal", cw_now());
    }
    printf("%-30s %6u finite cases %s", name, nfin, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%u mismatches)", bad); printf("; %u special (inf/NaN/random bits) cases, %u differ\n", nc - nfin, badsp); bad_total += bad;
  }
  return bad_total != 0;
}
