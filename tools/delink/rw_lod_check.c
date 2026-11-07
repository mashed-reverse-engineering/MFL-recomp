/* rw_lod_check.c: bit-exact check of the librw DLL's lod group (tools/delink/rw_lod.txt, librw-mfl capi/rwlod.cpp: RpLODAtomicSetGeometry,
   RpLODAtomicSetRange, LODAtomicCallBackRender, LODAtomicCallBackSelectLOD) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   Image mapping as rw_rwmath_check.c.  RpGeometryAddRef 0x4f9050, RpGeometryDestroy 0x4f9060, RpAtomicSetGeometry 0x4f69e0 and RwFrameGetLTM 0x4d0ff0
   are jmp-patched to logging harness functions (both sides; the DLL through mflrw_lod_bind with the exe addresses, the hook 0x568140 and the default
   selector 0x568030 included).  RwEngineInstance 0x7d2128 points at a harness engine: +0 the current camera, +0x118 / +0x11c a logging bump free list.
   RpLODAtomicGlobals 0x9488d0 gets a random extension offset and the harness free list.
   usage: rw_lod_check.exe <mflrw.dll> <MFL.exe> [N=100000] [name,name|all] [neg|fld]
   Per entry N finite cases + N/5 special cases (inf / NaN / random bits in the floats; reported, not counted).  x87 CW 0x7f.  Compared per case: the
   arena (atomics, extensions, geometry stand-ins, frames, the call log, the heap top), the return value, the CW, and (all cases) ebx / esi / edi / ebp
   + x87 TOP / tag word (REGDIFF).  neg = one input changed on the DLL side (LOD index, range word, frame position, the render hook's selector answer);
   fld = unbalanced fld1 in front of the DLL entry.  The lrintf fixture is a separate DLL (rwlod.cpp with -DMFL_FIXTURE_LOD_LRINTF, rw_lod_check.sh fix):
   LODAtomicCallBackSelectLOD must DIFF there. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
enum { K_SETGEOM, K_SETRANGE, K_SELECT, K_RENDER };
static const struct { const char *name; u32 va; int kind; } T[] = {
  {"rw37_RpLODAtomicSetGeometry", 0x568490, K_SETGEOM}, {"rw37_RpLODAtomicSetRange", 0x568600, K_SETRANGE},
  {"rw37_LODAtomicCallBackSelectLOD", 0x568030, K_SELECT}, {"rw37_LODAtomicCallBackRender", 0x568140, K_RENDER}};
#define NK ((int)(sizeof T / sizeof T[0]))
typedef void (__cdecl *Bind)(void *engine, void *globals, void *addRef, void *destroy, void *setGeom, void *ltm, void *select, void *hook);
static u32 rs = 12345;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static float ud(void) { return (float)((rnd() & 0xffffff) / 16777216.0); }
static float uf(float lo, float hi) { return lo + (hi - lo) * ud(); }
static int special;
static float bitsf(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static float spf(float f) { u32 r; if (!special || rnd() % 6) return f; r = rnd() % 4; return bitsf(r == 0 ? 0x7f800000 : r == 1 ? 0xff800000 : r == 2 ? 0x7fc00000 : rnd()); }
static u32 fw(float f) { u32 w; memcpy(&w, &f, 4); return w; }
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

#define ASZ (1u << 16)
#define HEAP0 0x1000u
static u8 *arena, *snap, *after0;
static u32 *hp, *logn;
static void hlog(u32 op, u32 a, u32 b) { u32 *l = (u32 *)(arena + 8); if (*logn < 300) { l[3 * *logn] = op; l[3 * *logn + 1] = a; l[3 * *logn + 2] = b; } (*logn)++; }
static u32 aoff(const void *p) { return p == 0 ? 0 : (const u8 *)p >= arena && (const u8 *)p < arena + ASZ ? (u32)((const u8 *)p - arena) : 0xbad00000u ^ (u32)(uintptr_t)p; }
static void *hraw(u32 n) { u8 *p; u32 k, o; *hp = (*hp + 15) & ~15u; o = *hp; if (o + n + 16 > ASZ) { printf("arena full\n"); exit(2); } p = arena + o; *hp = o + n; for (k = 0; k < n; k++) p[k] = (u8)((o + k) * 0x9du >> 3); return p; }
static void *allocr(u32 n) { u8 *p = hraw(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
static void *__cdecl e_flalloc(void *fl, u32 hint) { void *p = hraw(0x40); hlog(5, aoff(fl), aoff(p) ^ hint << 8); return p; }
static void __cdecl e_flfree(void *fl, void *p) { hlog(6, aoff(fl), aoff(p)); }
static void *__cdecl h_addref(void *g) { hlog(7, aoff(g), 0); return g; }
static int __cdecl h_destroy(void *g) { hlog(8, aoff(g), 0); return 1; }
static void *__cdecl h_setgeom(u8 *a, void *g, int same) { hlog(9, aoff(a), aoff(g) ^ (u32)same << 24); *(void **)(a + 0x18) = g; return a; }
static void *__cdecl frame_ltm(u8 *frame) { hlog(10, aoff(frame), 0); return frame + 0x50; }
static int sel_answer, sel_neg;
static int32_t __cdecl h_select(u8 *a) { hlog(11, aoff(a), 0); return sel_answer + sel_neg; }
static u8 *__cdecl h_render(u8 *a) { hlog(12, aoff(a), 0); return a + 4; }
static u8 *eng;
#define GLOB 0x9488d0u

static void __attribute__((noinline)) stackwipe(void) { volatile u32 b[2048]; int k; for (k = 0; k < 2048; k++) b[k] = 0; }
static volatile int cur_fi, cur_case, side;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT in %s case %d\n", T[cur_fi].name, cur_case); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH (cannot continue) %08lx at %p in %s case %d side %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, T[cur_fi].name, cur_case, side); fflush(stdout); ExitProcess(4); return 0; }

static u8 *mkframe(void) { u8 *f = allocr(0x90); int k; for (k = 0; k < 16; k++) ((float *)(f + 0x50))[k] = spf(uf(-500, 500)); { float *at = (float *)(f + 0x70), l; at[0] = uf(-1, 1); at[1] = uf(-1, 1); at[2] = uf(-1, 1); l = (float)sqrt(at[0] * at[0] + at[1] * at[1] + at[2] * at[2]); if (l > 0 && rnd() % 4) for (k = 0; k < 3; k++) at[k] = spf(at[k] / l); } return f; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int N = argc > 3 ? atoi(argv[3]) : 100000, fi, bad_total = 0, neg = argc > 5 && !strcmp(argv[5], "neg"), fld = argc > 5 && !strcmp(argv[5], "fld");
  const char *only = argc > 4 && strcmp(argv[4], "all") ? argv[4] : 0; Bind bind;
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(120000u + (u32)N * 20u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ASZ + 64); snap = malloc(ASZ); after0 = malloc(ASZ); arena = (u8 *)(((uintptr_t)arena + 15) & ~(uintptr_t)15); hp = (u32 *)arena; logn = (u32 *)(arena + 4);
  jmp_patch(0x4f9050, (void *)h_addref); jmp_patch(0x4f9060, (void *)h_destroy); jmp_patch(0x4f69e0, (void *)h_setgeom); jmp_patch(0x4d0ff0, (void *)frame_ltm);
  eng = calloc(1, 0x200); W(0x7d2128) = (u32)(uintptr_t)eng; *(void **)(eng + 0x118) = (void *)e_flalloc; *(void **)(eng + 0x11c) = (void *)e_flfree;
  bind = (Bind)GetProcAddress(m, "mflrw_lod_bind");
  if (!bind) { printf("mflrw_lod_bind MISSING export\n"); return 1; }
  bind((void *)0x7d2128, (void *)GLOB, (void *)0x4f9050, (void *)0x4f9060, (void *)0x4f69e0, (void *)0x4d0ff0, (void *)0x568030, (void *)0x568140);
  cw_set(0x7f);
  printf("control word 0x%x, exe mapped, DLL %s%s\n", cw_now(), argv[1], neg ? "  NEGATIVE FIXTURE: one input changed on the DLL side" : fld ? "  PLANTED FIXTURE: unbalanced fld1 in front of the DLL entry" : "");
  for (fi = 0; fi < NK; fi++) {
    void *dll = (void *)GetProcAddress(m, T[fi].name), *orig = (void *)(uintptr_t)T[fi].va; int i, k, bad = 0, badsp = 0, badreg = 0, kind = T[fi].kind; long cov[4] = {0};
    if (only && !strstr(only, T[fi].name + 5)) continue;
    if (!dll) { printf("%-36s MISSING export\n", T[fi].name); bad_total++; continue; }
    rs = 12345 + 7919 * fi;
    for (i = 0; i < N + N / 5; i++) {
      u32 a0, a1 = 0, a2 = 0, ret[2], x87[2], regbad = 0, top[2], setup, off; u8 *at, *cam, *fl; int nfield = 0; u32 *e = 0;
      cur_fi = fi; cur_case = i; special = i >= N; *hp = HEAP0; *logn = 0;
      fl = hraw(16); off = 0x4c + 4 * (rnd() % 8);
      W(GLOB) = 0x1234; W(GLOB + 4) = off; W(GLOB + 8) = 0; W(GLOB + 12) = P(fl);
      cam = allocr(0x40); *(u8 **)(cam + 4) = mkframe(); *(u8 **)eng = cam;
      at = allocr(0x80); *(u8 **)(at + 4) = rnd() % 8 ? mkframe() : *(u8 **)(cam + 4);
      *(u32 *)(at + 0x18) = rnd() % 4 ? P(allocr(16)) : 0; *(u32 *)(at + 0x48) = P(h_render);
      if (kind == K_RENDER || kind == K_SELECT || rnd() % 3) {
        e = allocr(0x40); *(u32 **)(at + off) = e;
        e[0] = rnd() % 2; e[1] = P(h_render); e[2] = kind == K_RENDER ? P(h_select) : 0x568030;
        ((float *)e)[3] = spf(uf(1, 2000)); ((float *)e)[4] = spf(rnd() % 8 ? 10.0f / ((float *)e)[3] : uf(-1, 1)); e[5] = (int)(rnd() % 12) - 1;
        for (k = 0; k < 10; k++) e[6 + k] = rnd() % 3 ? P(allocr(16)) : 0;
        if (rnd() % 4 == 0) e[6 + rnd() % 10] = *(u32 *)(at + 0x18);
      } else *(u32 **)(at + off) = 0;
      if (kind == K_SELECT && rnd() % 16 == 0) *(u32 **)(at + off) = 0;
      sel_answer = (int)(rnd() % 14) - 2;
      a0 = P(at);
      if (kind == K_SETGEOM) { a1 = rnd() % 10; a2 = rnd() % 4 ? P(allocr(16)) : rnd() % 2 ? 0 : *(u32 *)(at + 0x18); cov[e != 0]++; }
      if (kind == K_SETRANGE) { a1 = fw(spf(rnd() % 8 ? uf(1, 3000) : uf(-5, 5))); cov[e != 0]++; }
      if (kind == K_SELECT) { if (*(u32 **)(at + off)) { float *fp = (float *)(*(u8 **)(at + 4) + 0x80), *cp = (float *)(*(u8 **)(cam + 4) + 0x80);
          if (rnd() % 2) for (k = 0; k < 3; k++) fp[k] = spf(cp[k] + uf(-3000, 3000)); } cov[*(u32 **)(at + off) != 0]++; }
      if (kind == K_RENDER) cov[sel_answer < 0 ? 0 : sel_answer >= 10 ? 1 : 2]++;
      setup = *hp; memcpy(snap, arena, setup);
      for (side = 0; side < 2; side++) {
        void *fn = side ? orig : dll; u32 r, b1 = a1; float *fp = 0; u32 sv = 0;
        memcpy(arena, snap, setup); sel_neg = 0;
        if (neg && !side) {
          if (kind == K_SETGEOM) b1 = (a1 + 1) % 10;
          else if (kind == K_SETRANGE) b1 = a1 ^ 0x00400000;
          else if (kind == K_SELECT) { fp = (float *)(*(u8 **)(at + 4) + 0x80); sv = fw(fp[0]); fp[0] = fp[0] * 1.5f + 7.0f; }
          else sel_neg = 1;
        }
        cw_set(0x7f); stackwipe();
        if (fld && !side) { g_fn2 = (u32)(uintptr_t)fn; fn = plant_fld; }
        for (k = 0; k < 4; k++) g_sent[k] = 0x5e000000u ^ (u32)i * 0x9e3779b1u ^ (u32)k << 28 ^ (u32)side << 20;
        r = call4(fn, a0, b1, a2, 0);
        for (k = 0; k < 4; k++) regbad |= g_out[k] != g_sent[k];
        x87[side] = (u32)(g_env[2] >> 11 & 7) << 16 | g_env[4];
        ret[side] = (kind == K_SELECT ? r : aoff((void *)(uintptr_t)r)) ^ ((u32)cw_now() ^ 0x7f) << 24; cw_set(0x7f);
        if (fp) memcpy(fp, &sv, 4);
        top[side] = *hp;
        if (!side) memcpy(after0, arena, *hp);
      }
      if (kind == K_SELECT && !special) cov[2 + (ret[1] == 0 || ret[1] == 9)]++;
      if (regbad || x87[0] != x87[1]) { if (++badreg <= 3) printf("  REGDIFF %s case %d: callee-saved %s, x87 TOP/tag dll %05x orig %05x\n", T[fi].name, i, regbad ? "CHANGED" : "kept", x87[0], x87[1]); }
      if (ret[0] == ret[1] && top[0] == top[1] && !memcmp(after0, arena, top[0])) continue;
      if (special) { badsp++; continue; }
      if (++bad <= 3) {
        printf("  DIFF %s case %d: ret dll %08x orig %08x, heap top %x / %x", T[fi].name, i, ret[0], ret[1], top[0], top[1]);
        for (k = 0; k < (int)(top[0] < top[1] ? top[0] : top[1]); k += 4) if (*(u32 *)(after0 + k) != *(u32 *)(arena + k)) { printf(", first arena diff at +0x%x: dll %08x orig %08x", k, *(u32 *)(after0 + k), *(u32 *)(arena + k)); break; }
        printf("\n");
      }
    }
    printf("%-36s %7d finite cases %s", T[fi].name, N, bad ? "DIFF" : "EQUAL"); if (bad) printf(" (%d mismatches)", bad);
    printf("; coverage %ld %ld %ld %ld; %d of %d special cases differ; regs/x87 %s", cov[0], cov[1], cov[2], cov[3], badsp, N / 5, badreg ? "REGDIFF" : "EQUAL");
    if (badreg) printf(" (%d cases)", badreg);
    printf("\n"); bad_total += bad != 0 || badreg != 0;
  }
  printf("DONE\n");
  return bad_total != 0;
}
