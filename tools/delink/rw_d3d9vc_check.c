/* rw_d3d9vc_check.c: bit-exact check of the librw DLL's compressed-normal instancing converter (tools/delink/rw_d3d9vc.txt:
   rpD3D9VertexDeclarationInstV3dComp 0x506ad0, librw-mfl capi/d3d9vd.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine) on
   the same CPU.  Made from rw_d3d9vd_check.c (same image mapping: MFL.exe's sections copied into a bss block that spans 0x4b0000..0x94b000, so
   the original runs in place with its own jump table, size table and constants).  The converter touches no exe state.
   usage: rw_d3d9vc_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name|all] [neg]
   Per case: type 0..16 (15 / 16 = past the switch, the size table entry only), n -2..23, stride = element size + 0..12, random source bytes
   (4 per vertex: x y z pad as signed bytes), the destination with random bytes around and between the elements.  x87 CW 0x7f on even and
   0x27f on odd cases.  Compared: the whole arena after the call, the return value, the control word after the call.
   neg = negative fixture (negtest.sh): the DLL reads a copy of the source (outside the arena) with bytes x and y swapped; must DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
static const struct { const char *name; u32 va; } T[] = {{"rw37__rpD3D9VertexDeclarationInstV3dComp", 0x506ad0}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef u32 (__cdecl *FInst)(u32 type, void *mem, const void *src, int32_t n, u32 stride);
static const u32 esz[17] = { 4, 8, 12, 16, 4, 4, 4, 8, 4, 4, 8, 4, 8, 4, 4, 4, 8 };

static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
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

#define ASZ (1u << 16)
static u8 *arena, *snap, *after0, *negsrc;
static u32 an;
static void *allocr(u32 n) { u8 *p; u32 k; an = (an + 15) & ~15u; if (an + n > ASZ) { printf("arena full\n"); exit(2); } p = arena + an; an += n; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }

static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"); const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 16u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ); snap = malloc(ASZ); after0 = malloc(ASZ); negsrc = malloc(ASZ); if (!arena || !snap || !after0 || !negsrc) { printf("no memory\n"); return 2; }
  { unsigned x = 0x9fc0; __asm__ volatile("ldmxcsr %0" : : "m"(x)); }
  printf("exe mapped, DLL %s%s\n", argv[1], neg ? "  NEGATIVE FIXTURE: the DLL reads bytes x <-> y swapped" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0; long cov[4] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-46s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N; i++) {
      u8 *mem, *src; u32 type, stride, ret[2], cwa[2]; int32_t n; unsigned short cw = (i & 1) ? 0x27f : 0x7f;
      cur_fi = fi; cur_case = i; an = 0;
      type = rnd() % 8 == 0 ? 15 + rnd() % 2 : rnd() % 15;
      n = rnd() % 16 == 0 ? -(int32_t)(rnd() % 3) : (int32_t)(rnd() % 24);
      stride = esz[type] + 4 * (rnd() % 4); if (rnd() % 4 == 0) stride = esz[type];
      mem = allocr((n > 0 ? n : 1) * stride + 32);
      src = allocr((n > 0 ? n : 1) * 4 + 16);
      if (rnd() % 4 == 0) for (k = 0; k < 4 * (n > 0 ? n : 0); k++) { static const u8 ed[6] = {0x80, 0x7f, 0, 1, 0xff, 0x40}; src[k] = ed[rnd() % 6]; }
      cov[n <= 0 ? 0 : type <= 3 ? 1 : type <= 14 ? 2 : 3]++;
      memcpy(snap, arena, an);
      for (side = 0; side < 2; side++) {
        const u8 *s = src; u32 r;
        memcpy(arena, snap, an);
        if (neg && !side && n > 0) { memcpy(negsrc, src, 4 * n); for (k = 0; k < n; k++) { u8 t = negsrc[4 * k]; negsrc[4 * k] = negsrc[4 * k + 1]; negsrc[4 * k + 1] = t ^ (t == negsrc[4 * k] ? 1 : 0); } s = negsrc; }
        cw_set(cw);
        r = ((FInst)(side ? orig : dll))(type, mem, s, n, stride);
        cwa[side] = cw_now(); cw_set(0x27f);
        ret[side] = r;
        if (!side) memcpy(after0, arena, an);
      }
      if (ret[0] == ret[1] && cwa[0] == cwa[1] && !memcmp(after0, arena, an)) continue;
      if (++bad <= 3) {
        printf("  DIFF %s case %d: type %u n %d stride %u ret dll %08x orig %08x cw %04x / %04x", T[fi].name, i, type, n, stride, ret[0], ret[1], cwa[0], cwa[1]);
        for (k = 0; k < (int)an; k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-46s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld\n", cov[0], cov[1], cov[2], cov[3]); bad_total += bad != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
