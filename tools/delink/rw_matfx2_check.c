/* rw_matfx2_check.c: bit-exact check of the librw DLL's matfx2 exports (group matfx2, librw-mfl capi/rwmatfx2.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_matfx2_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [va:byte ...]   (build / run: tools/delink/rw_matfx2_check.sh)
   Rows: _rpMatFXD3D9AtomicMatFXDualPassRender 0x550b50 (device through tools/delink/fakedev.c, render state / texture stage calls and DefaultRender as logging stand-ins) and
   CalculatePerturbedUVs 0x552850 (vertex / index buffers and the vertex declaration are fake COM objects that log their calls; matrices, frames and _rwInvSqrt are stand-ins; the
   vertex format decoders 0x508cb0 / 0x5090a0 run as original code).  CalculatePerturbedUVs runs at x87 control word 0x7f and 0x27f (chosen per call).  Stack addresses are never logged.
   Extra arguments va:byte patch the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
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
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }

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
static u32 fbits(float f) { u32 u; memcpy(&u, &f, 4); return u; }
static float frnd(void) { return (float)((int)(hrnd() % 2001) - 1000) / 250.0f; }

/* DualPass callees: logging stand-ins */
static void __cdecl h_getrs(u32 st, u32 *out) { *out = hrnd() % 3 == 0; lg(0xa3c5); lg(st); lg(*out); }
static void __cdecl h_55d0(u32 a, u32 b) { lg(0xa55d); lg(a); lg(b); }
static void __cdecl h_setrs(u32 a, u32 b) { lg(0xa3c0); lg(a); lg(b); }
static void __cdecl h_settss(u32 a, u32 b, u32 c) { lg(0xa3c7); lg(a); lg(b); lg(c); }
static void __cdecl h_valpha(u32 a) { lg(0xa586); lg(a); }
static void __cdecl h_def(void *a, void *b, u32 c, void *d) { lg(0xa508); lg(aptr(a)); lg(aptr(b)); lg(c); lg((u32)(uintptr_t)d); }
static void __cdecl h_settransform(u32 st, u32 *m) { int k; lg(0xa470); lg(st); lg(aptr(m)); for (k = 0; k < 16; k++) lg(m[k]); }
static void __cdecl h_flush(void) { lg(0xa3b3); }
static int __cdecl h_4e5950(void) { int v = hrnd() % 2; lg(0xa4e59); lg(v); return v; }
static void __cdecl h_srcblend(u32 v) { lg(0xa4e53a); lg(v); }
static void __cdecl h_destblend(u32 v) { lg(0xa4e53f); lg(v); }
static void __cdecl h_convnonpal(void *r) { lg(0xa4e38b); lg((u32)(uintptr_t)r); }
static long __stdcall fd_validate(void *self, u32 *passes) { long h = hrnd() % 4 == 0 ? (long)0x8876086c : 0; *passes = hrnd() % 3; lg(0xde500000u | 70); lg(*passes); lg((u32)h); return h; }
static void __cdecl e_rsget(u32 id, u32 *p) { *p = hrnd() & 7; lg(0xa0024); lg(id); lg(*p); }
static void __cdecl e_rsset(u32 id, u32 v) { lg(0xa0020); lg(id); lg(v); }

/* CalculatePerturbedUVs callees */
static void __cdecl h_gettransform(u32 st, float *m) { int k; lg(0xa4db5d); lg(st); for (k = 0; k < 16; k++) m[k] = frnd(); }
static void __cdecl h_invert(float *d, float *s) { int k; lg(0xa449); for (k = 0; k < 16; k++) { lg(((u32 *)s)[k]); d[k] = s[(k * 5) & 15] * -0.5f + 1.0f; } }
static void *__cdecl h_getltm(void *fr) { lg(0xa0ff); lg(aptr(fr)); return (u8 *)fr + 0x40; }
static void __cdecl h_v3dpoint(float *o, float *in, float *m) {
  int k; lg(0xa5640); lg(fbits(in[0])); lg(fbits(in[1])); lg(fbits(in[2])); for (k = 0; k < 16; k++) lg(((u32 *)m)[k]);
  for (k = 0; k < 3; k++) o[k] = in[0] * m[k] + in[1] * m[4 + k] + in[2] * m[8 + k] + m[12 + k];
}
static void __cdecl h_v3dvector(float *o, float *in, float *m) {
  int k; lg(0xa56a0); lg(fbits(in[0])); lg(fbits(in[1])); lg(fbits(in[2])); for (k = 0; k < 16; k++) lg(((u32 *)m)[k]);
  for (k = 0; k < 3; k++) o[k] = in[0] * m[k] + in[1] * m[4 + k] + in[2] * m[8 + k];
}
static double __cdecl h_invsqrt(float x) { lg(0xa5470); lg(fbits(x)); if (x == 0.0f) return 1.0; return 1.0 / sqrt((double)x); }

/* fake vertex / index buffers and the vertex declaration */
struct HBuf { void **vtbl; u32 id, len; u8 *data; };
struct HDecl { void **vtbl; u32 n; u8 el[8 * 20]; };
static void *hb_vtbl[16], *hd_vtbl[16];
static void hb_trap(int n) { printf("TRAP unexpected buffer / declaration method %d\n", n); fflush(stdout); ExitProcess(5); }
#define HT(n) static long __stdcall ht_##n(void *self) { hb_trap(n); return 0; }
HT(0) HT(1) HT(2) HT(3) HT(4) HT(5) HT(6) HT(7) HT(8) HT(9) HT(10) HT(11) HT(12) HT(13) HT(14) HT(15)
static long __stdcall hb_lock(struct HBuf *b, u32 off, u32 size, void **pp, u32 flags) {
  lg(0xb10c); lg(b->id); lg(off); lg(size); lg(flags);
  if (off + size > b->len) { printf("check bug: lock %u+%u beyond %u\n", off, size, b->len); ExitProcess(6); }
  *pp = b->data + off; return 0;
}
static long __stdcall hb_unlock(struct HBuf *b) { lg(0xb10d); lg(b->id); return 0; }
static long __stdcall hd_get(struct HDecl *d, u8 *el, u32 *num) { lg(0xdec1); lg(d->n); memcpy(el, d->el, 8 * d->n); *num = d->n; return 0; }
static void init_vtbls(void) {
  static long (__stdcall *const t[16])(void *) = { ht_0, ht_1, ht_2, ht_3, ht_4, ht_5, ht_6, ht_7, ht_8, ht_9, ht_10, ht_11, ht_12, ht_13, ht_14, ht_15 };
  int i; for (i = 0; i < 16; i++) { hb_vtbl[i] = (void *)t[i]; hd_vtbl[i] = (void *)t[i]; }
  hb_vtbl[11] = (void *)hb_lock; hb_vtbl[12] = (void *)hb_unlock; hd_vtbl[4] = (void *)hd_get;
}
static u32 nbuf;
static struct HBuf *mkbuf(u32 len) { struct HBuf *b = amalloc(sizeof *b); b->vtbl = hb_vtbl; b->id = 0xb0f00000u + nbuf++; b->len = len; b->data = amalloc(len); return b; }

static u32 *mkmat2(void) { u32 *m = amalloc(0x40); int k; for (k = 0; k < 16; k++) m[k] = rnd(); m[3] = (rnd() % 2 ? 0x20000u : 0) | (rnd() & 0xff); return m; }
static float *mkmat(void) { float *m = amalloc(0x40); int k; for (k = 0; k < 16; k++) m[k] = frnd(); return m; }
static u8 *mkframe(void) {
  u8 *f = amalloc(0x100), *head = f + 0x90, *prev; u32 *hn = (u32 *)head; int n = rnd() % 4, i; static const u8 sub[] = {1, 0x81, 0x82, 0, 2, 5, 0x80, 0x83};
  memset(f, 0, 0x100); { float *m = (float *)(f + 0x40); int k; for (k = 0; k < 16; k++) m[k] = frnd(); }
  hn[0] = hn[1] = (u32)(uintptr_t)head; prev = head;
  for (i = 0; i < n; i++) {
    u8 *o = amalloc(0x20); u32 *lk = (u32 *)(o + 8); memset(o, 0, 0x20);
    o[0] = rnd() % 4 == 0 ? 1 : (rnd() % 4 == 0 ? 2 : 3); o[1] = sub[rnd() % 8];
    lk[0] = (u32)(uintptr_t)head; lk[1] = (u32)(uintptr_t)prev; ((u32 *)prev)[0] = (u32)(uintptr_t)lk; hn[1] = (u32)(uintptr_t)lk; prev = (u8 *)lk;
  }
  return f;
}
#define MATOFF 0x30

enum { DUAL, CUV, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rpMatFXD3D9AtomicMatFXDualPassRender", 0x550b50}, {"rw37_CalculatePerturbedUVs", 0x552850}};
static void *fn[2][NF]; static long ncalls[NF];

static void reset(u32 seed) {
  nlg = 0; ap = 0; nbuf = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; EW(0x108) = (u32)(uintptr_t)s_malloc; EW(0x10c) = (u32)(uintptr_t)s_free;
  W(0x7da71c) = MATOFF;
  W(0x62e76c) = 0; W(0x62e768) = 0; W(0x62e770) = 0; W(0x62e764) = 0; W(0x62e760) = 0;
  memset((void *)(uintptr_t)0x63a2d0, 0, 0x40);
  { static const u32 gl[] = {0x7da754, 0x7da758, 0x7da75c, 0x7da764, 0x63a2c8}; int k; for (k = 0; k < 5; k++) W(gl[k]) = 0; }
  fakedev_log = sink; fakedev_hr = 0; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  fakedev_vtbl[70] = (void *)fd_validate; EW(0x24) = (u32)(uintptr_t)e_rsget; EW(0x20) = (u32)(uintptr_t)e_rsset;
}

static u32 hashw(const u32 *p, u32 n) { u32 h = 2166136261u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 16777619u; return h; }

/* vertex layout: element types (D3DDECLTYPE) and their sizes */
static const u8 TSZ[15] = {4, 8, 12, 16, 4, 4, 4, 8, 4, 4, 8, 4, 8, 4, 4};
struct Elem { u8 stream, type, usage, uidx; u16 off; };
static void uvstep(int side, void **f) {
  static const u8 PT[] = {2, 2, 2, 3, 7, 10, 12, 14, 9}, NT[] = {2, 2, 14, 7, 10, 3}, TT[] = {2, 2, 3, 14, 10}, UT[] = {1, 1, 1, 6, 9, 11, 10, 3, 0, 7};
  struct Elem e[16]; int ne = 0, i, j, two = rnd() % 3 == 0, s1mode = rnd() % 3, nulltan = rnd() % 8 == 0;
  u32 nv = 1 + rnd() % 14, pt, nidx, vstart = rnd() % 3, sidx = rnd() % 4, off[2] = {0, 0}, stride[2], soff[2], capv, k, haveN, haveC, haveT, indexed = rnd() % 5 < 3;
  u8 *hdr = amalloc(0x60), *mesh = amalloc(0x60), *mat = amalloc(0x60), *md = amalloc(0x80), *frm, *cam = amalloc(0x10), *camf;
  struct HDecl *dc = amalloc(sizeof *dc); struct HBuf *vb[2] = {0, 0}, *ib = 0; u32 *out; static const float sc[] = {0.5f, 1.0f, 2.0f, 0.25f, 0.1f, 3.0f, -1.0f};
  unsigned short cw = rnd() % 2 ? 0x7f : 0x27f;
  u32 r = rnd() % 20; pt = r < 8 ? 4 : r < 17 ? 5 : (r < 19 ? 3 : 6);
  nidx = pt == 4 || pt == 5 ? 1 + rnd() % (2 * nv + 3) : 1 + rnd() % nv;
  capv = (nv > nidx ? nv : nidx) + 8;
  if (nulltan) two = 0;
  haveN = rnd() % 10 < 7; haveC = rnd() % 2; haveT = rnd() % 2;
  #define ADD(ty, us, ui) do { e[ne].type = (ty); e[ne].usage = (us); e[ne].uidx = (ui); e[ne].stream = two ? rnd() % 2 : 0; e[ne].off = off[e[ne].stream]; off[e[ne].stream] += (TSZ[ty] + 3) & ~3; ne++; } while (0)
  ADD(PT[rnd() % 9], 0, 0);
  if (haveN) ADD(NT[rnd() % 6], 3, 0);
  if (haveC) ADD(4, 10, 0);
  ADD(UT[rnd() % 10], 5, 0);
  if (haveT && !nulltan) ADD(TT[rnd() % 5], 6, 0);
  if (rnd() % 3 == 0) { static const u8 us[] = {0, 3, 5, 6, 10, 1}; u8 u = us[rnd() % 6]; ADD(2, u, 1); }
  /* shuffle */
  for (i = ne - 1; i > 0; i--) { j = rnd() % (i + 1); struct Elem t = e[i]; e[i] = e[j]; e[j] = t; }
  if (nulltan) { e[ne].stream = 1; e[ne].off = 0; e[ne].type = 2; e[ne].usage = 6; e[ne].uidx = 0; ne++; }
  for (i = 0; i < 2; i++) { stride[i] = off[i] + 4 * (rnd() % 3); if (!stride[i]) stride[i] = 4; soff[i] = 4 * (rnd() % 3); }
  /* decl */
  dc->vtbl = hd_vtbl; dc->n = ne + 1;
  for (i = 0; i < ne; i++) { u8 *p = dc->el + 8 * i; *(u16 *)p = e[i].stream; *(u16 *)(p + 2) = e[i].off; p[4] = e[i].type; p[5] = 0; p[6] = e[i].usage; p[7] = e[i].uidx; }
  { u8 *p = dc->el + 8 * ne; *(u16 *)p = 0xff; *(u16 *)(p + 2) = 0; p[4] = 17; p[5] = 0; p[6] = 0; p[7] = 0; }
  /* buffers: stream 0 always, stream 1 when used (or sometimes unused with other lock flags) */
  memset(hdr, 0, 0x60);
  /* stream 1 unused: mode 0 no buffer, 1 a buffer that is not locked (flags & 0xff6 == 0), 2 a locked one nobody reads; the null tangent trick needs it unlocked */
  if (nulltan) s1mode = rnd() % 2;
  for (i = 0; i < 2; i++) {
    int used = i == 0 || two; u32 fl;
    *(u32 *)(hdr + 0x18 + 16 * i) = stride[i]; *(u16 *)(hdr + 0x1c + 16 * i) = 0xff6;
    if (!used && s1mode == 0) continue;
    vb[i] = mkbuf((vstart + capv) * stride[i] + soff[i] + 64 + 8 * stride[i]);
    for (k = 0; k < vb[i]->len / 4; k++) { float v = (float)((int)(rnd() % 2001) - 1000) / 250.0f; memcpy(vb[i]->data + 4 * k, &v, 4); }
    fl = used ? (rnd() % 2 ? 0xff6u : 0x2u) : (s1mode == 1 ? (rnd() % 2 ? 0u : 0x1u) : 0xff6u);
    *(u32 *)(hdr + 0x10 + 16 * i) = (u32)(uintptr_t)vb[i]; *(u32 *)(hdr + 0x14 + 16 * i) = soff[i]; *(u16 *)(hdr + 0x1c + 16 * i) = (u16)fl;
  }
  if (indexed) {
    ib = mkbuf(2 * (sidx + nidx + 16) + 64);
    for (k = 0; k < ib->len / 2; k++) ((u16 *)ib->data)[k] = (u16)(rnd() % nv);
    *(u32 *)(hdr + 8) = (u32)(uintptr_t)ib;
  }
  *(u32 *)(hdr + 0xc) = pt; *(u32 *)(hdr + 0x34) = (u32)(uintptr_t)dc;
  /* mesh, material, frames */
  memset(mesh, 0, 0x60); *(u32 *)mesh = nidx; *(u32 *)(mesh + 4) = vstart; *(u32 *)(mesh + 8) = (u32)(uintptr_t)mat; *(u32 *)(mesh + 0x18) = nv; *(u32 *)(mesh + 0x1c) = sidx;
  memset(mat, 0, 0x60); *(u32 *)(mat + MATOFF) = (u32)(uintptr_t)md;
  memset(md, 0, 0x80); frm = mkframe(); *(u32 *)md = rnd() % 10 < 7 ? (u32)(uintptr_t)frm : 0;
  { float a = sc[rnd() % 7], b = sc[rnd() % 7]; memcpy(md + 0xc, &a, 4); memcpy(md + 0x10, &b, 4); }
  camf = mkframe(); memset(cam, 0, 0x10); ((u32 *)cam)[1] = (u32)(uintptr_t)camf; EW(0) = (u32)(uintptr_t)cam;
  out = amalloc(44 * capv + 64);
  cw_set(cw);
  ((int (__cdecl *)(u32, u32, u32))f[CUV])((u32)(uintptr_t)hdr, (u32)(uintptr_t)mesh, (u32)(uintptr_t)out);
  cw_set(0x27f);
  ncalls[CUV]++; lg(cw); lg(hashw(out, 11 * capv)); lg(aptr(out));
  (void)side;
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 5;
    lg(0x57e90000u + s); lg(op);
    if (op < 2) {
      static const u32 PAIRS[6][2] = {{9, 1}, {1, 3}, {5, 6}, {9, 3}, {2, 2}, {1, 5}};
      u8 *hdr = amalloc(0x40), *mesh = amalloc(0x60), *mat = amalloc(0x60), *md = amalloc(0x80), *tr, *tr2; u32 *tex = 0, *tex2 = 0, *pp, type, src, dst, k, fl;
      memset(hdr, 0, 0x40); memset(mesh, 0, 0x60); memset(mat, 0, 0x60); memset(md, 0, 0x80);
      *(u32 *)(hdr + 8) = rnd() % 2 ? 0x1b0000u + rnd() % 3 : 0; *(u32 *)(hdr + 0xc) = 4 + rnd() % 3;
      *(u32 *)(mesh + 8) = (u32)(uintptr_t)mat; *(u32 *)(mat + MATOFF) = (u32)(uintptr_t)md; *(u32 *)(mesh + 0x10) = 0x5000 + rnd() % 3;
      for (k = 0x14; k < 0x24; k += 4) *(u32 *)(mesh + k) = rnd() % 100;
      type = rnd() % 8 == 0 ? rnd() % 8 : (rnd() % 2 ? 4 : 6);
      if (rnd() % 6) { k = rnd() % 6; src = PAIRS[k][0]; dst = PAIRS[k][1]; } else { src = 1 + rnd() % 9; dst = 1 + rnd() % 9; }
      *(u32 *)(md + 0x30) = type;
      if (type == 6) { pp = (u32 *)(md + 0x18); *(u32 *)md = rnd() % 4 ? (u32)(uintptr_t)mkmat2() : 0; *(u32 *)(md + 4) = rnd() % 4 ? (u32)(uintptr_t)mkmat2() : 0; }
      else { pp = (u32 *)md; *(u32 *)md = rnd(); }
      pp[1] = src; pp[2] = dst;
      tr = amalloc(0x40); tr2 = amalloc(0x40); memset(tr, 0, 0x40); memset(tr2, 0, 0x40); tr[0x23] = rnd() % 2 ? 0x20 : 0; tr2[0x23] = rnd() % 2 ? 0x20 : 0;
      if (rnd() % 6) { tex = amalloc(16); tex[0] = (u32)(uintptr_t)tr; }
      k = rnd() % 12;
      if (k == 0) tex2 = 0; else if (k == 1) tex2 = tex; else if (k == 2) { tex2 = amalloc(16); tex2[0] = 0; } else { tex2 = amalloc(16); tex2[0] = (u32)(uintptr_t)tr2; }
      W(0x7da754) = rnd() % 5 != 0; W(0x7da764) = rnd() % 5; W(0x7da758) = rnd() % 2; W(0x7da75c) = rnd() % 2; W(0x63a2c8) = rnd() % 2;
      W(0x62e76c) = rnd() % 2 ? 0x5ade0000u + rnd() % 4 : 0; W(0x62e768) = 0x5000 + rnd() % 3; fakedev_hr = 0;
      fl = rnd() % 3 == 0 ? 0x84 : rnd() & 0xff;
      ((void (__cdecl *)(u32, u32, u32, u32, u32))f[DUAL])((u32)(uintptr_t)hdr, (u32)(uintptr_t)mesh, fl, (u32)(uintptr_t)tex, (u32)(uintptr_t)tex2); ncalls[DUAL]++;
      lg(W(0x62e76c)); lg(W(0x62e768)); lg(W(0x7da75c)); lg(W(0x63a2c8)); lg(tr[0x23]); lg(tr2[0x23]);
      for (k = 0; k < 16; k++) lg(W(0x63a2d0 + 4 * k));
    } else uvstep(side, f);
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
  jmp_patch(0x4e55d0, (void *)h_55d0); jmp_patch(0x4e3c70, (void *)h_settss); jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4e3c50, (void *)h_getrs);
  jmp_patch(0x4e38b0, (void *)h_convnonpal); jmp_patch(0x4db470, (void *)h_settransform); jmp_patch(0x4e3b30, (void *)h_flush); jmp_patch(0x4e5950, (void *)h_4e5950);
  jmp_patch(0x4e5860, (void *)h_valpha); jmp_patch(0x4e53a0, (void *)h_srcblend); jmp_patch(0x4e53f0, (void *)h_destblend); jmp_patch(0x5508d0, (void *)h_def);
  jmp_patch(0x4db5d0, (void *)h_gettransform); jmp_patch(0x4d4490, (void *)h_invert); jmp_patch(0x4d0ff0, (void *)h_getltm); jmp_patch(0x4d5640, (void *)h_v3dpoint);
  jmp_patch(0x4d56a0, (void *)h_v3dvector); jmp_patch(0x4d5470, (void *)h_invsqrt);
  bind = (void *)GetProcAddress(m, "mflrw_matfx2_bind");
  if (!bind) { printf("mflrw_matfx2_bind MISSING export\n"); return 1; }
  { static const u32 t[] = {0x7da71c, 0x7da754, 0x7da764, 0x7da758, 0x7da75c, 0x63a2c8, 0x4e55d0, 0x4e3c70, 0x4e3c00, 0x4e3c50, 0x4e38b0, 0x4db470, 0x4e3b30, 0x63a2d0,
                            0x7d2240, 0x62e76c, 0x62e768, 0x4e5950, 0x4e5860, 0x4e53a0, 0x4e53f0, 0x5508d0,
                            0x4db5d0, 0x4d4490, 0x4d0ff0, 0x4d5640, 0x4d56a0, 0x4d5470, 0x508cb0, 0x5090a0};
    int k; for (k = 0; k < 30; k++) tbl[k] = (void *)(uintptr_t)t[k]; }
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
