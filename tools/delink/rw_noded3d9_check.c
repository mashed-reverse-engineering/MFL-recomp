/* rw_noded3d9_check.c: bit-exact check of the librw DLL's noded3d9 exports (group noded3d9, librw-mfl capi/rwnoded3d9.cpp) against the ORIGINAL code of MFL.exe, both run
   natively (Wine).
   usage: rw_noded3d9_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_noded3d9_check.sh)
   Rows: see F[] below.  No device is involved: every exe callee of the rows (light enables, resource entry free, rxD3D9Instance, world transform, render state, the atomic
   default instance callback) and the node's lighting / render / user instance callbacks are logging stand-ins; the engine instance, current world, sector, mesh header,
   resource entries and the used resource list are blocks of the check's arena.  "rev" runs the original first.  va:byte patches the ORIGINAL's code first (mutation test:
   must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
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

#define ENGVAR 0x7d2128u
#define RESMOD 0x200u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 21)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))

/* stand-ins */
static u32 __cdecl h_lightsglobal(u32 type) { u32 r = hrnd() % 2; lg(0xa5055a0); lg(type); lg(r); return r; }
static u32 __cdecl h_lightlocal(u32 light) { u32 r = hrnd() % 2 ? hrnd() % 4 : 0; lg(0xa5058b0); lg(light); lg(r); return r; }
static u32 __cdecl h_lightsenable(u32 a, u32 b) { u32 r = hrnd(); lg(0xa505b20); lg(a); lg(b); lg(r); return r; }
static void __cdecl h_freeres(u32 res) { lg(0xa4ea870); lg(res); }
static u32 __cdecl h_instance(u32 a, u32 b, u32 c, u32 pentry, u32 mh, u32 cb, u32 native) {
  u32 r = hrnd() % 5 == 0 ? 0 : P(amalloc(0x40)); lg(0xa4faa20); lg(a); lg(b); lg(c); lg(pentry); lg(mh); lg(cb); lg(native); lg(r); *(u32 *)(uintptr_t)pentry = r; return r;
}
static void __cdecl h_xformworld(u32 m) { lg(0xa4db600); lg(m); }
static void __cdecl h_setrs(u32 s, u32 v) { lg(0xa4e3c00); lg(s); lg(v); }
static u32 __cdecl h_atinstance(u32 atomic, u32 p, u32 one) {
  u32 r = hrnd(); lg(0xa4ff980); lg(atomic); lg(p); lg(one); lg(*(u16 *)((u8 *)(uintptr_t)*(u32 *)(uintptr_t)(atomic + 0x18) + 0xc)); lg(r); return r;
}
static u32 __cdecl h_usercb(u32 atomic, u32 p, u32 one) { u32 r = hrnd() % 3 == 0 ? 0 : hrnd(); lg(0xcb0001); lg(atomic); lg(p); lg(one); lg(r); return r; }
static u32 __cdecl h_lightcb(u32 sector) { lg(0xcb0002); lg(sector); return hrnd(); }
static u32 __cdecl h_rendercb(u32 res, u32 sector, u32 type, u32 flags) { lg(0xcb0003); lg(res); lg(sector); lg(type); lg(flags); return hrnd(); }
static u32 __cdecl h_reinstcb(u32 atomic, u32 res, u32 cb) { u32 r = hrnd() % 4 != 0; lg(0xcb0004); lg(atomic); lg(res); lg(cb); lg(r); return r; }
static u8 *ltmbuf, *sphbuf;
static void __cdecl h_lightdir(u32 light) { lg(0xa505780); lg(light); }
static u8 *__cdecl h_framegetltm(u32 frame) { lg(0xa4d0ff0); lg(frame); return ltmbuf; }
static u8 *__cdecl h_atomicsphere(u32 atomic) { lg(0xa4f6240); lg(atomic); return sphbuf; }
static void __cdecl h_getrs(u32 state, u32 *out) { u32 v = hrnd() % 3 ? 1 : 0; *out = v; lg(0xa4e3c50); lg(state); lg(v); }

enum { WSINIT, ATINIT, SORTCB, WSLIGHT, REINST, WSNODE, ATNODE, ATLIGHT, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_D3D9WorldSectorAllInOnePipelineInit", 0x500510}, {"rw37_D3D9AtomicAllInOnePipelineInit", 0x4ff950},
  {"rw37_D3D9VertexElementSortCB", 0x500310}, {"rw37_D3D9WorldSectorDefaultLightingCallback", 0x500540}, {"rw37_D3D9AtomicDefaultReinstanceCallback", 0x500340},
  {"rw37_D3D9WorldSectorAllInOneNode", 0x5003c0}, {"rw37_D3D9AtomicAllInOneNode", 0x4ff6b0}, {"rw37_rwD3D9AtomicDefaultLightingCallback", 0x4ff4c0}};
typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF];

static void reset(u32 seed) {
  nlg = 0; ap = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng); W(0x947138) = RESMOD; W(0x62e738) = 0; W(0x62e73c) = 0; W(0x62e740) = 0; W(0x62e744) = 0;
}
static u8 *mkworld(u32 flags) { u8 *w = rblock(0x60); *(u32 *)(w + 8) = flags; EW(4) = P(w); return w; }
/* a resource entry: serial at +0x18; unlinked (next 0), or linked between two list nodes; or none */
static u8 *mkres(u32 serial) {
  u8 *res; u32 *Pn, *Nn;
  if (rnd() % 4 == 0) return 0;
  res = rblock(0x40); *(u32 *)(res + 0x18) = rnd() % 2 ? serial : rnd() % 2 ? serial + 1 : serial | 0x10000;
  if (rnd() % 3 == 0) { *(u32 *)res = 0; return res; }
  Pn = amalloc(8); Nn = amalloc(8); Pn[0] = P(res); Pn[1] = rnd(); Nn[0] = rnd(); Nn[1] = P(res); *(u32 *)res = P(Nn); *(u32 *)(res + 4) = P(Pn);
  return res;
}
static void mkreslist(void) { u32 *L = amalloc(8), *Fn = amalloc(8); L[0] = P(Fn); L[1] = rnd(); Fn[0] = rnd(); Fn[1] = P(L); EW(RESMOD + 0x24) = P(L); }
static u32 fbits(void) {      /* float bits: mostly values whose squares and sums hit the compared bounds exactly or nearly, sometimes any bit pattern */
  static const float v[] = {0.0f, 1.0f, -1.0f, 0.6f, 0.8f, 0.5f, 0.94868f, 1.0488f, 0.7f, 0.3f, 2.0f, 3.0f, 4.0f, -2.0f, 5.0f, 0.948683f, 1.048809f};
  union { float f; u32 u; } x; if (rnd() % 10 == 0) return rnd(); x.f = v[rnd() % 17]; return x.u;
}
/* a matrix row for the atomic node's unit length test: 3 of 4 rows have a squared length near or inside [0.9, 1.1] (one axis value, or 0.6 / 0.8), so the second and third row
   and both bounds are really compared (a first version drew all 3 components freely: the bound mutations 4ff884 / 4ff88c / 4ff8b9 were not caught) */
static void unitrow(u32 *r) {
  static const float a[] = {1.0f, -1.0f, 0.94868f, 1.0488f, 0.948683f, 1.048809f, 0.96f, 1.02f, 1.003f, 0.92f, 1.07f, 0.95f, 1.05f};
  union { float f; u32 u; } x; u32 p = rnd() % 3;
  if (rnd() % 4 == 0) { r[0] = fbits(); r[1] = fbits(); r[2] = fbits(); return; }
  r[0] = r[1] = r[2] = 0;
  if (rnd() % 4 == 0) { x.f = 0.6f; r[p] = x.u; x.f = rnd() % 2 ? 0.8f : -0.8f; r[(p + 1) % 3] = x.u; }
  else { x.f = a[rnd() % 13]; r[p] = x.u; }
}
static void setcw(u32 cw) { __asm__ volatile("fldcw %0" :: "m"(cw)); }
static void mkfloats(void) { u32 k; ltmbuf = amalloc(0x40); sphbuf = amalloc(0x10); for (k = 0; k < 16; k++) ((u32 *)ltmbuf)[k] = fbits(); for (k = 0; k < 4; k++) ((u32 *)sphbuf)[k] = fbits(); }
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 14, k, r = 0, cw;
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 8: case 9: case 10: {      /* D3D9AtomicAllInOneNode */
      static const u32 mf[] = {0, 0, 1, 0x20000, 0x20001, 0x10};
      u8 *node = rblock(0x20), *atomic = rblock(0x80), *geom = rblock(0x60), *mh = rblock(0x10), *r1, *r2; u32 *cbs = (u32 *)rblock(16), *params = amalloc(8), serial = rnd() % 4;
      *(u32 *)(node + 0x14) = P(cbs); cbs[1] = rnd() % 4 == 0 ? 0 : P(h_reinstcb); cbs[2] = rnd() % 4 == 0 ? 0 : P(h_lightcb); cbs[3] = rnd() % 4 == 0 ? 0 : P(h_rendercb);
      params[0] = P(atomic); params[1] = rnd(); *(u32 *)(atomic + 0x18) = P(geom); *(u32 *)(geom + 0x54) = P(mh);
      *(u32 *)(geom + 0x14) = rnd() % 8 == 0 ? (rnd() % 2 ? 0 : (u32)-5) : 1 + rnd() % 100; *(u16 *)(mh + 4) = (u16)(rnd() % 8 == 0 ? 0 : 1 + rnd() % 5); *(u16 *)(mh + 6) = (u16)serial;
      *(u32 *)(geom + 8) = (rnd() % 4 == 0 ? 0x1000000 : 0) | (rnd() % 5 == 0 ? 0x2000000 : 0) | (rnd() & 0xfcff00ffu);
      *(u32 *)(geom + 0x18) = rnd() % 2 ? 1 : 2 + rnd() % 2; *(u16 *)(geom + 0xc) = (u16)(rnd() % 2 ? 0 : rnd());
      r1 = mkres(serial); r2 = mkres(serial); *(u32 *)(geom + 0x58) = P(r1); *(u32 *)(atomic + 0x14) = P(r2); mkreslist(); mkfloats(); *(u32 *)(ltmbuf + 0xc) = mf[rnd() % 6];
      unitrow((u32 *)ltmbuf); unitrow((u32 *)(ltmbuf + 0x10)); unitrow((u32 *)(ltmbuf + 0x20));
      setcw(rnd() % 2 ? 0x7f : 0x27f);
      r = ((G2)f[ATNODE])(P(node), P(params)); ncalls[ATNODE]++;
      __asm__ volatile("fnstcw %0" : "=m"(cw)); lg(cw & 0xffff); setcw(0x27f);
      lg(r); lg(*(u32 *)(geom + 0x58)); lg(*(u32 *)(atomic + 0x14)); lg(*(u16 *)(geom + 0xc)); lg(*(u32 *)(atomic + 0x4c));
      if (r1) { lg(*(u32 *)r1); lg(*(u32 *)(r1 + 4)); } if (r2) { lg(*(u32 *)r2); lg(*(u32 *)(r2 + 4)); }
      break; }
    case 11: case 12: case 13: {      /* rwD3D9AtomicDefaultLightingCallback */
      u8 *atomic = rblock(0x80), *geom = rblock(0x60), *world, *prev, *pool[3]; u32 n = rnd() % 4, m = rnd() % 3, i, j;
      *(u32 *)(atomic + 0x18) = P(geom); *(u32 *)(geom + 8) = (rnd() & ~0x30u) | (rnd() % 5 ? 0x20 : 0) | (rnd() % 4 ? 0x10 : 0);
      world = mkworld(rnd()); if (rnd() % 10 == 0) EW(4) = 0;
      *(u16 *)(eng + 0xa) = (u16)(rnd() % 4 ? rnd() % 8 : 0xffff);
      prev = world + 0x3c;
      for (i = 0; i < n; i++) {      /* world lights: link at +0x34 */
        u8 *l = rblock(0x50); l[1] = (u8)(rnd() % 2 ? 1 : rnd() % 4); l[2] = (u8)(rnd() % 4 ? rnd() | 1 : rnd() & ~1u); for (k = 0; k < 3; k++) *(u32 *)(l + 0x18 + 4 * k) = fbits();
        *(u32 *)prev = P(l + 0x34); prev = l + 0x34;
      }
      *(u32 *)prev = P(world + 0x3c);
      for (i = 0; i < 3; i++) {      /* local lights: frame +4, radius +0x14, frame counter u16 +0x3c */
        pool[i] = rblock(0x50); pool[i][2] = (u8)(rnd() % 4 ? rnd() | 1 : rnd() & ~1u); *(u32 *)(pool[i] + 0x14) = fbits(); *(u16 *)(pool[i] + 0x3c) = (u16)(rnd() % 8);
      }
      prev = atomic + 0x64;
      for (i = 0; i < m; i++) {      /* sector ties {next, prev, sector}; the sector's light list at +0x40 {next, prev, light} */
        u32 *t = amalloc(12), kk = rnd() % 4; u8 *sector = rblock(0x60), *lp = sector + 0x40;
        for (j = 0; j < kk; j++) { u32 *e = amalloc(12); e[1] = rnd(); e[2] = rnd() % 5 == 0 ? 0 : P(pool[rnd() % 3]); *(u32 *)lp = P(e); lp = (u8 *)e; }
        *(u32 *)lp = P(sector + 0x40);
        t[1] = rnd(); t[2] = P(sector); *(u32 *)prev = P(t); prev = (u8 *)t;
      }
      *(u32 *)prev = P(atomic + 0x64);
      mkfloats(); for (k = 0; k < 4; k++) W(0x62e738 + 4 * k) = rnd();
      if (rnd() % 2) {      /* exact case of the sphere test: distance 5 (3, 4, 0), sphere radius 2, light radius 2 / 3 / 4: below, equal, above (a first version never produced
                               the equal case: mutation 4ff64c was not caught) */
        static const u32 sp[4] = {0x40400000, 0, 0, 0x40000000}, ps[3] = {0, 0x40800000, 0}, rad[3] = {0x40000000, 0x40400000, 0x40800000};
        for (k = 0; k < 4; k++) ((u32 *)sphbuf)[k] = sp[k]; for (k = 0; k < 3; k++) ((u32 *)ltmbuf)[12 + k] = ps[k];
        for (i = 0; i < 3; i++) *(u32 *)(pool[i] + 0x14) = rad[rnd() % 3];
      }
      setcw(rnd() % 2 ? 0x7f : 0x27f);
      r = ((G1)f[ATLIGHT])(P(atomic)); ncalls[ATLIGHT]++;
      __asm__ volatile("fnstcw %0" : "=m"(cw)); lg(cw & 0xffff); setcw(0x27f);
      lg(r); lg(*(u16 *)(eng + 0xa)); for (k = 0; k < 4; k++) lg(W(0x62e738 + 4 * k)); for (i = 0; i < 3; i++) lg(*(u16 *)(pool[i] + 0x3c));
      break; }
    case 0: {      /* the two pipeline init functions */
      u8 *node = rblock(0x20); u32 *cbs = (u32 *)rblock(16), w = rnd() % 2; *(u32 *)(node + 0x14) = P(cbs);
      r = ((G1)f[w ? WSINIT : ATINIT])(P(node)); ncalls[w ? WSINIT : ATINIT]++; lg(r); for (k = 0; k < 4; k++) lg(cbs[k]); break; }
    case 1: {      /* D3D9VertexElementSortCB: small values, so equal streams / offsets occur */
      u16 *a = (u16 *)rblock(8), *b = (u16 *)rblock(8);
      if (rnd() % 4) { a[0] = rnd() % 3; b[0] = rnd() % 3; a[1] = rnd() % 3; b[1] = rnd() % 3; }
      r = ((G2)f[SORTCB])(P(a), P(b)); ncalls[SORTCB]++; lg(r); break; }
    case 2: {      /* D3D9WorldSectorDefaultLightingCallback */
      u8 *sector = rblock(0x90), *prev = sector + 0x40; u32 n = rnd() % 5;
      mkworld(rnd() % 3 ? rnd() | 0x20 : rnd() & ~0x20u); *(u16 *)(eng + 0xa) = (u16)(rnd() % 4 ? rnd() : 0xffff);
      for (k = 0; k < n; k++) {
        u32 *e = amalloc(12); u8 *light = rnd() % 4 == 0 ? 0 : rblock(8); if (light) light[2] = (u8)(rnd() % 2 ? rnd() | 2 : rnd() & ~2u);
        e[1] = P(prev); e[2] = P(light); *(u32 *)prev = P(e); prev = (u8 *)e;
      }
      *(u32 *)prev = P(sector + 0x40);
      r = ((G1)f[WSLIGHT])(P(sector)); ncalls[WSLIGHT]++; lg(r); lg(*(u16 *)(eng + 0xa)); break; }
    case 3: case 4: {      /* D3D9AtomicDefaultReinstanceCallback */
      u8 *atomic = rblock(0x60), *geom = rblock(0x30), *res = rblock(0x40);
      *(u32 *)(geom + 0x18) = rnd() % 2 ? 1 : rnd() % 4 ? 2 + rnd() % 3 : rnd(); *(u16 *)(geom + 0xc) = (u16)(rnd() % 3 == 0 ? 0 : rnd()); *(u32 *)(atomic + 0x18) = P(geom);
      r = ((G3)f[REINST])(P(atomic), P(res), rnd() % 4 == 0 ? 0 : P(h_usercb)); ncalls[REINST]++; lg(r); lg(*(u16 *)(geom + 0xc)); break; }
    default: {     /* D3D9WorldSectorAllInOneNode */
      u8 *node = rblock(0x20), *sector = rblock(0x90), *mh = rblock(0x10), *res = 0; u32 *cbs = (u32 *)rblock(16), *params = amalloc(8), *L = amalloc(8), *Fn = amalloc(8), *Pn = 0, *Nn = 0;
      u32 serial = rnd() % 4;
      *(u32 *)(node + 0x14) = P(cbs); cbs[2] = rnd() % 4 == 0 ? 0 : P(h_lightcb); cbs[3] = rnd() % 4 == 0 ? 0 : P(h_rendercb); params[0] = P(sector); params[1] = rnd();
      *(u16 *)(sector + 0x82) = (u16)(rnd() % 6 == 0 ? 0 : rnd() | 1); *(u16 *)(mh + 4) = (u16)(rnd() % 6 == 0 ? 0 : 1 + rnd() % 5); *(u16 *)(mh + 6) = (u16)serial; *(u32 *)(sector + 0x78) = P(mh);
      if (rnd() % 4) {
        res = rblock(0x40); *(u32 *)(res + 0x18) = rnd() % 2 ? serial : rnd() % 2 ? serial + 1 : serial | 0x10000;
        if (rnd() % 3 == 0) *(u32 *)res = 0;
        else { Pn = amalloc(8); Nn = amalloc(8); Pn[0] = P(res); Pn[1] = rnd(); Nn[0] = rnd(); Nn[1] = P(res); *(u32 *)res = P(Nn); *(u32 *)(res + 4) = P(Pn); }
      }
      *(u32 *)(sector + 0x34) = P(res);
      mkworld((rnd() % 3 == 0 ? 0x1000000 : 0) | (rnd() % 4 == 0 ? 0x2000000 : 0) | (rnd() & 0xfcff00ffu));
      L[0] = P(Fn); L[1] = rnd(); Fn[0] = rnd(); Fn[1] = P(L); EW(RESMOD + 0x24) = P(L);
      r = ((G2)f[WSNODE])(P(node), P(params)); ncalls[WSNODE]++; lg(r); lg(*(u32 *)(sector + 0x34)); lg(L[0]); lg(L[1]); lg(Fn[0]); lg(Fn[1]);
      if (res) { lg(*(u32 *)res); lg(*(u32 *)(res + 4)); } if (Pn) { lg(Pn[0]); lg(Nn[1]); }
      break; }
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
    printf("  seed %u: call log differs at entry %u (step %u op %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 30 ? i - 30 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
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
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; long tot = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[16];
  static const u32 t[] = {0x5005b0, 0x500540, 0x4faeb0, 0x4ff980, 0x500340, 0x4ff4c0, 0x5055a0, 0x5058b0, 0x505b20, 0x4ea870, 0x4faa20, 0x947138, 0x4db600, 0x4e3c00,
                          0x62e738, 0x505780, 0x4d0ff0, 0x4f6240, 0x4e3c50};
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
  bind = (void *)GetProcAddress(m, "mflrw_noded3d9_bind");
  if (!bind) { printf("mflrw_noded3d9_bind MISSING export\n"); return 1; }
  jmp_patch(0x5055a0, (void *)h_lightsglobal); jmp_patch(0x5058b0, (void *)h_lightlocal); jmp_patch(0x505b20, (void *)h_lightsenable); jmp_patch(0x4ea870, (void *)h_freeres);
  jmp_patch(0x4faa20, (void *)h_instance); jmp_patch(0x4db600, (void *)h_xformworld); jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4ff980, (void *)h_atinstance);
  jmp_patch(0x505780, (void *)h_lightdir); jmp_patch(0x4d0ff0, (void *)h_framegetltm); jmp_patch(0x4f6240, (void *)h_atomicsphere); jmp_patch(0x4e3c50, (void *)h_getrs);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind((void *)ENGVAR, tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
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
