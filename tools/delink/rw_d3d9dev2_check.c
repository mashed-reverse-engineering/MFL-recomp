/* rw_d3d9dev2_check.c: bit-exact check of the librw DLL's d3d9dev2 exports (group d3d9dev2, librw-mfl capi/rwd3d9dev2.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9dev2_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [va:byte ...]   (build / run: tools/delink/rw_d3d9dev2_check.sh)
   Rows: see F[] below.  The exe's device global (0x7d2240) holds the fake device of tools/delink/fakedev.c with the recorders of this file added (an unrecorded method traps),
   _RwDirect3DObject (0x7d2238) a fake IDirect3D9, textures / surfaces / declarations are fake COM objects that log their calls.  Stack addresses are never logged.
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
static struct { u8 *p; u32 n; } blk[1 << 16]; static u32 nblk;
static u32 aptr(const void *p) { return (const u8 *)p >= arena && (const u8 *)p < arena + ARENA ? (u32)(uintptr_t)p : 0x57ac; }
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA || nblk == (1 << 16)) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; blk[nblk].p = p; blk[nblk++].n = n; return p;
}
static void *__cdecl s_malloc(u32 n, u32 hint) { void *p = amalloc(n); lg(0xa108); lg(n); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl s_free(void *p) { lg(0xa10c); lg(aptr(p)); }
static void *__cdecl s_realloc(void *old, u32 n, u32 hint) {
  void *p = amalloc(n); u32 i, on = 0; for (i = 0; i < nblk; i++) if (blk[i].p == old) on = blk[i].n;
  memcpy(p, old, on < n ? on : n); lg(0xa110); lg(aptr(old)); lg(n); lg(hint); lg((u32)(uintptr_t)p); return p;
}
static void *__cdecl e_flalloc(void *fl, u32 hint) { void *p = amalloc(0x40); lg(0xa118); lg(aptr(fl)); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl e_flfree(void *fl, void *p) { lg(0xa11c); lg(aptr(fl)); lg(aptr(p)); }

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
static u32 oid(const void *o) { return o ? ((const struct Obj *)o)->id : 0; }
static u32 __stdcall c_addref(struct Obj *o) { lg(0xc10a); lg(o->id); o->rc++; lg(o->rc); return o->rc; }
static u32 __stdcall c_release(struct Obj *o) { lg(0xc20e); lg(o->id); if (o->rc) o->rc--; lg(o->rc); return o->rc; }
static u32 hashw(const u32 *p, u32 n) { u32 h = 2166136261u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 16777619u; return h; }
static void *v_sc[40];
static long __stdcall s_getdesc(struct Obj *o, u32 *d) {
  int k; d[0] = hrnd() % 70; d[1] = hrnd() % 4; d[2] = hrnd(); d[3] = hrnd() % 4; d[4] = hrnd() % 3; d[5] = hrnd() % 4; d[6] = hrnd() % 2000; d[7] = hrnd() % 1200;
  lg(0xc30c); lg(o->id); for (k = 0; k < 8; k++) lg(d[k]); return 0;
}
static long __stdcall sc_getbb(struct Obj *o, u32 i, u32 type, void **pp) { lg(0xc505); lg(o->id); lg(i); lg(type); *pp = mkobj(v_surf); lg(oid(*pp)); return 0; }
static long __stdcall d_getmode(void *self, u32 adapter, u32 *mode) { mode[0] = 640; mode[1] = 480; mode[2] = hrnd() % 100; mode[3] = 19 + hrnd() % 22; lg(0xd308); lg(adapter); lg(mode[3]); return 0; }
static long dh(void) { return hrnd() % 3 == 0 ? (long)0x8876086a : 0; }
static long __stdcall d_checktype(void *self, u32 a, u32 t, u32 df, u32 bf, u32 w) { long h = dh(); lg(0xd309); lg(a); lg(t); lg(df); lg(bf); lg(w); lg((u32)h); return h; }
static long __stdcall d_checkfmt(void *self, u32 a, u32 t, u32 af, u32 usage, u32 rt, u32 f) { long h = dh(); lg(0xd30a); lg(a); lg(t); lg(af); lg(usage); lg(rt); lg(f); lg((u32)h); return h; }
static long __stdcall d_depthmatch(void *self, u32 a, u32 t, u32 af, u32 bf, u32 df) { long h = dh(); lg(0xd30c); lg(a); lg(t); lg(af); lg(bf); lg(df); lg((u32)h); return h; }
static long __stdcall d_mscheck(void *self, u32 adapter, u32 type, u32 fmt, u32 win, u32 mst, u32 *q) {
  long h = hrnd() % 3 == 0 ? (long)0x8876086a : 0; lg(0xd30b); lg(adapter); lg(type); lg(fmt); lg(win); lg(mst); lg(q != 0); if (h >= 0 && q) *q = hrnd() % 6; lg((u32)h); if (q) lg(h >= 0 ? *q : 0x77); return h;
}
static long __stdcall c_getlevel(struct Obj *t, u32 level, void **pp) { lg(0xc318); lg(t->id); lg(level); *pp = mkobj(v_surf); lg(oid(*pp)); return 0; }
static long __stdcall c_getcube(struct Obj *t, u32 face, u32 level, void **pp) { lg(0xc319); lg(t->id); lg(face); lg(level); *pp = mkobj(v_surf); lg(oid(*pp)); return 0; }
static u32 __stdcall d_modecount(void *self, u32 adapter, u32 fmt);
static long __stdcall d_enummode(void *self, u32 adapter, u32 fmt, u32 mode, u32 *out);
static void init_vtbls(void) {
  int i;
  for (i = 0; i < 40; i++) v_surf[i] = v_tex2d[i] = v_texcube[i] = v_decl[i] = v_d3d[i] = v_sc[i] = ctraps[i];
  v_surf[1] = v_decl[1] = (void *)c_addref; v_surf[2] = v_decl[2] = v_sc[2] = (void *)c_release; v_surf[12] = (void *)s_getdesc; v_sc[5] = (void *)sc_getbb; v_d3d[8] = (void *)d_getmode; v_d3d[6] = (void *)d_modecount; v_d3d[7] = (void *)d_enummode;
  v_tex2d[18] = (void *)c_getlevel; v_texcube[18] = (void *)c_getcube;
  v_d3d[9] = (void *)d_checktype; v_d3d[10] = (void *)d_checkfmt; v_d3d[11] = (void *)d_mscheck; v_d3d[12] = (void *)d_depthmatch;
}
static struct Obj *d3dobj;
static void *devobj;

/* stand-ins for exe callees and the window rect import */
static int __stdcall s_getwindowrect(u32 hwnd, int32_t *rc) {
  rc[0] = hrnd() % 100; rc[1] = hrnd() % 100; rc[2] = rc[0] + (hrnd() % 4 == 0 ? 0 : hrnd() % 800); rc[3] = rc[1] + (hrnd() % 4 == 0 ? 0 : hrnd() % 600);
  lg(0xd10); lg(hwnd); lg((u32)rc[0]); lg((u32)rc[1]); lg((u32)rc[2]); lg((u32)rc[3]); return 1;
}
static u32 nofail;      /* rows that continue with the created rasters (CameraClear after the attach) need the creations not to fail */
static void *__cdecl h_rastercreate(u32 w, u32 h, u32 d, u32 f) {
  void *p = !nofail && hrnd() % 5 == 0 ? 0 : amalloc(0xa0); if (p) { memset(p, 0, 0xa0); *(u32 *)p = (u32)(uintptr_t)p; } lg(0xa4d7950); lg(w); lg(h); lg(d); lg(f); lg((u32)(uintptr_t)p); return p;
}
static void __cdecl h_rasterdestroy(void *r) { lg(0xa4d77e0); lg(aptr(r)); }
static void __cdecl h_fldestroy(u32 fl) { lg(0xa4e92b0); lg(fl); }
static void __cdecl h_rasterclose(void) { lg(0xa4dfbf0); }
static void __cdecl h_vbmclose(void) { lg(0xa4ecee0); }
static void __cdecl h_im2dclose(void) { lg(0xa4eb670); }
static void __cdecl h_relvidmem(void) { lg(0xa4d9c60); }
static int __cdecl h_rastlistrestore(void) { int r = hrnd() % 4 != 0; lg(0xa4e1be0); lg(r); return r; }
static int __cdecl h_dynvbrestore(void) { int r = hrnd() % 4 != 0; lg(0xa4eca90); lg(r); return r; }
static void __cdecl h_rsreset(void) { lg(0xa4e4960); }
static int __cdecl h_im2dopen(void) { int r = hrnd() % 4 != 0; lg(0xa4eb500); lg(r); return r; }
static int __cdecl h_im3dopen(void) { int r = hrnd() % 4 != 0; lg(0xa4f0a40); lg(r); return r; }
static void __cdecl h_restorecb(void) { lg(0xcb0cb0); }
static void __cdecl h_relcb(void) { lg(0xcb0cb1); }
static void __cdecl h_dynvbdiscard(void) { lg(0xa4ec9c0); }
static int __cdecl h_copyquad(u32 a, u32 b, u32 c) { int r = hrnd() % 4 != 0; lg(0xa4e0ab0); lg(aptr((void *)(uintptr_t)a)); lg(b); lg(c); lg(r); return r; }
static void *__cdecl h_imgcreate(u32 w, u32 h, u32 d) { void *p = hrnd() % 10 ? amalloc(0x40) : 0; lg(0xa4dd550); lg(w); lg(h); lg(d); lg(p != 0); return p; }
static void __cdecl h_imgalloc(void *img) { lg(0xa4dd610); lg(aptr(img)); }
static void __cdecl h_imgfromraster(void *img, void *parent, u32 z) { lg(0xa4ee2d0); lg(aptr(img)); lg(aptr(parent)); lg(z); }
static int __cdecl h_chktex(u32 fmt) { int r = hrnd() % 2; lg(0xa4e36a0); lg(fmt); lg(r); return r; }
static int __cdecl h_chkcube(u32 fmt) { int r = hrnd() % 2; lg(0xa4e3760); lg(fmt); lg(r); return r; }
static void __cdecl h_rdestroy2(u32 a, void *r, u32 b) { lg(0xa4e2e40); lg(a); lg(aptr(r)); lg(b); }
static void __cdecl h_rcreate2(u32 a, void *r, u32 flags) { lg(0xa4e20f0); lg(a); lg(aptr(r)); lg(flags); }
static void __cdecl h_setfromimage(void *r, void *img, u32 z) { lg(0xa4ef240); lg(aptr(r)); lg(aptr(img)); lg(z); }
static void __cdecl h_imgdestroy(void *img) { lg(0xa4dd5b0); lg(aptr(img)); }
static void __cdecl h_renderquad(u32 a, u32 b, u32 c, u32 d) { lg(0xa4e0560); lg(aptr((void *)(uintptr_t)a)); lg(b); lg(c); lg(d); }
static int __cdecl h_cleargeneric(const u32 *rect, u32 v) { int r = (int)hrnd(), k; lg(0xa4e0dd0); for (k = 0; k < 4; k++) lg(rect[k]); lg(v); lg((u32)r); return r; }
static void __cdecl h_errorset(const u32 *e) { lg(0xa4ea160); lg(e[0]); lg(e[1]); }
static int __cdecl h_errcode(u32 code, u32 extra) { int r = (int)hrnd(); lg(0xa53eac0); lg(code); if (code == 2) lg(extra); lg((u32)r); return r; }
static long __stdcall fd_setpal(void *self, u32 n, const u32 *p) { lg(0xde500000u | 71); lg(n); lg(aptr(p)); lg(hashw(p, 8)); return 0; }
static void *__cdecl h_framegetltm(void *frame) { static u8 m[64]; lg(0xa4d0ff0); lg(aptr(frame)); return m; }
static void __cdecl h_matinvert(float *dst, void *src) { int k; for (k = 0; k < 16; k++) ((u32 *)dst)[k] = (hrnd() & 0x807fffffu) | ((120 + hrnd() % 16) << 23); lg(0xa4d4490); lg(aptr(src)); }
static void __cdecl h_settransform(u32 state, u32 *m) { int k; lg(0xa4db470); lg(state); for (k = 0; k < 16; k++) lg(m[k]); }
static void __cdecl h_setrenderstate(u32 state, u32 value) { lg(0xa4e3c00); lg(state); lg(value); }
static int __stdcall s_getclientrect(u32 hwnd, int32_t *rc) {
  static const int32_t w[] = {640, 800, 640, 800, 0}, h[] = {480, 600, 480, 600, 0}; u32 a = hrnd() % 5, b = hrnd() % 5;
  rc[0] = hrnd() % 3; rc[1] = hrnd() % 3; rc[2] = hrnd() % 8 == 0 ? 0 : w[a]; rc[3] = hrnd() % 8 == 0 ? 0 : h[b];
  lg(0xd2b4); lg(hwnd); lg((u32)rc[0]); lg((u32)rc[1]); lg((u32)rc[2]); lg((u32)rc[3]); return 1;
}
static int __stdcall s_isiconic(u32 hwnd) { int r = hrnd() % 6 == 0; lg(0xd238); lg(hwnd); lg(r); return r; }
static u32 __stdcall s_getwindowlong(u32 hwnd, int idx) { u32 v = hrnd() & 0xff0000ffu; if (hrnd() % 2) v |= 0x1000000; else v &= ~0x1000000u; lg(0xd23c); lg(hwnd); lg((u32)idx); lg(v); return v; }
static u32 __stdcall s_setwindowlong(u32 hwnd, int idx, u32 v) { lg(0xd294); lg(hwnd); lg((u32)idx); lg(v); return 0; }
static u32 __stdcall s_getmenu(u32 hwnd) { u32 r = hrnd() % 2 ? 0 : 0x77; lg(0xd268); lg(hwnd); lg(r); return r; }
static int __stdcall s_adjustrect(int32_t *rc, u32 style, int menu, u32 ex) {
  lg(0xd240); lg((u32)rc[0]); lg((u32)rc[1]); lg((u32)rc[2]); lg((u32)rc[3]); lg(style); lg(menu); lg(ex); rc[0] -= 3; rc[1] -= 20; rc[2] += 3; rc[3] += 3; return 1;
}
static int __stdcall s_setwindowpos(u32 hwnd, u32 after, int x, int y, int cx, int cy, u32 fl) { lg(0xd298); lg(hwnd); lg(after); lg((u32)x); lg((u32)y); lg((u32)cx); lg((u32)cy); lg(fl); return 1; }
static long __stdcall fd_setvp(void *self, const u32 *vp) { int k; lg(0xde500000u | 47); for (k = 0; k < 6; k++) lg(vp[k]); return 0; }
static long __stdcall fd_clear(void *self, u32 count, void *rects, u32 flags, u32 color, u32 z, u32 stencil) {
  lg(0xde500000u | 43); lg(count); lg((u32)(uintptr_t)rects); lg(flags); lg(color); lg(z); lg(stencil); return 0;
}
static long __stdcall fd_stretch(void *self, struct Obj *src, const int32_t *sr, struct Obj *dst, const int32_t *dr, u32 filter) {
  int k; lg(0xde500000u | 34); lg(oid(src)); for (k = 0; k < 4; k++) lg((u32)sr[k]); lg(oid(dst)); for (k = 0; k < 4; k++) lg((u32)dr[k]); lg(sr == dr); lg(filter); return 0;
}
static long pres_hr(void) { static const long v[] = {0, 0, (long)0x88760868, (long)0x88760868, (long)0x8876086c, 1}; return v[hrnd() % 6]; }
static long __stdcall fd_present(void *self, u32 a, u32 b, u32 c, u32 d) { long h = pres_hr(); lg(0xde500000u | 17); lg(a); lg(b); lg(c); lg(d); lg((u32)h); return h; }
static long __stdcall fd_endscene(void *self) { lg(0xde500000u | 42); return 0; }
static long __stdcall sc_present(struct Obj *o, u32 a, u32 b, u32 c, u32 d, u32 e) { long h = pres_hr(); lg(0xc503); lg(o->id); lg(a); lg(b); lg(c); lg(d); lg(e); lg((u32)h); return h; }
static void __cdecl h_im3dclose(void) { lg(0xa4f09d0); }
static void __cdecl h_dynvbrel(void) { lg(0xa4eca00); }
static void __cdecl h_rastlistrel(void) { lg(0xa4e1b20); }
static void __cdecl h_flpurgeall(void) { lg(0xa4e96a0); }
static void *__cdecl h_flcreate(u32 size, u32 a, u32 b, u32 hint) { void *p = amalloc(0x20); lg(0xa4e90b); lg(size); lg(a); lg(b); lg(hint); lg((u32)(uintptr_t)p); return p; }
static void __cdecl h_flpurge(void *fl) { lg(0xa4e951); lg(aptr(fl)); }
static u32 __stdcall d_modecount(void *self, u32 adapter, u32 fmt) { u32 n = hrnd() % 5; lg(0xd306); lg(adapter); lg(fmt); lg(n); return n; }
static long __stdcall d_enummode(void *self, u32 adapter, u32 fmt, u32 mode, u32 *out) {
  static const u32 rates[] = {0, 24, 30, 50, 59, 60, 70, 75, 85, 100, 120, 144};
  out[0] = hrnd() % 2 ? 640 : 800; out[1] = hrnd() % 2 ? 480 : 600; out[2] = rates[hrnd() % 12]; out[3] = fmt; lg(0xd307); lg(adapter); lg(fmt); lg(mode); lg(out[0]); lg(out[1]); lg(out[2]); return 0;
}

/* device recorders */
static long __stdcall fd_release(struct Obj *o) { lg(0xde500000u | 2); return 0; }
static long __stdcall fd_addswap(void *self, const u32 *pp, void **psc) {
  int k; long h = !nofail && hrnd() % 6 == 0 ? (hrnd() % 2 ? (long)0x8876086c : 1) : 0; lg(0xde500000u | 13); for (k = 0; k < 14; k++) lg(pp[k]); lg((u32)h);
  if (h == 0) { *psc = mkobj(v_sc); lg(oid(*psc)); } return h;
}
static long __stdcall fd_testcoop(void *self) {
  static const long v[] = {0, 0, (long)0x88760869, (long)0x88760869, (long)0x88760869, (long)0x88760868, 1};
  long h = v[hrnd() % 7]; lg(0xde500000u | 3); lg((u32)h); return h;
}
static long __stdcall fd_reset(void *self, const u32 *pp) {
  static const long v[] = {0, 0, (long)0x8876086c, (long)0x8876086c, (long)0x88760868, 1};
  long h = v[hrnd() % 6]; lg(0xde500000u | 16); lg(hashw(pp, 14)); lg((u32)h); return h;
}
static long __stdcall fd_getrt(void *self, u32 i, void **pp) { lg(0xde500000u | 38); lg(i); *pp = mkobj(v_surf); lg(oid(*pp)); return 0; }
static long __stdcall fd_getds(void *self, void **pp) { lg(0xde500000u | 40); *pp = mkobj(v_surf); lg(oid(*pp)); return 0; }
static long __stdcall fd_beginscene(void *self) { long h = hrnd() % 4 == 0 ? (long)0x8876086c : 0; lg(0xde500000u | 41); lg((u32)h); return h; }
static long __stdcall fd_setrt(void *self, u32 index, void *surf) { long h = hrnd() % 5 == 0 ? (long)0x8876086c : 0; lg(0xde500000u | 37); lg(index); lg(aptr(surf)); lg((u32)h); return h; }
static long __stdcall fd_setds(void *self, void *surf) { lg(0xde500000u | 39); lg(aptr(surf)); return 0; }
static long __stdcall fd_lightenable(void *self, u32 i, int32_t e) { long h = hrnd() % 6 == 0 ? (long)0x8876086c : 0; lg(0xde500000u | 53); lg(i); lg((u32)e); lg((u32)h); return h; }
static long __stdcall fd_createdecl(void *self, const u8 *elems, void **pp) {
  u32 n = 0; long h = hrnd() % 5 == 0 ? (long)0x8876086c : 0;
  while (elems[4 + 8 * n] != 0x11) n++;
  lg(0xde500000u | 86); lg(n + 1); lg(hashw((const u32 *)elems, 2 * (n + 1))); lg((u32)h);
  if (h >= 0) { *pp = mkobj(v_decl); lg(oid(*pp)); }
  return h;
}

enum { CVD, IB32, ATTW, STOP, BSCN, RELVM, STDS, DML, SHOWR, SPP, CLR, BUPD, RCTOR, RDTOR, RCLOSE, RLOCK, RUNLOCK, RFAST, RSCALED, RCLRECT, RCLEAR, RSETCTX, RGEN, CTNP, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_RwD3D9CreateVertexDeclaration", 0x4db9e0}, {"rw37_rwD3D9IndexBuffer32bitsCreate", 0x4db9b0},
  {"rw37_RwD3D9CameraAttachWindow", 0x4da0f0}, {"rw37_D3D9DeviceSystemStop", 0x4d8e00}, {"rw37_rwD3D9BeginScene", 0x4d9e60}, {"rw37_D3D9DeviceReleaseVideoMemory", 0x4d9c60},
  {"rw37_D3D9DeviceSystemStandards", 0x4d8fe0}, {"rw37_D3D9CreateDisplayModesList", 0x4d9230}, {"rw37_rwD3D9RasterShowRaster", 0x4dad50},
  {"rw37_D3D9SetPresentParameters", 0x4d8990}, {"rw37_rwD3D9CameraClear", 0x4d9420},
  {"rw37_rwD3D9CameraBeginUpdate", 0x4da2a0}, {"rw37_rwD3D9RasterCtor", 0x4dfb80}, {"rw37_rwD3D9RasterDtor", 0x4dfba0}, {"rw37_rwD3D9RasterClose", 0x4dfbf0},
  {"rw37_rwD3D9RasterLockPalette", 0x4e02d0}, {"rw37_rwD3D9RasterUnlockPalette", 0x4e0360}, {"rw37_rwD3D9RasterRenderFast", 0x4e0d70}, {"rw37_rwD3D9RasterRenderScaled", 0x4e0d90},
  {"rw37_rwD3D9RasterClearRect", 0x4e0db0}, {"rw37_rwD3D9RasterClear", 0x4e13a0}, {"rw37_rwD3D9SetRasterContext", 0x4e13e0},
  {"rw37_rwD3D9RasterRenderGeneric", 0x4e0410}, {"rw37__rwD3D9RasterConvertToNonPalettized", 0x4e38b0}};
/* the standards table of the exe: the stub, then the 27 functions in the order of the original's constant pairs */
static const u32 stdfns[28] = {0x405e40, 0x4da2a0, 0x4dac70, 0x4d9420, 0x4dad50, 0x4ede80, 0x4ee0d0, 0x4ef240, 0x4ee2d0, 0x4e2e40, 0x4e20f0, 0x4ef150, 0x4e3280, 0x4dfcd0, 0x4e0040,
                               0x4e02d0, 0x4e0360, 0x4e13a0, 0x4e0db0, 0x4e03f0, 0x4e0d90, 0x4e0d70, 0x4e13e0, 0x4e3290, 0x4ed270, 0x4ed400, 0x4ed780, 0x4e3200};
typedef u32 (__cdecl *G4)(u32, u32, u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32);
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32);
static void *fn[2][NF]; static long ncalls[NF];
#define EXTOFF 0x40

static void reset(u32 seed) {
  nlg = 0; ap = 0; nblk = 0; nobj = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; EW(0x108) = (u32)(uintptr_t)s_malloc; EW(0x10c) = (u32)(uintptr_t)s_free; EW(0x110) = (u32)(uintptr_t)s_realloc;
  EW(0x118) = (u32)(uintptr_t)e_flalloc; EW(0x11c) = (u32)(uintptr_t)e_flfree;
  { u32 a; for (a = 0x7d21f0; a < 0x7d26c0; a += 4) W(a) = 0; for (a = 0x62e748; a < 0x62e774; a += 4) W(a) = 0; }      /* exe state of the rows: the runs must start equal */
  W(0x9473c0) = 0; W(0x9475f0) = 0; W(0x9475f8) = 0; W(0x9476f4) = 0;
  W(0x947144) = EXTOFF;
  { u32 a; for (a = 0x947740; a < 0x947780; a += 4) W(a) = 0; for (a = 0x947160; a < 0x947260; a += 4) W(a) = rnd(); }      /* pixel format info table: filled at run time in the game */
  W(0x9475f4) = 0; W(0x7d26cc) = 0; W(0x94a004) = 0x27f; W(0x5dd244) = (u32)(uintptr_t)s_getwindowrect;
  fakedev_log = sink; fakedev_hr = 0; devobj = fakedev_init(); W(DEVVAR) = (u32)(uintptr_t)devobj;
  fakedev_vtbl[86] = (void *)fd_createdecl; fakedev_vtbl[2] = (void *)fd_release; fakedev_vtbl[13] = (void *)fd_addswap; fakedev_vtbl[3] = (void *)fd_testcoop;
  fakedev_vtbl[16] = (void *)fd_reset; fakedev_vtbl[38] = (void *)fd_getrt; fakedev_vtbl[40] = (void *)fd_getds; fakedev_vtbl[41] = (void *)fd_beginscene; fakedev_vtbl[53] = (void *)fd_lightenable;
  W(0x5dd2b4) = (u32)(uintptr_t)s_getclientrect; W(0x5dd238) = (u32)(uintptr_t)s_isiconic; W(0x5dd23c) = (u32)(uintptr_t)s_getwindowlong; W(0x5dd294) = (u32)(uintptr_t)s_setwindowlong;
  W(0x5dd268) = (u32)(uintptr_t)s_getmenu; W(0x5dd240) = (u32)(uintptr_t)s_adjustrect; W(0x5dd298) = (u32)(uintptr_t)s_setwindowpos; W(0x9475e4) = 0; W(0x9475e8) = 0;
  { u32 a; for (a = 0x947380; a < 0x9473c4; a += 4) W(a) = 0; for (a = 0x62e778; a < 0x62e7b8; a += 4) W(a) = 0; }
  { u32 a; for (a = 0x7d27f0; a < 0x7d2814; a += 4) W(a) = 0; }
  fakedev_vtbl[71] = (void *)fd_setpal; fakedev_vtbl[47] = (void *)fd_setvp; fakedev_vtbl[43] = (void *)fd_clear; fakedev_vtbl[34] = (void *)fd_stretch;
  fakedev_vtbl[17] = (void *)fd_present; fakedev_vtbl[42] = (void *)fd_endscene; v_sc[3] = (void *)sc_present; W(0x947774) = 0;
  fakedev_vtbl[37] = (void *)fd_setrt; fakedev_vtbl[39] = (void *)fd_setds; W(0x7d26c8) = 0; W(0x9475e0) = 0;
  d3dobj = mkobj(v_d3d); W(0x7d2238) = (u32)(uintptr_t)d3dobj;
}
static void *mktexobj(u32 al) { return mkobj((al & 0xf) ? v_texcube : v_tex2d); }
/* a frame buffer raster: [0] = parent (itself or a separate block that holds the type, sizes and extension), [0xc] / [0x10] size, +0x1c / +0x1e int16 viewport origin */
static void *mkfbraster2(u32 hwnd, u32 type, u32 withsc, u32 own) {
  u8 *r = amalloc(0xa0), *p = own ? r : amalloc(0xa0); static const u32 w[] = {640, 800}, h[] = {480, 600}; u32 al = rnd() % 3 == 0 ? ((rnd() % 6) << 4) | (1 + rnd() % 15) : (rnd() & 0xf0);
  memset(r, 0, 0xa0); if (!own) memset(p, 0, 0xa0);
  *(u32 *)r = (u32)(uintptr_t)p; *(u32 *)(r + 0xc) = w[rnd() % 2]; *(u32 *)(r + 0x10) = h[rnd() % 2];
  *(u16 *)(r + 0x1c) = (u16)(rnd() % 4 ? rnd() % 50 : 0xfff0 + rnd() % 8); *(u16 *)(r + 0x1e) = (u16)(rnd() % 4 ? rnd() % 50 : 0xfff0 + rnd() % 8);
  p[0x20] = (u8)type; *(u32 *)(p + 0x28) = w[rnd() % 2]; *(u32 *)(p + 0x2c) = h[rnd() % 2];
  p[EXTOFF + 9] = (u8)al; *(u32 *)(p + EXTOFF) = (u32)(uintptr_t)(type == 5 ? mktexobj(al) : (void *)mkobj(v_surf));
  *(u32 *)(p + EXTOFF + 0x1c) = withsc ? (u32)(uintptr_t)mkobj(v_sc) : 0; *(u32 *)(p + EXTOFF + 0x20) = hwnd;
  return r;
}
static void *mkfbraster(u32 hwnd) { return mkfbraster2(hwnd, 5, 1, 1); }
/* a camera (0x90 bytes, +0x60 frame buffer raster, +0x64 z raster) and the exe state around it for CameraClear / CameraBeginUpdate */
static u8 *setup_cam(void) {
  static const u32 sz[] = {640, 800, 0}, sh[] = {480, 600, 0}; u8 *cam = amalloc(0x90), *lc = amalloc(0x6c * 4), *fb, *z; u32 mode = rnd() % 3, hw = 0x1000 + rnd() % 9, k;
  memset(cam, 0, 0x90); for (k = 0; k < 0x6c; k++) lc[k] = (u8)rnd();
  fb = mkfbraster2(hw + rnd() % 2, mode == 2 ? (rnd() % 8 == 5 ? 3 : rnd() % 8) : 5, mode == 0, rnd() % 2);
  *(u32 *)(cam + 0x60) = (u32)(uintptr_t)fb;
  if (rnd() % 3) {
    u8 *zp = amalloc(0xa0); memset(zp, 0, 0xa0); z = amalloc(0xa0); memset(z, 0, 0xa0);
    *(u32 *)z = rnd() % 2 ? (u32)(uintptr_t)z : (u32)(uintptr_t)zp; zp[0x20] = rnd() % 3 ? 1 : 5; *(u32 *)(z + 0xc) = sz[rnd() % 2]; *(u32 *)(z + 0x10) = sh[rnd() % 2];
    if (*(u32 *)z == (u32)(uintptr_t)z) { zp = z; zp[0x20] = rnd() % 3 ? 1 : 5; }
    *(u32 *)(zp + EXTOFF) = (u32)(uintptr_t)(zp[0x20] == 1 ? (void *)mkobj(v_surf) : (void *)mkobj(v_tex2d));
    *(u32 *)(cam + 0x64) = (u32)(uintptr_t)z;
  }
  for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd();
  W(0x7d2234) = hw; W(0x9475e4) = rnd() % 2 ? 640 : 800; W(0x9475e8) = rnd() % 2 ? 480 : 600; W(0x947760) = rnd() % 2; W(0x7d225c) = rnd(); W(0x7d2254) = rnd() % 2; W(0x9476f4) = rnd() % 2 ? 0x1000100 : rnd() & 0xff;
  W(0x7d2248) = (u32)(uintptr_t)mkobj(v_surf); W(0x7d2244) = (u32)(uintptr_t)mkobj(v_surf);
  for (k = 0; k < 4; k++) W(0x7d26a8 + 4 * k) = rnd() % 3 == 0 ? W(0x7d2248) : rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf);
  W(0x7d26a4) = rnd() % 3 == 0 ? W(0x7d2244) : rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf);
  W(0x7d269c) = rnd() % 6 == 0 ? (u32)-1 : rnd() % 4; W(0x7d26a0) = (u32)(uintptr_t)lc; W(0x7d26cc) = rnd() % 2 ? (u32)(uintptr_t)h_restorecb : 0;
  W(0x7d223c) = rnd() % 3; W(0x7d2238) = (u32)(uintptr_t)d3dobj;
  return cam;
}
static void lg_decltab(void) {
  u32 n = W(0x7d2274), i; u8 *t = (u8 *)(uintptr_t)W(0x7d227c);
  lg(n); lg(W(0x7d2278)); lg(t != 0);
  for (i = 0; i < n; i++) { u32 c = *(u32 *)(t + 12 * i + 4); lg(oid((void *)(uintptr_t)*(u32 *)(t + 12 * i))); lg(c); lg(hashw(*(u32 **)(t + 12 * i + 8), 2 * c)); }
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 18, k, r = 0;      /* the ops, not the rows: 12 = Ctor / Dtor, 14 = Lock / Unlock, 15 = the small raster wrappers */
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: {      /* RwD3D9CreateVertexDeclaration */
      u32 m = 1 + rnd() % 4, cnt = m + 1, n = rnd() % 6, i, mx; u8 *el = amalloc(8 * cnt), *tab = 0; void *out = (void *)0x1234;
      for (i = 0; i < m; i++) { *(u16 *)(el + 8 * i) = rnd() % 2; *(u16 *)(el + 8 * i + 2) = 4 * i; el[8 * i + 4] = rnd() % 16; el[8 * i + 5] = 0; el[8 * i + 6] = rnd() % 6; el[8 * i + 7] = rnd() % 2; }
      *(u16 *)(el + 8 * m) = 0xff; *(u16 *)(el + 8 * m + 2) = 0; el[8 * m + 4] = 0x11; el[8 * m + 5] = 0; el[8 * m + 6] = 0; el[8 * m + 7] = 0;
      mx = n + (rnd() % 3 == 0 ? 0 : rnd() % 3 == 0 ? 1 : 3);
      if (n || rnd() % 2) tab = amalloc(12 * (mx ? mx : 1)); else mx = 0;
      for (i = 0; i < n; i++) {
        u32 c = rnd() % 4 ? cnt : 1 + rnd() % 5; u8 *cp = amalloc(8 * c); u32 j;
        for (j = 0; j < 8 * c; j++) cp[j] = j < 8 * cnt && c == cnt && rnd() % 8 ? el[j] : (u8)rnd();
        if (c == cnt && rnd() % 3) { memcpy(cp, el, 8 * cnt); if (rnd() % 4 == 0) cp[rnd() % (8 * cnt)] ^= 1 << (rnd() % 8); }
        *(u32 *)(tab + 12 * i) = rnd() % 4 == 0 ? 0 : (u32)(uintptr_t)mkobj(v_decl); *(u32 *)(tab + 12 * i + 4) = c; *(u32 *)(tab + 12 * i + 8) = (u32)(uintptr_t)cp;
      }
      W(0x7d2274) = n; W(0x7d2278) = mx; W(0x7d227c) = (u32)(uintptr_t)tab;
      r = ((G2)f[CVD])((u32)(uintptr_t)el, (u32)(uintptr_t)&out); ncalls[CVD]++; lg(r); lg(oid(out == (void *)0x1234 ? 0 : out)); lg(out == (void *)0x1234); lg_decltab(); break; }
    case 1: {      /* rwD3D9IndexBuffer32bitsCreate */
      void *out = (void *)0x1234; fakedev_createhr = rnd() % 4 == 0 ? (long)0x8876086c : 0;
      r = ((G2)f[IB32])(rnd() % 100000, (u32)(uintptr_t)&out); ncalls[IB32]++; lg(r); lg(out == (void *)0x1234 ? 0x1234 : out ? ((struct fakedev_buf *)out)->id : 0); break; }
    case 2: {      /* RwD3D9CameraAttachWindow */
      u8 *cam = amalloc(0x80); u32 hwnd = 0x1000 + rnd() % 50;
      memset(cam, 0, 0x80);
      if (rnd() % 2) *(u32 *)(cam + 0x60) = (u32)(uintptr_t)amalloc(0xa0);
      if (rnd() % 2) *(u32 *)(cam + 0x64) = (u32)(uintptr_t)amalloc(0xa0);
      for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd();
      W(0x947760) = rnd() % 5 != 0; W(0x7d2234) = rnd() % 6 == 0 ? hwnd : 0x2000;
      r = ((G2)f[ATTW])((u32)(uintptr_t)cam, hwnd); ncalls[ATTW]++; lg(r); lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x60))); lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x64)));
      { u8 *ras = (u8 *)(uintptr_t)*(u32 *)(cam + 0x60); if (ras) lg(hashw((u32 *)ras, 0xa0 / 4)); }
      break; }
    case 3: {      /* D3D9DeviceSystemStop */
      static const u32 mx[] = {0x1f80, 0x9f80, 0x1fc0, 0x9fc0, 0x3f80, 0x5f80}, cws[] = {0x27f, 0x37f, 0x7f, 0x137f}; u32 n = rnd() % 5, i, cw, mxo, *lm = (u32 *)(uintptr_t)0x7d2288;
      u8 *tab = n ? amalloc(12 * n) : (rnd() % 2 ? amalloc(12) : 0); u32 nd = rnd() % 3;
      W(0x7d2250) = 1; W(0x7d2260) = rnd() % 2 ? (u32)(uintptr_t)amalloc(0x40) : 0; W(0x7d2258) = rnd() % 9;
      W(0x94a004) = cws[rnd() % 4]; W(0x7d26bc) = rnd() % 2; { u32 x = mx[rnd() % 6]; __asm__ volatile("ldmxcsr %0" :: "m"(x)); }
      for (k = 0; k < 0x104; k++) lm[k] = rnd() % 10 == 0 ? (u32)(uintptr_t)amalloc(0x40) : 0;
      W(0x7d2698) = rnd() % 4 ? (u32)(uintptr_t)amalloc(0x20) : 0; W(0x7d269c) = rnd() % 5; W(0x7d26a0) = rnd() % 2 ? (u32)(uintptr_t)amalloc(0x20) : 0;
      for (i = 0; i < n; i++) { *(u32 *)(tab + 12 * i) = rnd() % 4 ? (u32)(uintptr_t)mkobj(v_decl) : 0; *(u32 *)(tab + 12 * i + 4) = rnd() % 5; *(u32 *)(tab + 12 * i + 8) = rnd() % 3 ? (u32)(uintptr_t)amalloc(0x20) : 0; }
      W(0x7d2274) = n; W(0x7d2278) = n + nd; W(0x7d227c) = (u32)(uintptr_t)tab;
      r = ((G0)f[STOP])(); ncalls[STOP]++; lg(r);
      __asm__ volatile("fnstcw %0" : "=m"(cw)); __asm__ volatile("stmxcsr %0" : "=m"(mxo)); lg(cw & 0xffff); lg(mxo);
      cw = 0x27f; __asm__ volatile("fldcw %0" :: "m"(cw)); mxo = 0x1f80; __asm__ volatile("ldmxcsr %0" :: "m"(mxo));
      lg(W(0x7d2250)); lg(W(0x7d2260)); lg(W(0x7d2258)); lg(W(0x7d2698)); lg(W(0x7d269c)); lg(W(0x7d26a0)); lg(hashw(lm, 0x104)); lg(W(0x7d2274)); lg(W(0x7d2278)); lg(W(0x7d227c)); lg(W(DEVVAR));
      W(DEVVAR) = (u32)(uintptr_t)devobj; break; }
    case 4: {      /* rwD3D9BeginScene */
      u8 *lc = amalloc(0x6c * 4);
      for (k = 0; k < 0x6c; k++) lc[k] = (u8)rnd();
      for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd();
      W(0x947760) = rnd() % 4 != 0; W(0x7d223c) = rnd() % 3; W(0x7d2270) = rnd() % 3; W(0x7d269c) = rnd() % 6 == 0 ? (u32)-1 : rnd() % 4; W(0x7d26a0) = (u32)(uintptr_t)lc;
      W(0x7d26cc) = rnd() % 2 ? (u32)(uintptr_t)h_restorecb : 0; W(0x7d2238) = (u32)(uintptr_t)d3dobj;
      r = ((G0)f[BSCN])(); ncalls[BSCN]++; lg(r); lg(W(0x7d2270)); lg(W(0x9475f0)); lg(W(0x9475f4)); lg(W(0x947748)); lg(oid((void *)(uintptr_t)W(0x7d2248))); lg(oid((void *)(uintptr_t)W(0x7d2244)));
      break; }
    case 5: {      /* D3D9DeviceReleaseVideoMemory */
      void *fl = rnd() % 3 ? amalloc(0x20) : 0; u32 *lm = (u32 *)(uintptr_t)0x7d2288, *cs = (u32 *)(uintptr_t)0x7d26a8; u32 rsurf = (u32)(uintptr_t)mkobj(v_surf), dsurf = (u32)(uintptr_t)mkobj(v_surf);
      for (k = 0; k < 5; k++) W(0x62e760 + 4 * k) = rnd();
      for (k = 0; k < 15; k++) W(0x7d21f0 + 4 * k) = rnd();
      W(0x7d2698) = (u32)(uintptr_t)fl; W(0x7d2284) = rnd(); for (k = 0; k < 0x104; k++) lm[k] = rnd() % 8 == 0 ? (u32)(uintptr_t)amalloc(0x40) : 0;
      W(0x7d2688) = rnd(); W(0x7d2248) = rsurf; W(0x7d2244) = dsurf;
      for (k = 0; k < 4; k++) cs[k] = rnd() % 3 == 0 ? rsurf : rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf);
      W(0x7d26a4) = rnd() % 3 == 0 ? dsurf : rnd() % 2 ? 0 : (u32)(uintptr_t)mkobj(v_surf); W(0x7d26c8) = rnd() % 2 ? (u32)(uintptr_t)h_relcb : 0;
      r = ((G0)f[RELVM])(); ncalls[RELVM]++; lg(r);
      for (k = 0; k < 5; k++) lg(W(0x62e760 + 4 * k)); for (k = 0; k < 15; k++) lg(W(0x7d21f0 + 4 * k));
      lg(W(0x7d2698)); lg(W(0x7d2284)); lg(hashw(lm, 0x104)); lg(W(0x7d2688)); lg(hashw((u32 *)(uintptr_t)W(0x7d2688), 16)); for (k = 0; k < 4; k++) lg(cs[k]); lg(W(0x7d26a4));
      break; }
    case 6: {      /* D3D9DeviceSystemStandards */
      static const u32 cnts[] = {(u32)-1, 0, 1, 2, 3, 5, 30, 40}; u32 *tab = amalloc(64 * 4), i, cnt = cnts[rnd() % 8];
      for (i = 0; i < 64; i++) tab[i] = rnd() % 2 ? rnd() : 0;
      r = ((G3)f[STDS])((u32)(uintptr_t)tab, rnd(), cnt); ncalls[STDS]++; lg(r); lg(hashw(tab, 64)); break; }
    case 12: {     /* rwD3D9RasterCtor / Dtor */
      u8 *ras = amalloc(0xa0); memset(ras, 0, 0xa0); *(u32 *)(ras + EXTOFF + 0x1c) = rnd() % 2 ? (u32)(uintptr_t)mkobj(v_sc) : 0;
      if (rnd() % 2) { r = ((G1)f[RCTOR])((u32)(uintptr_t)ras); ncalls[RCTOR]++; }
      else { r = ((G1)f[RDTOR])((u32)(uintptr_t)ras); ncalls[RDTOR]++; }
      lg(aptr((void *)(uintptr_t)r)); lg(*(u32 *)(ras + EXTOFF + 0x1c)); break; }
    case 13: {     /* rwD3D9RasterClose */
      u32 n = rnd() % 5, i, prev = 0;
      for (i = 0; i < n; i++) { u32 *e = amalloc(8); e[0] = rnd(); e[1] = prev; prev = (u32)(uintptr_t)e; }
      W(0x7d2808) = prev; W(0x7d280c) = rnd() % 3 ? (u32)(uintptr_t)amalloc(0x20) : 0; W(0x7d2800) = rnd() % 2 ? (u32)(uintptr_t)amalloc(0x20) : 0;
      W(0x7d27f4) = rnd(); W(0x7d27f8) = rnd(); W(0x7d27fc) = rnd(); W(0x7d2804) = rnd() % 2 ? (u32)(uintptr_t)amalloc(0x20) : 0;
      ((G0)f[RCLOSE])(); ncalls[RCLOSE]++; for (k = 0; k < 7; k++) lg(W(0x7d27f4 + 4 * k)); lg(W(0x7d280c)); lg(W(0x7d2808)); break; }
    case 14: {     /* rwD3D9RasterLockPalette / UnlockPalette */
      u8 *ras = amalloc(0xa0), *pal = amalloc(0x410); u32 out = 0x1234, lock = rnd() % 2;
      memset(ras, 0, 0xa0); for (k = 0; k < 0x410; k++) pal[k] = (u8)rnd();
      ras[0x20] = (u8)rnd(); ras[0x22] = (u8)rnd(); *(u32 *)(ras + 8) = rnd(); *(u32 *)(ras + EXTOFF + 4) = (u32)(uintptr_t)pal; *(u32 *)(pal + 0x400) = rnd() % 8;
      if (lock) { r = ((G3)f[RLOCK])((u32)(uintptr_t)&out, (u32)(uintptr_t)ras, rnd()); ncalls[RLOCK]++; lg(aptr((void *)(uintptr_t)out)); }
      else { r = ((G3)f[RUNLOCK])(rnd(), (u32)(uintptr_t)ras, rnd()); ncalls[RUNLOCK]++; }
      lg(r); lg(*(u32 *)(ras + 8) != 0); lg(ras[0x22]); break; }
    case 17: {     /* _rwD3D9RasterConvertToNonPalettized */
      static const u32 dep[] = {0, 0x10, 0x20}; u8 *ras = amalloc(0xa0);
      memset(ras, 0, 0xa0); *(u32 *)ras = (u32)(uintptr_t)amalloc(0x40); *(u32 *)(ras + 0xc) = 64 << (rnd() % 4); *(u32 *)(ras + 0x10) = 32 << (rnd() % 4);
      ras[0x20] = (u8)(rnd() % 3 ? rnd() % 8 : rnd()); ras[EXTOFF + 8] = (u8)(rnd() % 2 ? rnd() % 4 : 0); ras[EXTOFF + 9] = (u8)(rnd() % 3 ? rnd() & 0xf0 : rnd());
      W(0x9475f4) = dep[rnd() % 3]; W(0x9475f0) = 20 + rnd() % 8; W(0x947768) = 0x40 + rnd() % 16; W(0x62e748) = rnd() % 4; W(0x7d223c) = rnd() % 3; W(0x7d2238) = (u32)(uintptr_t)d3dobj;
      ((G1)f[CTNP])((u32)(uintptr_t)ras); ncalls[CTNP]++; break; }
    case 16: {     /* rwD3D9RasterRenderGeneric */
      u32 *tgt = amalloc(0x40), *src = rnd() % 10 ? amalloc(0x40) : 0;
      for (k = 0; k < 16; k++) tgt[k] = rnd(); if (src) for (k = 0; k < 16; k++) src[k] = rnd();
      ((u8 *)tgt)[0x20] = (u8)(rnd() % 8); if (src) ((u8 *)src)[0x20] = (u8)(rnd() % 3 ? rnd() % 8 : rnd()); W(0x7d27f0) = (u32)(uintptr_t)tgt;
      r = ((G4)f[RGEN])((u32)(uintptr_t)src, rnd(), rnd(), rnd()); ncalls[RGEN]++; lg(r); break; }
    case 15: {     /* RenderFast / RenderScaled / ClearRect / RasterClear / SetRasterContext */
      u32 w = rnd() % 5, *rect = amalloc(16), *tgt = amalloc(0x40), *src = rnd() % 10 ? amalloc(0x40) : 0;
      for (k = 0; k < 4; k++) rect[k] = rnd(); for (k = 0; k < 16; k++) tgt[k] = rnd(); if (src) for (k = 0; k < 16; k++) src[k] = rnd();
      ((u8 *)tgt)[0x20] = (u8)(rnd() % 8); if (src) ((u8 *)src)[0x20] = (u8)(rnd() % 8); W(0x7d27f0) = (u32)(uintptr_t)tgt;
      switch (w) {
      case 0: r = ((G2)f[RFAST])((u32)(uintptr_t)src, rnd()); ncalls[RFAST]++; break;
      case 1: r = ((G2)f[RSCALED])((u32)(uintptr_t)src, rnd()); ncalls[RSCALED]++; break;
      case 2: r = ((G3)f[RCLRECT])(rnd(), (u32)(uintptr_t)rect, rnd()); ncalls[RCLRECT]++; break;
      case 3: r = ((G3)f[RCLEAR])(rnd(), rnd(), rnd()); ncalls[RCLEAR]++; break;
      default: r = ((G2)f[RSETCTX])(rnd(), (u32)(uintptr_t)rect); ncalls[RSETCTX]++; lg(aptr((void *)(uintptr_t)W(0x7d27f0))); break; }
      lg(r); break; }
    case 11: {     /* rwD3D9CameraBeginUpdate */
      u8 *cam = setup_cam(); u32 cw, bits;
      *(u32 *)(cam + 4) = (u32)(uintptr_t)amalloc(0x40); *(u32 *)(cam + 0x14) = rnd() % 3 == 0 ? 2 : 1;
      for (k = 0; k < 6; k++) { bits = (rnd() & 0x807fffffu) | ((120 + rnd() % 16) << 23); *(u32 *)(cam + 0x70 + 4 * k) = bits; }
      if (rnd() % 8 == 0) *(u32 *)(cam + 0x84) = *(u32 *)(cam + 0x80);
      *(u32 *)(cam + 0x88) = rnd(); W(0x7d2270) = rnd() % 3; W(0x7d2280) = rnd(); W(0x7d224c) = rnd() % 2;
      nofail = 1; r = ((G3)f[BUPD])(rnd(), (u32)(uintptr_t)cam, rnd()); nofail = 0; ncalls[BUPD]++;
      __asm__ volatile("fnstcw %0" : "=m"(cw)); lg(cw & 0xffff); cw = 0x27f; __asm__ volatile("fldcw %0" :: "m"(cw));
      lg(r); lg(aptr((void *)(uintptr_t)W(0x9473c0))); for (k = 0; k < 16; k++) lg(W(0x947380 + 4 * k)); for (k = 0; k < 16; k++) lg(W(0x62e778 + 4 * k));
      lg(W(0x7d2280)); lg(W(0x7d224c)); lg(W(0x7d2270)); lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x60))); lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x64))); lg(W(0x9475e4)); lg(W(0x9475e8));
      lg(oid((void *)(uintptr_t)W(0x7d26a8))); lg(oid((void *)(uintptr_t)W(0x7d26a4))); lg(oid((void *)(uintptr_t)W(0x7d2248))); lg(oid((void *)(uintptr_t)W(0x7d2244)));
      lg(W(0x9475f0)); lg(W(0x9475f4)); lg(W(0x947748));
      break; }
    case 10: {     /* rwD3D9CameraClear */
      u8 *cam = setup_cam(), *col = amalloc(4); u32 flags = rnd() % 8;
      for (k = 0; k < 4; k++) col[k] = (u8)rnd();
      nofail = 1; r = ((G3)f[CLR])((u32)(uintptr_t)cam, (u32)(uintptr_t)col, flags); nofail = 0; ncalls[CLR]++; lg(r);
      lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x60))); lg(aptr((void *)(uintptr_t)*(u32 *)(cam + 0x64))); lg(W(0x9475e4)); lg(W(0x9475e8)); lg(W(0x947740)); lg(W(0x947744));
      lg(oid((void *)(uintptr_t)W(0x7d26a8))); lg(oid((void *)(uintptr_t)W(0x7d26a4))); lg(oid((void *)(uintptr_t)W(0x7d2248))); lg(oid((void *)(uintptr_t)W(0x7d2244)));
      break; }
    case 9: {      /* D3D9SetPresentParameters */
      static const u32 rate[] = {0, 24, 60, 75, 144}, ms[] = {0, 1, 2, 4, 8}, nm[] = {0, 1, 2, 3, 9}; u32 *md = amalloc(16);
      md[0] = 640 + rnd() % 3; md[1] = 480; md[2] = rate[rnd() % 5]; md[3] = rnd() % 4 == 0 ? 0x16 : 19 + rnd() % 22;
      for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd();
      W(0x62e75c) = rate[rnd() % 5]; W(0x7d2234) = 0x1000 + rnd() % 9; W(0x7d2238) = (u32)(uintptr_t)d3dobj; W(0x7d223c) = rnd() % 3; W(0x62e748) = rnd() % 4;
      W(0x62e750) = ms[rnd() % 5]; W(0x62e758) = nm[rnd() % 5]; W(0x7d226c) = rnd() % 4 == 0; W(0x9475fc) = rnd(); W(0x7d2254) = rnd();
      ((G3)f[SPP])((u32)(uintptr_t)md, rnd(), rnd() % 4 == 0 ? 0x16 : 19 + rnd() % 22); ncalls[SPP]++;
      for (k = 0; k < 14; k++) lg(W(0x947740 + 4 * k)); lg(W(0x947774)); lg(W(0x9475fc)); lg(W(0x7d2254)); break; }
    case 8: {      /* rwD3D9RasterShowRaster */
      static const u32 tg[] = {0x80000000u, 1, 0, 5}; u8 *lc = amalloc(0x6c * 4), *ras = amalloc(0x10), *par = amalloc(0x80);
      for (k = 0; k < 0x6c; k++) lc[k] = (u8)rnd();
      *(u32 *)ras = (u32)(uintptr_t)par; *(u32 *)(par + EXTOFF + 0x1c) = rnd() % 2 ? (u32)(uintptr_t)mkobj(v_sc) : 0;
      for (k = 0; k < 14; k++) W(0x947740 + 4 * k) = rnd();
      W(0x7d2270) = rnd() % 2; W(0x947774) = tg[rnd() % 4]; W(0x7d269c) = rnd() % 6 == 0 ? (u32)-1 : rnd() % 4; W(0x7d26a0) = (u32)(uintptr_t)lc;
      W(0x7d26cc) = rnd() % 2 ? (u32)(uintptr_t)h_restorecb : 0;
      r = ((G3)f[SHOWR])((u32)(uintptr_t)ras, rnd(), rnd()); ncalls[SHOWR]++; lg(r); lg(W(0x7d2270)); lg(W(0x947774)); lg(oid((void *)(uintptr_t)W(0x7d2248))); lg(oid((void *)(uintptr_t)W(0x7d2244)));
      break; }
    case 7: {      /* D3D9CreateDisplayModesList */
      u32 maxn = 12 + rnd() % 4, n;
      W(0x9475e0) = maxn; W(0x7d223c) = rnd() % 3; W(0x7d2238) = (u32)(uintptr_t)d3dobj; W(0x7d2258) = rnd() % 20;
      W(0x7d2260) = rnd() % 2 ? (u32)(uintptr_t)amalloc(20 * (1 + rnd() % 20)) : 0;
      ((G0)f[DML])(); ncalls[DML]++; n = W(0x7d2258); lg(n); lg(aptr((void *)(uintptr_t)W(0x7d2260)));
      { u32 *dm = (u32 *)(uintptr_t)W(0x7d2260); for (k = 0; k < n; k++) { lg(dm[5 * k]); lg(dm[5 * k + 1]); lg(dm[5 * k + 2]); lg(dm[5 * k + 3]); lg(k ? dm[5 * k + 4] : dm[4]); } }
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
  void (__cdecl *bind)(void *, void *); static void *tbl[128];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  init_vtbls();
  bind = (void *)GetProcAddress(m, "mflrw_d3d9dev2_bind");
  if (!bind) { printf("mflrw_d3d9dev2_bind MISSING export\n"); return 1; }
  jmp_patch(0x4d7950, (void *)h_rastercreate); jmp_patch(0x4d77e0, (void *)h_rasterdestroy); jmp_patch(0x4e92b0, (void *)h_fldestroy);  jmp_patch(0x4ecee0, (void *)h_vbmclose); jmp_patch(0x4eb670, (void *)h_im2dclose); jmp_patch(0x4e1be0, (void *)h_rastlistrestore);
  jmp_patch(0x4eca90, (void *)h_dynvbrestore); jmp_patch(0x4e4960, (void *)h_rsreset); jmp_patch(0x4eb500, (void *)h_im2dopen); jmp_patch(0x4f0a40, (void *)h_im3dopen);
  { static const u32 t[] = {0x7d2240, 0x7d2274, 0x7d2278, 0x7d227c,
                            0x5dd244, 0x947760, 0x7d2234, 0x947740, 0x947144, 0x947160, 0x4d7950, 0x4d77e0,
                            0x7d2250, 0x7d2260, 0x7d2258, 0x5dc42b, 0x7d26bc, 0x7d2288, 0x7d2698, 0x4e92b0, 0x7d269c, 0x7d26a0, 0x4dfbf0, 0x4ecee0, 0x4eb670,
                            0x4d9c60, 0x7d2238, 0x7d223c, 0x9475f0, 0x9475f4, 0x7d2248, 0x7d2244, 0x4e1be0, 0x4eca90, 0x4e4960, 0x4eb500, 0x4f0a40, 0x7d26cc,
                            0x7d2270,
                            0x9475e0, 0x5ea080, 0, 0x62e760, 0x7d21f0, 0x7d2284, 0x7d2688, 0x5ea090, 0x7d26a8, 0x7d26a4, 0x7d26c8, 0x4f09d0, 0x4eca00, 0x4e1b20,
                            0x4e96a0, 0x4e90b0, 0x4e9510, 0x947774, 0x4ec9c0,
                            0x62e748, 0x62e750, 0x62e758, 0x7d226c, 0x9475fc, 0x7d2254, 0x62e75c,
                            0x5dd2b4, 0x5dd238, 0x5dd23c, 0x5dd294, 0x5dd268, 0x5dd240, 0x5dd298, 0x4da0f0, 0x9475e4, 0x9475e8, 0x7d225c, 0x9476f4, 0x5ea0d0,
                            0x5dc432, 0x4d0ff0, 0x4d4490, 0x4db470, 0x4e3c00, 0x947380, 0x62e778, 0x9473c0, 0x7d2280, 0x7d224c,
                            0x7d2808, 0x7d280c, 0x7d2800, 0x7d27f4, 0x7d27f8, 0x7d27fc, 0x7d2804, 0x4e0410, 0x4e0dd0, 0x7d27f0, 0x4ea160, 0x53eac0,
                            0x4e0ab0, 0x4e0560, 0x62e880,
                            0x4dd550, 0x4dd610, 0x4ee2d0, 0x4e36a0, 0x4e3760, 0x4e2e40, 0x4e20f0, 0x4ef240, 0x4dd5b0, 0x947768};
    int k; for (k = 0; k < 113; k++) tbl[k] = (void *)(uintptr_t)t[k]; tbl[41] = (void *)stdfns; }
  jmp_patch(0x4f09d0, (void *)h_im3dclose); jmp_patch(0x4eca00, (void *)h_dynvbrel); jmp_patch(0x4e1b20, (void *)h_rastlistrel); jmp_patch(0x4e96a0, (void *)h_flpurgeall);
  jmp_patch(0x4e90b0, (void *)h_flcreate); jmp_patch(0x4e9510, (void *)h_flpurge); jmp_patch(0x4ec9c0, (void *)h_dynvbdiscard);
  jmp_patch(0x4dd550, (void *)h_imgcreate); jmp_patch(0x4dd610, (void *)h_imgalloc); jmp_patch(0x4ee2d0, (void *)h_imgfromraster); jmp_patch(0x4e36a0, (void *)h_chktex);
  jmp_patch(0x4e3760, (void *)h_chkcube); jmp_patch(0x4e2e40, (void *)h_rdestroy2); jmp_patch(0x4e20f0, (void *)h_rcreate2); jmp_patch(0x4ef240, (void *)h_setfromimage);
  jmp_patch(0x4dd5b0, (void *)h_imgdestroy);
  jmp_patch(0x4e0ab0, (void *)h_copyquad); jmp_patch(0x4e0560, (void *)h_renderquad); jmp_patch(0x4e0dd0, (void *)h_cleargeneric); jmp_patch(0x4ea160, (void *)h_errorset); jmp_patch(0x53eac0, (void *)h_errcode);
  jmp_patch(0x4d0ff0, (void *)h_framegetltm); jmp_patch(0x4d4490, (void *)h_matinvert); jmp_patch(0x4db470, (void *)h_settransform); jmp_patch(0x4e3c00, (void *)h_setrenderstate);  bind((void *)ENGVAR, tbl);
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
