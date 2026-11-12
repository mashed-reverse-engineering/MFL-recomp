/* rw_d3d9sys_check.c: bit-exact check of the librw DLL's D3D9System export (group d3d9sys, librw-mfl capi/rwd3d9sys.cpp) against the ORIGINAL code of MFL.exe (0x4d7c00), both run
   natively (Wine).
   usage: rw_d3d9sys_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9sys_check.sh)
   The exe's Direct3DCreate9 import slot (0x5dd2d4) returns a fake IDirect3D9 (slots 2 4 5 6 8 11 14 16: Release, GetAdapterCount, GetAdapterIdentifier, GetAdapterModeCount,
   GetAdapterDisplayMode, CheckDeviceMultiSampleType, GetDeviceCaps, CreateDevice); CreateDevice hands out the fake device of tools/delink/fakedev.c with recorders for slots
   20 38 40 added (an unrecorded method traps).  Every exe callee is a jmp-patched logging stand-in, ShowWindow / GetClientRect are stand-ins in the exe's import slots.
   Stack addresses are never logged.  "rev" runs the original first.  va:byte patches the ORIGINAL's code first (mutation test: must give DIFF).
   Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
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

#include "fakedev.c"      /* the fake IDirect3DDevice9; unrecorded methods trap */
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
#define ARENA (1u << 21)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
#define P(p) ((u32)(uintptr_t)(p))
static u32 hashw(const u32 *p, u32 n) { u32 h = 2166136261u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 16777619u; return h; }

/* fake COM objects: {vtbl, id, refcount} */
static void c_trap(int n) { printf("TRAP unexpected COM method %d\n", n); fflush(stdout); ExitProcess(5); }
#define CT(n) static long __stdcall ct_##n(void *self) { c_trap(n); return 0; }
CT(0) CT(1) CT(2) CT(3) CT(4) CT(5) CT(6) CT(7) CT(8) CT(9) CT(10) CT(11) CT(12) CT(13) CT(14) CT(15) CT(16) CT(17) CT(18) CT(19)
#define CTP(n) (void *)ct_##n
static void *const ctraps[20] = { CTP(0), CTP(1), CTP(2), CTP(3), CTP(4), CTP(5), CTP(6), CTP(7), CTP(8), CTP(9), CTP(10), CTP(11), CTP(12), CTP(13), CTP(14), CTP(15), CTP(16), CTP(17), CTP(18), CTP(19) };
struct Obj { void **vtbl; u32 id, rc; };
static void *v_surf[20], *v_d3d[20];
static u32 nobj;
static void *devobj;
static struct Obj *mkobj(void **vt) { struct Obj *o = amalloc(sizeof *o); o->vtbl = vt; o->id = 0xc0b00000u + nobj++; o->rc = 1 + hrnd() % 3; return o; }
static u32 __stdcall c_release(struct Obj *o) { lg(0xc20e); lg(o->id); if (o->rc) o->rc--; lg(o->rc); return o->rc; }
static u32 __stdcall d_count(struct Obj *o) { u32 n = hrnd() % 4; lg(0xd304); lg(o->id); lg(n); return n; }
static long __stdcall d_ident(struct Obj *o, u32 adapter, u32 flags, u8 *id) {      /* D3DADAPTER_IDENTIFIER9, 0x44c bytes: Description at +0x200, VendorId +0x428, DeviceId +0x42c */
  u32 k, n = hrnd() % 24, q = hrnd() % 6;
  for (k = 0; k < 0x44c; k++) id[k] = (u8)hrnd();
  for (k = 0; k < n; k++) id[0x200 + k] = (u8)('a' + hrnd() % 26); id[0x200 + n] = 0;
  if (q < 4) *(u32 *)(id + 0x428) = 0x1002; if (q == 0 || q == 1 || q == 4) *(u32 *)(id + 0x42c) = 0x5157;
  lg(0xd305); lg(o->id); lg(adapter); lg(flags); lg(*(u32 *)(id + 0x428)); lg(*(u32 *)(id + 0x42c)); return 0;
}
static u32 __stdcall d_modecount(struct Obj *o, u32 adapter, u32 fmt) { u32 n = hrnd() % 5; lg(0xd306); lg(o->id); lg(adapter); lg(fmt); lg(n); return n; }
static long __stdcall d_getmode(struct Obj *o, u32 adapter, u32 *mode) {
  mode[0] = 640 + hrnd() % 3; mode[1] = 480 + hrnd() % 3; mode[2] = hrnd() % 100; mode[3] = 0x13 + hrnd() % 0x13;
  lg(0xd308); lg(o->id); lg(adapter); lg(P(mode) == 0x9475e4); lg(mode[0]); lg(mode[1]); lg(mode[2]); lg(mode[3]); return 0;
}
static long __stdcall d_mscheck(struct Obj *o, u32 adapter, u32 type, u32 fmt, u32 win, u32 mst, u32 *q) {
  long h = hrnd() % 3 == 0 ? (long)0x8876086a : 0; lg(0xd30b); lg(o->id); lg(adapter); lg(type); lg(fmt); lg(win); lg(mst); lg(q != 0); if (q) lg(*q); if (h >= 0 && q) *q = hrnd() % 20; lg((u32)h); if (q) lg(*q); return h;
}
static long __stdcall d_getcaps(struct Obj *o, u32 adapter, u32 type, u32 *caps) {      /* D3DCAPS9, 0x130 bytes; DevCaps at +0x1c: bits 0x10000 / 0x100000 random */
  long h = hrnd() % 3 == 0 ? (long)0x8876086a : 0; u32 k; lg(0xd30e); lg(o->id); lg(adapter); lg(type); lg(P(caps)); lg((u32)h);
  if (h >= 0) for (k = 0; k < 0x4c; k++) caps[k] = hrnd();
  return h;
}
static long __stdcall d_createdev(struct Obj *o, u32 adapter, u32 type, u32 hwnd, u32 flags, const u32 *pp, void **pdev) {
  long h = hrnd() % 4 == 0 ? (long)0x8876086c : 0; lg(0xd310); lg(o->id); lg(adapter); lg(type); lg(hwnd); lg(flags); lg(P(pp)); lg(hashw(pp, 14)); lg(P(pdev)); lg((u32)h);
  if (h >= 0) *pdev = devobj;
  return h;
}
static void init_vtbls(void) {
  int i;
  for (i = 0; i < 20; i++) v_surf[i] = v_d3d[i] = ctraps[i];
  v_surf[2] = v_d3d[2] = (void *)c_release; v_d3d[4] = (void *)d_count; v_d3d[5] = (void *)d_ident; v_d3d[6] = (void *)d_modecount; v_d3d[8] = (void *)d_getmode;
  v_d3d[11] = (void *)d_mscheck; v_d3d[14] = (void *)d_getcaps; v_d3d[16] = (void *)d_createdev;
}
/* device recorders */
static long __stdcall fd_dialog(void *self, u32 b) { lg(0xde500000u | 20); lg(b); return 0; }
static long __stdcall fd_getrt(void *self, u32 i, void **pp) { lg(0xde500000u | 38); lg(i); lg(P(pp)); *pp = mkobj(v_surf); lg(P(*pp)); return 0; }
static long __stdcall fd_getds(void *self, void **pp) { lg(0xde500000u | 40); lg(P(pp)); *pp = mkobj(v_surf); lg(P(*pp)); return 0; }

/* engine functions, imports, exe callees */
static void __cdecl s_strcpy(u32 dst, const u8 *src) { u32 h = 2166136261u, n = 0; while (src[n] && n < 0x200) h = (h ^ src[n++]) * 16777619u; lg(0xa0cc); lg(dst); lg(n); lg(h); }
static void __cdecl s_free(u32 p) { lg(0xa10c); lg(p); }
static void *__stdcall h_d3dcreate(u32 ver) { void *r = hrnd() % 6 == 0 ? 0 : mkobj(v_d3d); lg(0xd3d9); lg(ver); lg(r != 0); return r; }
static int __stdcall s_showwindow(u32 hwnd, u32 cmd) { lg(0xd278); lg(hwnd); lg(cmd); return 1; }
static int __stdcall s_getclientrect(u32 hwnd, int32_t *rc) {
  rc[0] = hrnd() % 3; rc[1] = hrnd() % 3; rc[2] = hrnd() % 8 == 0 ? 0 : 640 + hrnd() % 200; rc[3] = hrnd() % 8 == 0 ? 0 : 480 + hrnd() % 200;
  lg(0xd2b4); lg(hwnd); lg((u32)rc[2]); lg((u32)rc[3]); return 1;
}
static void mkmodes(u32 n, u32 (*r)(void)) {      /* display mode list: 20 byte entries {width, height, rate, format, flags} */
  u32 *m = amalloc(20 * n), i;
  for (i = 0; i < n; i++) { m[5 * i] = 320 + r() % 1000; m[5 * i + 1] = 200 + r() % 800; m[5 * i + 2] = r() % 120; m[5 * i + 3] = 0x13 + r() % 0x13; m[5 * i + 4] = r() % 4 ? r() % 4 : r(); }
  W(0x7d2260) = P(m); W(0x7d2258) = n;
}
static void __cdecl h_createmodes(void) { u32 n = 1 + hrnd() % 6; mkmodes(n, hrnd); lg(0xa4d9230); lg(n); }
static void __cdecl h_setpp(u32 mode, u32 flags, u32 fmt) { int k; lg(0xa4d8990); lg(mode); lg(flags); lg(fmt); for (k = 0; k < 14; k++) { lg(W(0x947740 + 4 * k)); W(0x947740 + 4 * k) = hrnd(); } }
static u32 __cdecl h_sysstop(u32 a, u32 b, u32 c) { u32 r = hrnd(); lg(0xa4d8e00); lg(a); lg(b); lg(c); lg(r); return r; }
static u32 __cdecl h_standards(u32 a, u32 b, u32 c) { u32 r = hrnd(); lg(0xa4d8fe0); lg(a); lg(b); lg(c); lg(r); return r; }
#define TAGFN(name, tag) static void __cdecl name(void) { lg(tag); }
TAGFN(h_calcms, 0xa4d88d0) TAGFN(h_clearshaders, 0xa4d87e0) TAGFN(h_clearmatrix, 0xa4d8820) TAGFN(h_rasteropen, 0xa4dfbd0) TAGFN(h_im2dopen, 0xa4eb500) TAGFN(h_rsopen, 0xa4e4320)
TAGFN(h_vbmopen, 0xa4ece70) TAGFN(h_fpinit, 0xa5dc404)
#define CPUFN(name, tag) static u32 __cdecl name(void) { u32 r = hrnd() % 2; lg(tag); lg(r); return r; }
CPUFN(h_havemmx, 0xa4ed0b0) CPUFN(h_havesse, 0xa4ed110) CPUFN(h_havesse2, 0xa4ed170) CPUFN(h_have3dnow, 0xa4ed1d0)
static void __cdecl h_setmatmult(u32 fn) { lg(0xa4d3780); lg(fn); }
static void __cdecl h_setvecmult(u32 a, u32 b, u32 c, u32 d) { lg(0xa4d4ed0); lg(a); lg(b); lg(c); lg(d); }

static const u32 vecfns[12] = {0x4ec5c0, 0x4ec490, 0x4ec650, 0x4ec530, 0x4ec3a0, 0x4ec280, 0x4ec420, 0x4ec310, 0x5dc0ef, 0x5dc000, 0x5dc160, 0x5dc07f};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2]; static long ncalls[32];

/* the exe state D3D9System reads or writes (all cleared in reset, logged after every call) */
static const u32 state[] = {0x7d2250, 0x7d2258, 0x7d2260, 0x7d2230, 0x9475e0, 0x9475e4, 0x9475e8, 0x9475ec, 0x9475f0, 0x9475f4, 0x9475f8, 0x62e748, 0x62e74c, 0x62e750, 0x62e754,
                            0x62e758, 0x7d2238, 0x7d223c, 0x7d2234, 0x7d21e0, 0x7d21e4, 0x7d21e8, 0x7d21ec, 0x7d26b8, 0x7d26bc, 0x7d26c0, 0x7d26c4, 0x7d2248, 0x7d2244, 0x7d26a4,
                            0x7d26a8, 0x7d26ac, 0x7d26b0, 0x7d26b4, 0x7d269c, 0x7d26a0, 0x9473c4, 0x947760, 0x7d226c, 0x7d2264, 0x7d2268};
#define NSTATE (sizeof state / sizeof state[0])
static void reset(u32 seed) {
  u32 a, k;
  nlg = 0; ap = 0; nobj = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng); EW(0xcc) = P(s_strcpy); EW(0x10c) = P(s_free);
  for (k = 0; k < NSTATE; k++) W(state[k]) = 0;
  for (a = 0x947740; a < 0x947780; a += 4) W(a) = 0;
  for (a = 0x947600; a < 0x947730; a += 4) W(a) = 0;
  fakedev_log = sink; fakedev_hr = 0; devobj = fakedev_init(); W(DEVVAR) = P(devobj);
  fakedev_vtbl[20] = (void *)fd_dialog; fakedev_vtbl[38] = (void *)fd_getrt; fakedev_vtbl[40] = (void *)fd_getds;
  W(0x5dd278) = P(s_showwindow); W(0x5dd2b4) = P(s_getclientrect); W(0x5dd2d4) = P(h_d3dcreate);
}
static void run(int side, u32 seed, int steps) {
  int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    static const u8 reqs[] = {0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 3, 4, 5, 5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 10, 11, 12, 13, 14, 14, 15, 16, 16, 16, 17, 17, 17, 19, 19, 22, 18, 20, 21, 23, 0x40};
    u32 req = rnd() % 40 == 0 ? rnd() : reqs[rnd() % sizeof reqs], k, r, n = 1 + rnd() % 6, mx, *out = amalloc(0x80), *io = amalloc(16); int32_t num;
    lg(0x57e90000u + s); lg(req);
    for (k = 0; k < 0x20; k++) out[k] = rnd(); for (k = 0; k < 4; k++) io[k] = rnd();
    W(0x7d2250) = req == 7 ? rnd() % 4 == 0 : rnd() % 2;
    if ((req == 1 || req == 5 || req == 6) && rnd() % 3 == 0) { W(0x7d2260) = 0; W(0x7d2258) = 0; n = 0; } else mkmodes(n, rnd);
    W(0x7d2238) = req == 1 && rnd() % 3 == 0 ? 0 : P(mkobj(v_d3d));
    W(DEVVAR) = (req == 8 && rnd() % 3 == 0) || (req == 2 && rnd() % 2) ? 0 : P(devobj);
    W(0x947760) = rnd() % 2; W(0x7d2234) = rnd() % 4 == 0 ? 0 : 0x1000 + rnd() % 9; W(0x7d226c) = rnd() % 2; W(0x7d2264) = rnd() % 2; W(0x7d2268) = rnd() % 3 == 0;
    W(0x9475e0) = rnd(); for (k = 0; k < 3; k++) W(0x9475e4 + 4 * k) = rnd(); W(0x9475f0) = 0x13 + rnd() % 0x13; W(0x9475f4) = rnd(); W(0x9475f8) = rnd() % 4 ? rnd() % 4 : rnd();
    W(0x62e748) = 1 + rnd() % 3; for (k = 0; k < 4; k++) W(0x62e74c + 4 * k) = rnd(); W(0x7d223c) = rnd() % 3; W(0x7d2230) = rnd() % 8;
    for (k = 0; k < 4; k++) { W(0x7d21e0 + 4 * k) = rnd(); W(0x7d26b8 + 4 * k) = rnd() % 2; W(0x7d26a8 + 4 * k) = rnd(); }
    W(0x7d26a4) = rnd(); W(0x7d269c) = rnd(); W(0x7d26a0) = rnd() % 2 ? P(amalloc(0x20)) : 0; W(0x9473c4) = rnd(); W(0x7d2248) = rnd(); W(0x7d2244) = rnd();
    for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd(); for (k = 0; k < 0x4c; k++) W(0x947600 + 4 * k) = rnd();
    if (rnd() % 4 == 0) W(0x94765c) = W(0x947658);
    W(0x947760) = rnd() % 2;      /* Present.Windowed, inside the 14 words above: set last (a first version left it random, request 8 then never reached ShowWindow) */
    num = req == 6 || req == 7 ? (rnd() % 8 == 0 ? (rnd() % 2 ? -1 : 1000) : (int32_t)(rnd() % (n + 1))) : req == 8 ? (rnd() % 2 ? 0 : (int32_t)rnd()) : (int32_t)(rnd() % 4);
    mx = rnd() % 2 ? 0x1f80 : 0x9f80; __asm__ volatile("ldmxcsr %0" :: "m"(mx));
    r = ((G4)fn[side])(req, P(out), P(io), (u32)num); ncalls[req < 31 ? req : 31]++;
    __asm__ volatile("stmxcsr %0" : "=m"(mx)); lg(r); lg(mx & 0xffc0); mx = 0x1f80; __asm__ volatile("ldmxcsr %0" :: "m"(mx));
    for (k = 0; k < NSTATE; k++) lg(W(state[k]));
    lg(W(DEVVAR) == P(devobj)); for (k = 0; k < 14; k++) lg(W(0x947740 + 4 * k)); lg(hashw((u32 *)(uintptr_t)0x947600, 0x4c)); lg(hashw(out, 0x20));
    W(DEVVAR) = P(devobj);
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena;
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u request %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 30 ? i - 30 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
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
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; long tot = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[64];
  static const u32 t[] = {0x7d2250, 0x7d2258, 0x7d2260, 0x7d2230, 0x9475e4, 0x9475f4, 0x9475f8,
                          0x62e74c, 0x62e750, 0x62e754, 0x62e758, 0x7d2238, 0x62e748, 0x7d223c, 0x4d9230,
                          DEVVAR, 0x947760, 0x7d2234, 0x5dd278, 0x62e7b8, 0x9473c4,
                          0x4ed260, 0x9473e0, 0x62e830, 0x947600, 0x9475e0, 0x5ea080, 0x7d21e0, 0x4d88d0,
                          0x4ed0b0, 0x4ed110, 0x4ed170, 0x4ed1d0, 0x7d26b8, 0x7d26bc, 0x7d26c0, 0x7d26c4,
                          0x947740, 0x4d8990, 0x7d226c, 0x7d2264, 0x7d2268, 0x5dd2b4, 0x7d2248, 0x7d2244, 0x7d26a4, 0x7d26a8,
                          0x4d87e0, 0x4d8820, 0x7d26a0, 0x7d269c, 0x4dfbd0, 0x4eb500, 0x4e4320, 0x4ece70,
                          0x4d8e00, 0x4d8fe0, 0x5dc404, 0x5dc2a0, 0x4d3780, 0x4d4ed0, 0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  init_vtbls();
  bind = (void *)GetProcAddress(m, "mflrw_d3d9sys_bind");
  if (!bind) { printf("mflrw_d3d9sys_bind MISSING export\n"); return 1; }
  jmp_patch(0x4d9230, (void *)h_createmodes); jmp_patch(0x4d8990, (void *)h_setpp); jmp_patch(0x4d8e00, (void *)h_sysstop); jmp_patch(0x4d8fe0, (void *)h_standards);
  jmp_patch(0x4d88d0, (void *)h_calcms); jmp_patch(0x4ed0b0, (void *)h_havemmx); jmp_patch(0x4ed110, (void *)h_havesse); jmp_patch(0x4ed170, (void *)h_havesse2);
  jmp_patch(0x4ed1d0, (void *)h_have3dnow); jmp_patch(0x4d87e0, (void *)h_clearshaders); jmp_patch(0x4d8820, (void *)h_clearmatrix); jmp_patch(0x4dfbd0, (void *)h_rasteropen);
  jmp_patch(0x4eb500, (void *)h_im2dopen); jmp_patch(0x4e4320, (void *)h_rsopen); jmp_patch(0x4ece70, (void *)h_vbmopen); jmp_patch(0x5dc404, (void *)h_fpinit);
  jmp_patch(0x4d3780, (void *)h_setmatmult); jmp_patch(0x4d4ed0, (void *)h_setvecmult);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  tbl[sizeof t / sizeof t[0] - 1] = (void *)vecfns;
  bind((void *)ENGVAR, tbl);
  fn[rev] = (void *)GetProcAddress(m, "rw37_D3D9System"); fn[!rev] = (void *)(uintptr_t)0x4d7c00;
  if (!fn[rev]) { printf("rw37_D3D9System MISSING export\n"); return 1; }
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < 32; i++) if (ncalls[i]) printf("D3D9System request %2d%s %9ld calls (both sides)\n", i, i == 31 ? "+" : " ", ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
