/* rw_d3d9dev_check.c: bit-exact check of the librw DLL's d3d9dev exports (group d3d9dev, librw-mfl capi/rwd3d9dev.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9dev_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [va:byte ...]   (build / run: tools/delink/rw_d3d9dev_check.sh)
   Rows: _rwD3D9SetRenderTarget 0x4d7bb0, RwD3D9DeleteVertexDeclaration 0x4dbbc0, RwD3D9EngineSetRefreshRate 0x4da040, RwD3D9SetRenderTarget 0x4db110,
   D3D9CalculateMaxMultisamplingLevels 0x4d88d0, rwD3D9CameraEndUpdate 0x4dac70, RwD3D9GetCurrentD3DDevice 0x4dafe0.  The exe's device global (0x7d2240) holds the fake device of
   tools/delink/fakedev.c with recorders for SetRenderTarget (37) and StretchRect (34) added here (an unrecorded method traps), _RwDirect3DObject (0x7d2238) a fake IDirect3D9
   (GetAdapterModeCount, EnumAdapterModes, CheckDeviceMultiSampleType), textures / surfaces / declarations are fake COM objects that log their calls.  Stack addresses are never logged.
   Extra arguments va:byte patch the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
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

#include "fakedev.c"      /* the fake IDirect3DDevice9 for the exe's device global; unrecorded methods trap */
#define ENGVAR 0x7d2128u
#define DEVVAR 0x7d2240u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void sink(u32 w) { lg(w); }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }

#define ARENA (1u << 20)
static u8 *arena; static u32 ap;
static u32 aptr(const void *p) { return (const u8 *)p >= arena && (const u8 *)p < arena + ARENA ? (u32)(uintptr_t)p : 0x57ac; }
static void *amalloc(u32 n) { u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); } p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p; }
static void *__cdecl s_malloc(u32 n, u32 hint) { void *p = amalloc(n); lg(0xa108); lg(n); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl s_free(void *p) { lg(0xa10c); lg(aptr(p)); }
static void *__cdecl e_flalloc(void *fl, u32 hint) { void *p = amalloc(0x40); lg(0xa118); lg(aptr(fl)); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl e_flfree(void *fl, void *p) { lg(0xa11c); lg(aptr(fl)); lg(aptr(p)); }
static void *__cdecl h_flcreate(u32 size, u32 a, u32 b, u32 hint) { void *p = amalloc(0x20); lg(0xa4e90b); lg(size); lg(a); lg(b); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl h_flpurge(void *fl) { lg(0xa4e951); lg(aptr(fl)); }

/* fake COM objects: {vtbl, id, refcount} */
static void c_trap(int n) { printf("TRAP unexpected COM method %d\n", n); fflush(stdout); ExitProcess(5); }
#define CT(n) static long __stdcall ct_##n(void *self) { c_trap(n); return 0; }
CT(0) CT(1) CT(2) CT(3) CT(4) CT(5) CT(6) CT(7) CT(8) CT(9) CT(10) CT(11) CT(12) CT(13) CT(14) CT(15) CT(16) CT(17) CT(18) CT(19)
CT(20) CT(21) CT(22) CT(23) CT(24) CT(25) CT(26) CT(27) CT(28) CT(29) CT(30) CT(31) CT(32) CT(33) CT(34) CT(35) CT(36) CT(37) CT(38) CT(39)
#define CTP(n) (void *)ct_##n
#define CT10(p) CTP(p##0), CTP(p##1), CTP(p##2), CTP(p##3), CTP(p##4), CTP(p##5), CTP(p##6), CTP(p##7), CTP(p##8), CTP(p##9)
static void *const ctraps[40] = { CTP(0), CTP(1), CTP(2), CTP(3), CTP(4), CTP(5), CTP(6), CTP(7), CTP(8), CTP(9), CT10(1), CT10(2), CT10(3) };
struct Obj { void **vtbl; u32 id, rc; };
static void *v_surf[40], *v_tex2d[40], *v_texcube[40], *v_decl[40], *v_d3d[40];
static u32 nobj, getfail;
static struct Obj *mkobj(void **vt) { struct Obj *o = amalloc(sizeof *o); o->vtbl = vt; o->id = 0xc0b00000u + nobj++; o->rc = hrnd() % 3; return o; }
static u32 __stdcall c_release(struct Obj *o) { lg(0xc20e); lg(o->id); if (o->rc) o->rc--; lg(o->rc); return o->rc; }
static long __stdcall c_getlevel(struct Obj *t, u32 level, void **pp) { lg(0xc318); lg(t->id); lg(level); if (getfail && hrnd() % 6 == 0) { lg(0); return (long)0x8876086c; } *pp = mkobj(v_surf); lg(((struct Obj *)*pp)->id); return 0; }
static long __stdcall c_getcube(struct Obj *t, u32 face, u32 level, void **pp) { lg(0xc319); lg(t->id); lg(face); lg(level); if (getfail && hrnd() % 6 == 0) { lg(0); return (long)0x8876086c; } *pp = mkobj(v_surf); lg(((struct Obj *)*pp)->id); return 0; }
static u32 __stdcall d_modecount(void *self, u32 adapter, u32 fmt) { u32 n = hrnd() % 5; lg(0xd306); lg(adapter); lg(fmt); lg(n); return n; }
static long __stdcall d_enummode(void *self, u32 adapter, u32 fmt, u32 mode, u32 *out) {
  static const u32 rates[] = {0, 24, 30, 50, 59, 60, 70, 75, 85, 100, 120, 144};
  out[0] = 640 + hrnd() % 3; out[1] = 480; out[2] = rates[hrnd() % 12]; out[3] = fmt; lg(0xd307); lg(adapter); lg(fmt); lg(mode); lg(out[2]); return 0;
}
static long __stdcall d_mscheck(void *self, u32 adapter, u32 type, u32 fmt, u32 win, u32 mst, u32 *q) {
  long h = hrnd() % 3 == 0 ? (long)0x8876086a : 0; lg(0xd30b); lg(adapter); lg(type); lg(fmt); lg(win); lg(mst); lg(q != 0); if (h >= 0 && q) *q = hrnd() % 6; lg((u32)h); if (q) lg(h >= 0 ? *q : 0x77); return h;
}
static long __stdcall fd_setrt(void *self, u32 index, void *surf) { long h = hrnd() % 5 == 0 ? (long)0x8876086c : 0; lg(0xde500000u | 37); lg(index); lg(aptr(surf)); lg((u32)h); return h; }
static long __stdcall fd_stretch(void *self, struct Obj *src, const int32_t *sr, struct Obj *dst, const int32_t *dr, u32 filter) {
  int k; lg(0xde500000u | 34); lg((u32)(uintptr_t)src); for (k = 0; k < 4; k++) lg((u32)sr[k]); lg(aptr(dst)); for (k = 0; k < 4; k++) lg((u32)dr[k]); lg(sr == dr); lg(filter); return 0;
}
static void init_vtbls(void) {
  int i;
  for (i = 0; i < 40; i++) v_surf[i] = v_tex2d[i] = v_texcube[i] = v_decl[i] = v_d3d[i] = ctraps[i];
  v_surf[2] = v_decl[2] = (void *)c_release; v_tex2d[18] = (void *)c_getlevel; v_texcube[18] = (void *)c_getcube;
  v_d3d[6] = (void *)d_modecount; v_d3d[7] = (void *)d_enummode; v_d3d[11] = (void *)d_mscheck;
}
static struct Obj *d3dobj;

enum { RT0, RT1, DELV, REFR, MSL, CEND, GCUR, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rwD3D9SetRenderTargetSurface", 0x4d7bb0}, {"rw37_RwD3D9SetRenderTarget", 0x4db110}, {"rw37_RwD3D9DeleteVertexDeclaration", 0x4dbbc0},
  {"rw37_RwD3D9EngineSetRefreshRate", 0x4da040}, {"rw37_D3D9CalculateMaxMultisamplingLevels", 0x4d88d0}, {"rw37_rwD3D9CameraEndUpdate", 0x4dac70}, {"rw37_RwD3D9GetCurrentD3DDevice", 0x4dafe0}};
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32);
static void *fn[2][NF]; static long ncalls[NF];
#define EXTOFF 0x40

static void reset(u32 seed) {
  nlg = 0; ap = 0; nobj = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; EW(0x108) = (u32)(uintptr_t)s_malloc; EW(0x10c) = (u32)(uintptr_t)s_free; EW(0x118) = (u32)(uintptr_t)e_flalloc; EW(0x11c) = (u32)(uintptr_t)e_flfree;
  { u32 a; for (a = 0x7d21f0; a < 0x7d26c0; a += 4) W(a) = 0; for (a = 0x62e748; a < 0x62e774; a += 4) W(a) = 0; }      /* exe state of the rows: the runs must start equal */
  W(0x9473c0) = 0; W(0x9475f0) = 0; W(0x9475f8) = 0; W(0x9476f4) = 0;
  W(0x947144) = EXTOFF;
  fakedev_log = sink; fakedev_hr = 0; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  fakedev_vtbl[37] = (void *)fd_setrt; fakedev_vtbl[34] = (void *)fd_stretch;
  d3dobj = mkobj(v_d3d); W(0x7d2238) = (u32)(uintptr_t)d3dobj;
}
static u32 hashw(const u32 *p, u32 n) { u32 h = 2166136261u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 16777619u; return h; }
static void *mktex(u32 al) { return mkobj((al & 0xf) ? v_texcube : v_tex2d); }
static void *mkraster(u32 al) {
  u8 *p = amalloc(0xa0), *r = amalloc(0xa0); memset(p, 0, 0xa0); memset(r, 0, 0xa0);
  *(u32 *)(p + EXTOFF) = (u32)(uintptr_t)mktex(al); p[EXTOFF + 9] = (u8)al; *(u32 *)r = (u32)(uintptr_t)p;
  return r;
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 7, k, r = 0;
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: { u32 idx = rnd() % 4; void *surf = rnd() % 4 == 0 ? 0 : mkobj(v_surf);
      W(0x7d26a8 + 4 * idx) = rnd() % 3 == 0 ? (u32)(uintptr_t)surf : (rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf));
      r = ((G2)f[RT0])(idx, (u32)(uintptr_t)surf); ncalls[RT0]++; lg(r); lg(W(0x7d26a8 + 4 * idx)); break; }
    case 1: { u32 idx = rnd() % 4, al = rnd() % 3 == 0 ? ((rnd() % 6) << 4) | (1 + rnd() % 15) : (rnd() & 0xf0); void *ras = rnd() % 6 == 0 ? 0 : mkraster(al);
      W(0x7d26a8 + 4 * idx) = ras ? (rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf)) : (rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf)); getfail = 1;
      r = ((G2)f[RT1])(idx, (u32)(uintptr_t)ras); getfail = 0; ncalls[RT1]++; lg(r); for (k = 0; k < 4; k++) lg(W(0x7d26a8 + 4 * k)); break; }
    case 2: { u32 n = rnd() % 5, i; u8 *tab = amalloc(12 * (n ? n : 1)); void *target = mkobj(v_decl);
      for (i = 0; i < n; i++) { *(u32 *)(tab + 12 * i) = rnd() % 6 == 0 ? 0 : (u32)(uintptr_t)mkobj(v_decl); *(u32 *)(tab + 12 * i + 4) = rnd(); *(u32 *)(tab + 12 * i + 8) = rnd(); }
      if (n && rnd() % 4) { i = rnd() % n; if (*(u32 *)(tab + 12 * i)) target = (void *)(uintptr_t)*(u32 *)(tab + 12 * i); }
      W(0x7d2274) = n; W(0x7d227c) = (u32)(uintptr_t)tab;
      r = ((G1)f[DELV])((u32)(uintptr_t)target); ncalls[DELV]++; lg(r); lg(hashw((u32 *)tab, 3 * n)); break; }
    case 3: { static const u32 hz[] = {0, 24, 30, 50, 59, 60, 70, 75, 85, 100, 120, 144};
      W(0x62e75c) = rnd(); W(0x7d223c) = rnd() % 3; ((G1)f[REFR])(hz[rnd() % 12]); ncalls[REFR]++; lg(W(0x62e75c)); break; }
    case 4: { W(0x9475f8) = rnd(); W(0x9475f0) = rnd() % 100; W(0x62e748) = rnd() % 4; W(0x7d223c) = rnd() % 3;
      W(0x62e74c) = rnd(); W(0x62e750) = rnd(); W(0x62e754) = rnd(); W(0x62e758) = rnd();
      ((G0)f[MSL])(); ncalls[MSL]++; for (k = 0; k < 4; k++) lg(W(0x62e74c + 4 * k)); break; }
    case 5: { u8 *g = amalloc(0x80), *cam = amalloc(0x40), *ras = amalloc(0xa0); u32 al = rnd() % 3 == 0 ? ((rnd() % 6) << 4) | (1 + rnd() % 15) : (rnd() & 0xf0);
      memset(g, 0, 0x80); memset(cam, 0, 0x40); memset(ras, 0, 0xa0);
      *(u32 *)(g + 0x60) = (u32)(uintptr_t)cam; *(u32 *)cam = (u32)(uintptr_t)ras; *(u32 *)(cam + 0xc) = rnd() % 700; *(u32 *)(cam + 0x10) = rnd() % 500;
      *(u16 *)(cam + 0x1c) = (u16)(rnd() % 5 ? rnd() % 300 : 0xfff0 + rnd() % 8); *(u16 *)(cam + 0x1e) = (u16)(rnd() % 5 ? rnd() % 300 : 0xfff0 + rnd() % 8);
      ras[0x20] = rnd() % 5 == 0 ? rnd() % 8 : 5; *(u32 *)(ras + EXTOFF + 0x1c) = rnd() % 4 == 0; ras[EXTOFF + 9] = (u8)al; *(u32 *)(ras + EXTOFF) = (u32)(uintptr_t)mktex(al);
      W(0x9473c0) = (u32)(uintptr_t)g; W(0x7d224c) = rnd() % 4 != 0; { static const u32 fl[] = {0, 0x100, 0x1000000, 0x1000100, 0x1}; W(0x9476f4) = rnd() % 5 ? fl[rnd() % 5] : rnd(); }
      W(0x7d2248) = (u32)(uintptr_t)mkobj(v_surf); r = ((G0)f[CEND])(); ncalls[CEND]++; lg(r); lg(W(0x9473c0)); lg(W(0x7d224c)); break; }
    case 6: { void *fl = rnd() % 3 ? amalloc(0x20) : 0; u32 *lm = (u32 *)(uintptr_t)0x7d2288;
      for (k = 0; k < 4; k++) W(0x7d26a8 + 4 * k) = rnd();
      W(0x7d26a4) = rnd(); for (k = 0; k < 5; k++) W(0x62e760 + 4 * k) = rnd();
      for (k = 0; k < 15; k++) W(0x7d21f0 + 4 * k) = rnd();      /* 15 words: the stream records up to 0x7d222c (12 left the last record's words unchecked) */
      W(0x7d2698) = (u32)(uintptr_t)fl; W(0x7d2284) = rnd(); for (k = 0; k < 0x104; k++) lm[k] = rnd() % 8 == 0 ? (u32)(uintptr_t)amalloc(0x40) : 0;
      W(0x7d2688) = rnd(); W(0x7d269c) = rnd(); W(0x7d26a0) = rnd() % 2 ? (u32)(uintptr_t)amalloc(0x20) : 0;
      r = ((G0)f[GCUR])(); ncalls[GCUR]++; lg(r);
      for (k = 0; k < 4; k++) lg(W(0x7d26a8 + 4 * k)); lg(W(0x7d26a4)); for (k = 0; k < 5; k++) lg(W(0x62e760 + 4 * k)); for (k = 0; k < 15; k++) lg(W(0x7d21f0 + 4 * k));
      lg(W(0x7d2698)); lg(W(0x7d2284)); lg(hashw(lm, 0x104)); lg(W(0x7d2688)); lg(hashw((u32 *)(uintptr_t)W(0x7d2688), 16)); lg(W(0x7d269c)); lg(W(0x7d26a0)); break; }
    }
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena, save_eng[sizeof eng];
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): dll %08x orig %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (dll/orig):"); for (j = i > 30 ? i - 30 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  if (memcmp(save_eng, eng, sizeof eng)) { for (i = 0; i < sizeof eng && save_eng[i] == eng[i]; i++); printf("  seed %u: engine instance differs at +0x%x\n", seed, i); return 1; }
  if (memcmp(save_arena, arena, ARENA)) { for (i = 0; i < ARENA && save_arena[i] == arena[i]; i++); printf("  seed %u: arena differs at +0x%x\n", seed, i); return 1; }
  return 0;
}
static volatile u32 cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH %08lx at %p seed %u; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0; u32 seed; long tot = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[40];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  init_vtbls();
  jmp_patch(0x4e90b0, (void *)h_flcreate); jmp_patch(0x4e9510, (void *)h_flpurge);
  bind = (void *)GetProcAddress(m, "mflrw_d3d9dev_bind");
  if (!bind) { printf("mflrw_d3d9dev_bind MISSING export\n"); return 1; }
  { static const u32 t[] = {0x7d2240, 0x7d26a8, 0x947144, 0x7d2274, 0x7d227c, 0x62e75c, 0x5ea080, 0x7d2238, 0x7d223c, 0x62e748, 0x9475f0, 0x9475f8, 0x62e74c, 0x9473c0, 0x7d224c,
                            0x7d2248, 0x9476f4, 0x7d26a4, 0x62e760, 0x7d21f0, 0x7d2698, 0x7d2284, 0x7d2288, 0x4e90b0, 0x4e9510, 0x7d2688, 0x5ea090, 0x7d269c, 0x7d26a0};
    int k; for (k = 0; k < 29; k++) tbl[k] = (void *)(uintptr_t)t[k]; }
  bind((void *)ENGVAR, tbl);
  for (i = 0; i < NF; i++) { fn[0][i] = (void *)GetProcAddress(m, F[i].dll); fn[1][i] = (void *)(uintptr_t)F[i].va; if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA); memcpy(save_eng, eng, sizeof eng);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
