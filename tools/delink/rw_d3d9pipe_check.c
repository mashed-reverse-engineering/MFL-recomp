/* rw_d3d9pipe_check.c: behavioural check of the librw DLL's d3d9pipe exports (tools/delink/rw_d3d9pipe.txt: the RW 3.7 D3D9 world pipeline functions of D3D9pipe.obj,
   librw-mfl capi/d3d9pipe.cpp, comparator rw37_SortTriangles in capi/rwleaf.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9pipe_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3d9pipe_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake IDirect3DDevice9 of tools/delink/fakedev.c (DrawPrimitive 81, DrawIndexedPrimitive 82, SetVertexDeclaration 87,
   SetVertexShader 92, SetIndices 104, SetPixelShader 107, CreateIndexBuffer 27 + buffer Release / Lock / Unlock; any other method = TRAP).  Every exe callee of the
   group except _rwD3D9ConvertToTriList 0x4fa820 and the CRT qsort 0x5d38b0 (both run as original code, so the sort also compares SortTriangles 0x4fa790 with
   rw37_SortTriangles) gets a jmp to a recorder at its original entry; RwEngineInstance 0x7d2128 points to a fake engine (current camera, RwMalloc / RwFree +0x108 /
   +0x10c recorders on a bump arena, the mesh module's primitive conversion bytes at meshModule 0x947778 + 8).  Both sides make the same calls into the same recorders.
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the
   original addresses.  Logged after every op: the device / recorder calls (the index buffer recorder logs a hash of the bytes written between Lock and Unlock), the
   return value (rxD3D9Instance, _rwD3D9EnableClippingIfNeeded), the control word, the 5 cache words 0x62e760..0x62e770, the destroyed entry (InstanceDataDestroy) or a
   hash of the last allocated resource entry (rxD3D9Instance; the instance callback recorder also hashes the header it gets).
   Extra arguments va:byte patch the ORIGINAL's code first (negative fixtures: must give DIFF).  Exit 1 on DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "fakedev.c"
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
#define W16(va) (*(volatile u16 *)(uintptr_t)(va))
#define W8(va) (*(volatile u8 *)(uintptr_t)(va))
#define LO 0x4b0000u
#define HI 0x94b000u
static unsigned char mfl_area[0x700000] __attribute__((aligned(4096)));
static int load_image(const char *path) {
  FILE *f = fopen(path, "rb"); unsigned char *d; long sz; IMAGE_NT_HEADERS32 *nt; IMAGE_SECTION_HEADER *s; int i; DWORD old;
  if (!f) return 0;
  fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET); d = malloc(sz); fread(d, 1, sz, f); fclose(f);
  if ((uintptr_t)mfl_area > LO || (uintptr_t)mfl_area + sizeof mfl_area < HI) { printf("bss block at %p does not cover %x..%x\n", mfl_area, LO, HI); return 0; }
  if (!VirtualProtect((void *)LO, HI - LO, PAGE_EXECUTE_READWRITE, &old)) { printf("VirtualProtect failed %lu\n", GetLastError()); return 0; }
  nt = (IMAGE_NT_HEADERS32 *)(d + ((IMAGE_DOS_HEADER *)d)->e_lfanew); s = IMAGE_FIRST_SECTION(nt);
  for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {
    u32 va = 0x400000 + s[i].VirtualAddress, n = s[i].SizeOfRawData < s[i].Misc.VirtualSize || !s[i].Misc.VirtualSize ? s[i].SizeOfRawData : s[i].Misc.VirtualSize, k;
    for (k = 0; k < n; k++) if (va + k >= LO && va + k < HI) ((unsigned char *)(uintptr_t)(va + k))[0] = d[s[i].PointerToRawData + k];
  }
  return 1;
}

#define DEVVAR 0x7d2240u
#define ENGVAR 0x7d2128u
#define MESHMOD 0x947778u
#define PRIMTAB 0x5ea2f4u
#define LAST0 0x62e760u      /* FVF, declaration, VS, PS, IB */
#define NOTIFY 0x4fa950u
#define T 0xc0de0000u

static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hashw(const u8 *p, u32 n) { u32 h = 0x811c9dc5u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 0x01000193u; return h; }

/* bump arenas (heap): resource entries / RwMalloc, and the op inputs */
static u8 *arena, *oa; static u32 narena, noa, last_alloc, last_size;
#define ARENA 0x1000000u
static u32 aalloc(u32 n) { u8 *p; if (narena + n + 16 > ARENA) { printf("ARENA full\n"); ExitProcess(6); } p = arena + narena; narena += (n + 15) & ~15u; memset(p, 0xcd, n); last_alloc = (u32)(uintptr_t)p; last_size = n; return last_alloc; }
static u32 oalloc(u32 n) { u8 *p; if (noa + n + 16 > ARENA) { printf("OBJ ARENA full\n"); ExitProcess(6); } p = oa + noa; noa += (n + 15) & ~15u; memset(p, 0, n); return (u32)(uintptr_t)p; }
static u32 eng[0x100];
static u32 rsval[0x100], lighting;

/* recorders at the original entries */
static u32 __cdecl e_malloc(u32 size, u32 hint) { lg(T | 0x108); lg(size); lg(hint); return aalloc(size); }
static void __cdecl e_free(u32 p) { lg(T | 0x10c); lg(p); }
static u32 __cdecl r_resalloc(u32 owner, u32 ref, u32 size, u32 notify) {
  u32 p; lg(T | 1); lg(owner); lg(ref); lg(size); lg(notify);
  p = aalloc(size + 0x18); W(p) = 0; W(p + 4) = 0; W(p + 8) = size; W(p + 0xc) = owner; W(p + 0x10) = ref; W(p + 0x14) = notify; W(ref) = p; return p;
}
static void __cdecl r_resfree(u32 re) { lg(T | 2); lg(re); }
static u32 __cdecl r_ibcreate(u32 n, u32 pib) {
  void *ib = 0; long hr;
  lg(T | 3); lg(n); lg(pib);
  if (rnd() % 8 == 0) { lg(0); return 0; }
  fakedev_createhr = 0;
  hr = ((long (__stdcall *)(void *, u32, u32, u32, u32, void **, void *))fakedev_vtbl[27])((void *)(uintptr_t)W(DEVVAR), n * 2, 8, 101, 0, &ib, 0);
  if (hr) { lg(0); return 0; }
  W(pib) = (u32)(uintptr_t)ib; lg(1); return 1;
}
static void __cdecl r_remdyn(u32 x) { lg(T | 4); lg(x); }
static void __cdecl r_deldecl(u32 d) { lg(T | 5); lg(d); }
static u32 __cdecl r_destroyvb(u32 stride, u32 size, u32 vb, u32 off) { lg(T | 6); lg(stride); lg(size); lg(vb); lg(off); return 1; }
static u32 __cdecl r_dynvbdestroy(u32 vb) { lg(T | 7); lg(vb); return 1; }
static u32 __cdecl r_sphere(u32 a) { lg(T | 8); lg(a); return a + 0x40; }
static u32 __cdecl r_sphin(u32 cam, u32 s) { u32 r = rnd() % 2; lg(T | 9); lg(cam); lg(s); lg(r); return r; }
static u32 __cdecl r_bboxin(u32 cam, u32 b) { u32 r = rnd() % 2; lg(T | 10); lg(cam); lg(b); lg(r); return r; }
static u32 __cdecl r_setrs(u32 s, u32 v) { lg(T | 11); lg(s); lg(v); return 0x5e700000u | (s & 0xff); }
static u32 __cdecl r_getrs(u32 s, u32 pv) { u32 v = s == 0x89 ? lighting : rsval[s & 0xff]; lg(T | 12); lg(s); lg(v); W(pv) = v; return 1; }
static u32 __cdecl r_settss(u32 st, u32 ty, u32 v) { lg(T | 13); lg(st); lg(ty); lg(v); return 1; }
static u32 __cdecl r_settex(u32 t, u32 st) { lg(T | 14); lg(t); lg(st); return 1; }
static u32 __cdecl r_vaenable(u32 b) { lg(T | 15); lg(b); return 1; }
static u32 __cdecl r_vaenabled(void) { u32 r = rnd() % 2; lg(T | 16); lg(r); return r; }
static u32 __cdecl r_surfprops(u32 sp, u32 col, u32 flags) { lg(T | 17); lg(sp); lg(col); lg(flags); lg(W(sp)); lg(W(col)); return 1; }
static u32 __cdecl r_texalpha(u32 t) { u32 r = rnd() % 2; lg(T | 18); lg(t); lg(r); return r; }
static u32 __cdecl r_flush(void) { lg(T | 19); return 1; }
static u32 __cdecl r_setstreams(u32 p, u32 use) { int i; lg(T | 20); lg(p); for (i = 0; i < 8; i++) lg(W(p + 4 * i)); lg(use); return 1; }
static u32 __cdecl r_instcb(u32 object, u32 h, u32 z) { u32 r = rnd() % 4 ? 1 : 0; lg(T | 21); lg(object); lg(h); lg(z); lg(hashw((u8 *)(uintptr_t)h, 0x40 + W(h + 4) * 0x24)); lg(r); return r; }
static const struct { u32 va; void *fn; } STUB[] = {
  {0x4ea960, r_resalloc}, {0x4ea870, r_resfree}, {0x4db980, r_ibcreate}, {0x5044a0, r_remdyn}, {0x4dbbc0, r_deldecl}, {0x4ec870, r_destroyvb},
  {0x4ecca0, r_dynvbdestroy}, {0x4f6240, r_sphere}, {0x4dbcb0, r_sphin}, {0x4dbd10, r_bboxin}, {0x4e3c00, r_setrs}, {0x4e3c50, r_getrs},
  {0x4e3c70, r_settss}, {0x4e55d0, r_settex}, {0x4e5860, r_vaenable}, {0x4e5950, r_vaenabled}, {0x4e3d10, r_surfprops}, {0x4dfc90, r_texalpha},
  {0x4e3b30, r_flush}, {0x4db2d0, r_setstreams}};
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

enum { DESTROY, INSTANCE, CLIP, RENDER, NALL };
static const struct { const char *dll; u32 va; int isvoid; } F[NALL] = {
  {"rw37__rwD3D9ResourceEntryInstanceDataDestroy", 0x4fa950, 1}, {"rw37_rxD3D9Instance", 0x4faa20, 0},
  {"rw37__rwD3D9EnableClippingIfNeeded", 0x4fae50, 0}, {"rw37_rxD3D9DefaultRenderCallback", 0x4faeb0, 1}};
typedef u32 (__cdecl *G7)(u32, u32, u32, u32, u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL];

static u32 small(void) { return rnd() % 3 ? rnd() % 4 : rnd(); }
static u32 fakeib(void) {      /* a real fake index buffer (Release goes through its vtable) */
  void *ib = 0; fakedev_createhr = 0;
  ((long (__stdcall *)(void *, u32, u32, u32, u32, void **, void *))fakedev_vtbl[27])((void *)(uintptr_t)W(DEVVAR), 64, 8, 101, 0, &ib, 0);
  return (u32)(uintptr_t)ib;
}
static void reset(u32 seed, unsigned short cw) {
  u32 a; int i;
  nlg = 0; rs = seed * 2654435761u + cw + 1;
  for (a = LAST0; a <= LAST0 + 0x10; a += 4) W(a) = rnd() % 2 ? 0 : rnd() % 4;
  narena = 0; noa = 0; last_alloc = last_size = 0; fakedev_hr = 0; fakedev_reset_bufs();
  memset(eng, 0, sizeof eng);
  eng[0] = 0xca000000u + rnd() % 16; eng[0x108 / 4] = (u32)(uintptr_t)e_malloc; eng[0x10c / 4] = (u32)(uintptr_t)e_free;
  for (i = 0; i < 0x100; i++) ((u8 *)eng)[0x208 + i] = rnd() % 3 ? 4 : rnd() % 7;
  ((u8 *)eng)[0x308] = 9;      /* past the table: never read */
  for (i = 0; i < 0x100; i++) rsval[i] = rnd() % 3 ? rnd() % 4 : rnd();
}
static u32 mkmaterial(void) {
  u32 m = oalloc(0x20), k;
  W(m) = rnd() % 3 ? 0x7e000000u + rnd() % 8 : 0;
  for (k = 4; k < 8; k++) W8(m + k) = rnd() % 3 ? 0xff : rnd();
  if (rnd() % 4 == 0) W(m + 4) = 0xffffffffu;
  W(m + 0xc) = rnd(); W(m + 0x10) = rnd(); W(m + 0x14) = rnd();
  return m;
}
static u32 mkdestroy(void) {
  u32 re = oalloc(0x58 + 0x24), s;
  W(re + 8) = 0x40;
  if (rnd() % 4) { u32 o = oalloc(0x20); static const u8 ty[] = { 0xff, 8, 1, 3, 8, 1 }; W8(o) = ty[rnd() % 6]; W(o + 0x18) = 0xa7000000u + rnd() % 16; W(re + 0xc) = o; }
  W(re + 0x10) = rnd(); W(re + 0x14) = NOTIFY;
  W(re + 0x20) = rnd() % 3 ? fakeib() : 0;
  W(re + 0x24) = rnd() % 7;
  for (s = re + 0x28; s < re + 0x48; s += 0x10) {
    W(s) = rnd() % 4 ? 0xb5000000u + rnd() % 16 : 0; W(s + 4) = rnd() % 2 ? 0 : rnd() % 4 * 0x100; W(s + 8) = rnd() % 3 ? 0x1c : rnd() % 64;
    W16(s + 0xc) = rnd(); W8(s + 0xe) = rnd() % 2 ? 0 : rnd() % 3; W8(s + 0xf) = rnd() % 3 ? 0 : 1;
  }
  W(re + 0x48) = rnd() % 2; W(re + 0x4c) = rnd() % 3 ? 0xdec00000u + rnd() % 4 : 0; W(re + 0x50) = rnd(); W(re + 0x54) = rnd() % 3 ? rnd() % 300 : rnd();
  return re;
}
static u32 mkmeshheader(void) {
  u32 nm = rnd() % 6 == 0 ? 0 : rnd() % 4 + 1, h = oalloc(0x10 + 12 * nm), i, base = rnd() % 3 ? 0 : rnd() % 200, range = rnd() % 3 ? 20 : rnd() % 600 + 1;
  W(h) = (rnd() % 5 == 0 ? 0x100 : 0) | (rnd() % 4 ? rnd() % 7 : rnd() & 0xff) | (rnd() % 4 == 0 ? 0x1000 : 0);
  int strip = ((u8 *)eng)[0x208 + (W(h) & 0xff)] == 4 && !(W(h) & 0x100);      /* may become a triangle list: < 3 indices would crash both sides (n - 2 loop) */
  W16(h + 4) = nm; W16(h + 6) = rnd(); W(h + 8) = rnd(); W(h + 0xc) = rnd();
  for (i = 0; i < nm; i++) {
    u32 m = h + 0x10 + 12 * i, n = rnd() % 5 == 0 && !strip ? rnd() % 3 : rnd() % 3 ? rnd() % 40 + 3 : rnd() % 300 + 3, ix = oalloc(n * 2 + 2), k;
    for (k = 0; k < n; k++) W16(ix + 2 * k) = rnd() % 6 == 0 && k ? W16(ix + 2 * k - 2) : base + rnd() % range;      /* repeats: degenerate strip triangles */
    W(m) = ix; W(m + 4) = n; W(m + 8) = rnd() % 4 ? mkmaterial() : 0;
  }
  return h;
}
static u32 mkobject(void) {
  u32 o = oalloc(0x100), gm = oalloc(0x20);
  W16(o + 0x84) = rnd() % 3 ? rnd() % 40 : rnd(); W(o + 0x18) = gm; W(gm + 0x10) = rnd() % 3 ? rnd() % 40 : rnd();
  W(o + 0x60) = rnd();
  return o;
}
static u32 mkrender(void) {
  u32 nm = rnd() % 6 == 0 ? 0 : rnd() % 4 + 1, re = oalloc(0x58 + 0x24 * nm), s, i;
  W(re + 0x18) = rnd(); W(re + 0x1c) = nm;
  W(re + 0x20) = rnd() % 3 ? 0x1b000000u + rnd() % 3 : 0; W(re + 0x24) = rnd() % 7;
  for (s = re + 0x28; s < re + 0x48; s += 0x10) { W(s) = rnd() % 4 ? 0xb5000000u + rnd() % 4 : 0; W(s + 4) = rnd() % 4 * 0x100; W(s + 8) = 0x1c; W(s + 0xc) = rnd(); }
  W(re + 0x48) = rnd() % 2; W(re + 0x4c) = rnd() % 4; W(re + 0x50) = rnd(); W(re + 0x54) = rnd();
  for (i = 0; i < nm; i++) {
    u32 p = re + 0x58 + 0x24 * i;
    W(p) = rnd() % 100; W(p + 4) = rnd() % 50; W(p + 8) = mkmaterial(); W(p + 0xc) = rnd() % 3 ? 0 : rnd() % 2 + 1; W(p + 0x10) = rnd() % 3;
    W(p + 0x14) = rnd() % 64; W(p + 0x18) = rnd() % 200; W(p + 0x1c) = rnd() % 300; W(p + 0x20) = rnd() % 100;
  }
  return re;
}
static u32 pickflags(void) { u32 f = 0; if (rnd() % 2) f |= 8; if (rnd() % 2) f |= 0x40; if (rnd() % 2) f |= rnd() % 2 ? 0x04 : 0x80; if (rnd() % 4 == 0) f |= rnd(); return f; }
static u32 picktype(void) { static const u32 t[] = { 1, 0xff, 2, 1, 0x1ff, 0x101, 0 }; return t[rnd() % 7]; }

static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s, i;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a[7] = { 0 }, r, q = rnd() % 100, dre = 0; int k = q < 20 ? DESTROY : q < 55 ? INSTANCE : q < 65 ? CLIP : RENDER;
    fakedev_hr = rnd() % 10 == 0 ? (int32_t)0x8876086c : 0;
    fakedev_lockhr = rnd() % 10 == 0 ? (int32_t)0x8876086c : 0;
    lighting = rnd() % 2 ? 0 : rnd() % 3 ? 1 : rnd();
    switch (k) {
    case DESTROY: a[0] = dre = mkdestroy(); break;
    case INSTANCE:
      a[0] = mkobject(); a[1] = 0x0e000000u + rnd() % 8; a[2] = picktype(); a[3] = oalloc(4); a[4] = mkmeshheader();
      a[5] = rnd() % 3 ? (u32)(uintptr_t)r_instcb : 0; a[6] = rnd() % 2 ? 0 : rnd() % 3 ? 1 : rnd(); break;
    case CLIP: a[0] = mkobject(); a[1] = picktype(); break;
    case RENDER: a[0] = mkrender(); a[1] = mkobject(); a[2] = picktype(); a[3] = pickflags(); break;
    }
    lg(0x57e90000u + s); lg(k); for (i = 0; i < 7; i++) lg(a[i]);
    last_alloc = last_size = 0;
    cw_set(cw);
    r = ((G7)f[k])(a[0], a[1], a[2], a[3], a[4], a[5], a[6]); ncalls[k]++;
    lg(F[k].isvoid ? 0 : r); lg(cw_now());
    cw_set(0x27f);
    for (u32 v = LAST0; v <= LAST0 + 0x10; v += 4) lg(W(v));
    if (k == DESTROY) for (u32 v = dre; v < dre + 0x58; v += 4) lg(W(v));
    if (k == INSTANCE) { lg(W(a[3])); lg(last_size); lg(last_size ? hashw((u8 *)(uintptr_t)last_alloc, last_size) : 0); }
  }
}

static u32 *save_log, nsave; static int bad_dumps;
static int compare(u32 seed, unsigned short cw) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u cw %x: log differs at entry %u (step %u op %s): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, op < NALL ? F[op].dll : "?",
           i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    if (bad_dumps++ < 2) for (k = i > 24 ? i - 24 : 0; k < i + 8 && k < n; k++) printf("    %6u  %08x %08x%s\n", k, save_log[k], LG[k], save_log[k] != LG[k] ? "  <" : "");
    return 1;
  }
  return 0;
}
static void sink(u32 w) { lg(w); }
static volatile int cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %d\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH %08lx at %p seed %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0, ci; u32 seed; long tot = 0;
  static const unsigned short cws[2] = { 0x7f, 0x27f };
  int (__cdecl *bind)(int, void *const *);
  static void *table[32];
  static const u32 tv[32] = { DEVVAR, ENGVAR, MESHMOD, PRIMTAB, LAST0, LAST0 + 4, LAST0 + 8, LAST0 + 0xc, LAST0 + 0x10,
    0x4ea960, 0x4ea870, 0x4db980, 0x4fa820, 0x5d38b0, NOTIFY, 0x5044a0, 0x4dbbc0, 0x4ec870, 0x4ecca0,
    0x4f6240, 0x4dbcb0, 0x4dbd10, 0x4e3c00, 0x4e3c50, 0x4e3c70, 0x4e55d0, 0x4e5860, 0x4e5950, 0x4e3d10, 0x4dfc90, 0x4e3b30, 0x4db2d0 };
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); oa = malloc(ARENA);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  W(ENGVAR) = (u32)(uintptr_t)eng; W(MESHMOD) = 0x200;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test of the fake device: the original render callback on a 1-mesh entry without index buffer, lit, gives a DrawPrimitive and no trap */
  reset(1, 0x7f); lighting = 1;
  { u32 re = oalloc(0x58 + 0x24); W(re + 0x1c) = 1; W(re + 0x24) = 4; W(re + 0x58 + 8) = mkmaterial(); ((G7)(uintptr_t)F[RENDER].va)(re, mkobject(), 1, 0, 0, 0, 0); }
  for (i = 0; i < (int)nlg && LG[i] != (FAKEDEV_TAG | 81); i++);
  printf("fakedev selftest: original rxD3D9DefaultRenderCallback logged %u words (%s)\n", nlg, i < (int)nlg ? "ok" : "NO DrawPrimitive");
  if (i >= (int)nlg) { printf("fakedev selftest DIFF\n"); return 1; }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9pipe_bind");
  if (!bind) { printf("mflrw_d3d9pipe_bind MISSING export\n"); return 1; }
  for (i = 0; i < 32; i++) table[i] = (void *)(uintptr_t)tv[i];
  if (bind(31, table) || !bind(32, table)) { printf("mflrw_d3d9pipe_bind count check wrong (DIFF)\n"); return 1; }
  if (!GetProcAddress(m, "rw37_SortTriangles")) { printf("rw37_SortTriangles MISSING export\n"); bad++; }
  for (i = 0; i < NALL; i++) {
    fn[1][i] = (void *)(uintptr_t)F[i].va;
    fn[0][i] = (void *)GetProcAddress(m, F[i].dll);
    if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; }
  }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps x CW 0x7f / 0x27f\n", argv[1], seeds, steps);
  for (ci = 0; ci < 2; ci++)
    for (seed = 1; seed <= (u32)seeds; seed++) {
      cur_seed = seed;
      run(0, seed, steps, cws[ci]);
      nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
      run(1, seed, steps, cws[ci]);
      tot += nlg;
      if (compare(seed, cws[ci]) && ++bad >= 5) goto done;
    }
done:
  for (i = 0; i < NALL; i++) printf("%-45s %9ld calls (both sides)\n", F[i].dll, ncalls[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
