/* rw_matfx_check.c: bit-exact check of the librw DLL's matfx exports (group effectpipesd3d9, librw-mfl capi/rwmatfx.cpp) against the ORIGINAL code of MFL.exe, both run
   natively (Wine).  usage: rw_matfx_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [va:byte ...]   (build / run: tools/delink/rw_matfx_check.sh)
   Step 0 of the group (lane K, window 5): the harness (engine allocator and string stand-ins, user data sections, materials / meshes, the default instance callbacks and the
   geometry flag functions as logging stand-ins) plus the device free rows.  The exe's D3D9 device global (0x7d2240) holds the fake device of tools/delink/fakedev.c: every
   method that has no recorder is a trap (prints "TRAP unexpected method n" and exits, never a null call), so rows that touch the device only work once the slots they use
   have a recorder there.  Rows: FUN_00554140 / FUN_00554120 / FUN_00554320 (user data array add), rpMatFXSetupBumpMapTexture, D3D9AtomicMatFXInstanceCallback,
   D3D9WorldSectorMatFXInstanceCallback.  Extra arguments va:byte patch the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8;
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
static u32 shash(const char *s) { u32 h = 2166136261u; if (!s) return 0; while (*s) h = (h ^ (u8)*s++) * 16777619u; return h; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }

#define ARENA (1u << 20)
static u8 *arena; static u32 ap, failmode;
static u32 aptr(const void *p) { return (const u8 *)p >= arena && (const u8 *)p < arena + ARENA ? (u32)(uintptr_t)p : 0x57ac; }
static void *amalloc(u32 n) { u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); } p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p; }
static void *__cdecl s_malloc(u32 n, u32 hint) { lg(0xa108); lg(n); lg(hint); if (n > 8192 || (failmode && hrnd() % failmode == 0)) { lg(0); return 0; } { void *p = amalloc(n); lg((u32)(uintptr_t)p); return p; } }
static void __cdecl s_free(void *p) { lg(0xa10c); lg(aptr(p)); }
static u32 __cdecl s_strlen(const char *s) { lg(0xa1f4); lg(aptr(s)); return (u32)strlen(s); }
static char *__cdecl s_strcpy(char *d, const char *s) { lg(0xa1cc); lg(aptr(d)); lg(shash(s)); return strcpy(d, s); }
/* exe callees of the rows */
static u32 __cdecl h_geomget(void *g) { u32 v = hrnd() & 0xffff; lg(0xa550); lg(aptr(g)); lg(v); return v; }
static void __cdecl h_geomset(void *g, u32 fl) { lg(0xa530); lg(aptr(g)); lg(fl); }
static u32 __cdecl h_sectorget(void *g) { u32 v = hrnd() & 0xffff; lg(0xa580); lg(aptr(g)); lg(v); return v; }
static void __cdecl h_sectorset(void *g, u32 fl) { lg(0xa560); lg(aptr(g)); lg(fl); }
static int __cdecl h_iscompressed(void *r) { int v = (u32)(uintptr_t)r % 3 == 0; lg(0xa5d9); lg((u32)(uintptr_t)r); lg(v); return v; }
static void *__cdecl h_maskcreate(void *a, void *b) { void *r = hrnd() % 4 == 0 ? 0 : (void *)(uintptr_t)(0xabcd0000u + (hrnd() & 0xff)); lg(0xa54e); lg((u32)(uintptr_t)a); lg((u32)(uintptr_t)b); lg((u32)(uintptr_t)r); return r; }
static void *__cdecl h_defatomic(void *a, void *h, void *f) { void *r = (void *)(uintptr_t)(0xdef0a000u + (hrnd() & 0xff)); lg(0xdea1); lg(aptr(a)); lg(aptr(h)); lg((u32)(uintptr_t)f); lg((u32)(uintptr_t)r); return r; }
static void *__cdecl h_defsector(void *a, void *h, void *f) { void *r = (void *)(uintptr_t)(0xdef05000u + (hrnd() & 0xff)); lg(0xdea2); lg(aptr(a)); lg(aptr(h)); lg((u32)(uintptr_t)f); lg((u32)(uintptr_t)r); return r; }

/* render callback callees (logging stand-ins; pointers into the arena are logged as they are) */
static void __cdecl h_clip(void *a, u32 b) { lg(0xa4fa); lg(aptr(a)); lg(b); }
static void __cdecl h_getrs(u32 st, u32 *out) { *out = hrnd() % 3 == 0; lg(0xa3c5); lg(st); lg(*out); }
static void __cdecl h_55d0(u32 a, u32 b) { lg(0xa55d); lg(a); lg(b); }
static void __cdecl h_setrs(u32 a, u32 b) { lg(0xa3c0); lg(a); lg(b); }
static void __cdecl h_settss(u32 a, u32 b, u32 c) { lg(0xa3c7); lg(a); lg(b); lg(c); }
static void __cdecl h_setstreams(void *a, u32 b) { lg(0xa4db); lg(aptr(a)); lg(b); }
static void __cdecl h_valpha(u32 a) { lg(0xa586); lg(a); }
static void __cdecl h_surf(void *a, void *b, u32 c) { lg(0xa3d1); lg(aptr(a)); lg(aptr(b)); lg(c); }
static void __cdecl h_bump(void *a, void *b, u32 c, void *d, void *e, void *f) { lg(0xa522); lg(aptr(a)); lg(aptr(b)); lg(c); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)e); lg((u32)(uintptr_t)f); }
static void __cdecl h_env(void *a, void *b, u32 c, void *d, void *e, void *f) { lg(0xa518); lg(aptr(a)); lg(aptr(b)); lg(c); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)e); lg((u32)(uintptr_t)f); }
static void __cdecl h_dual(void *a, void *b, u32 c, void *d, void *e) { lg(0xa50b); lg(aptr(a)); lg(aptr(b)); lg(c); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)e); }
static void __cdecl h_def(void *a, void *b, u32 c, void *d) { lg(0xa508); lg(aptr(a)); lg(aptr(b)); lg(c); lg((u32)(uintptr_t)d); }
static void __cdecl h_settransform(u32 st, u32 *m) { int k; lg(0xa470); lg(st); lg(aptr(m)); for (k = 0; k < 16; k++) lg(m[k]); }
static void __cdecl h_flush(void) { lg(0xa3b3); }
/* pipeline creation stand-ins */
static void *name_atomic[2] = {(void *)"MatFXAtomicNode", 0}, *name_sector[2] = {(void *)"MatFXSectorNode", 0};
static void *__cdecl h_pipecreate(void) { void *p = hrnd() % 6 == 0 ? 0 : amalloc(0x40); lg(0xa4de); lg((u32)(uintptr_t)p); if (p) memset(p, 0, 0x40); return p; }
static void *__cdecl h_pipelock(void *p) { void *l = hrnd() % 5 == 0 ? 0 : amalloc(0x20); lg(0xa4e6); lg((u32)(uintptr_t)p); lg((u32)(uintptr_t)l); lg(*(u32 *)((u8 *)p + 0x2c)); return l; }
static void *__cdecl h_fragatomic(void) { lg(0xa503); return name_atomic; }
static void *__cdecl h_fragsector(void) { lg(0xa509); return name_sector; }
static void *__cdecl h_addfrag(void *l, u32 a, void *fr, u32 b) { lg(0xa710); lg((u32)(uintptr_t)l); lg(a); lg(shash(*(const char **)fr)); lg(b); return l; }
static void __cdecl h_pipeunlock(void *p) { lg(0xa4e4); lg((u32)(uintptr_t)p); }
static void __cdecl h_pipedestroy(void *p) { lg(0xa4df1); lg((u32)(uintptr_t)p); }
static void *__cdecl h_findnode(void *pipe, const char *name, u32 a, u32 b) { void *n = amalloc(0x20); lg(0xa707); lg((u32)(uintptr_t)pipe); lg(shash(name)); lg(a); lg(b); return n; }
static void *__cdecl h_getinstcb(void *n) { void *r = (void *)(uintptr_t)(0xcb000000u + (hrnd() & 0xffff)); lg(0xa4fb45); lg((u32)(uintptr_t)n); lg((u32)(uintptr_t)r); return r; }
static void __cdecl h_setinstcb(void *n, void *cb) { lg(0xa4fb44); lg((u32)(uintptr_t)n); lg((u32)(uintptr_t)cb); }
static void __cdecl h_setrendercb(void *n, void *cb) { lg(0xa4fb46); lg((u32)(uintptr_t)n); lg((u32)(uintptr_t)cb); }
static u8 *__cdecl h_getcaps(void) { u8 *c = amalloc(0x100); memset(c, 0, 0x100); *(u32 *)(c + 0x94) = hrnd() % 5; *(u32 *)(c + 0x98) = hrnd() % 4; *(u32 *)(c + 0x90) = hrnd() & 0x1000010u; *(u32 *)(c + 0xcc) = (hrnd() % 2 ? 0x100u : 0x101u) + (hrnd() % 3 ? 0 : 0xffff0000u); lg(0xa4dbca); lg((u32)(uintptr_t)c); return c; }
static void __cdecl h_createps(void *data, u32 *slot) { *slot = 0x5ade0000u + (hrnd() & 0xff); lg(0xa4dbc7); lg((u32)(uintptr_t)data); lg(*slot); }
/* bump map render: dynamic vertex buffer, extra device recorders (89 SetFVF, 109 SetPixelShaderConstantF), engine render state get / set (+0x24 / +0x20) */
static int __cdecl h_hasalpha(void *t) { int v = (u32)(uintptr_t)t % 3 != 0; lg(0xa4df9); lg((u32)(uintptr_t)t); lg(v); return v; }
static int __cdecl h_dynlock(u32 stride, u32 n, void **vb, void **data, u32 *base) {
  int ok = hrnd() % 8 != 0; lg(0xa4ecc); lg(stride); lg(n); if (ok) { *vb = amalloc(16); *data = amalloc(0x40); *base = hrnd() % 100; lg((u32)(uintptr_t)*vb); lg((u32)(uintptr_t)*data); lg(*base); } lg(ok); return ok;
}
static void __cdecl h_dynunlock(void *vb) { lg(0xa4ece); lg((u32)(uintptr_t)vb); }
static void __cdecl h_calcuv(u8 *h, u8 *m, void *d) { lg(0xa5285); lg(aptr(h)); lg(aptr(m)); lg((u32)(uintptr_t)d); }
static void __cdecl h_setstreamsource(u32 s, void *vb, u32 off, u32 stride) { lg(0xa4db2); lg(s); lg((u32)(uintptr_t)vb); lg(off); lg(stride); }
static int __cdecl h_4e5950(void) { int v = hrnd() % 2; lg(0xa4e59); lg(v); return v; }
static void __cdecl h_srcblend(u32 v) { lg(0xa4e53a); lg(v); }
static void __cdecl h_destblend(u32 v) { lg(0xa4e53f); lg(v); }
static void __cdecl h_convnonpal(void *r) { lg(0xa4e38b); lg((u32)(uintptr_t)r); }
static long __stdcall fd_validate(void *self, u32 *passes) { long hr = hrnd() % 4 == 0 ? (long)0x8876086c : 0; *passes = hrnd() % 3; lg(0xde500000u | 70); lg(*passes); lg((u32)hr); return hr; }
static long __stdcall fd_setfvf(void *self, u32 fvf) { lg(0xde500000u | 89); lg(fvf); return 0; }
static long __stdcall fd_psconst(void *self, u32 reg, const u32 *c, u32 n) { u32 k; lg(0xde500000u | 109); lg(reg); lg(n); for (k = 0; k < 4 * n; k++) lg(c[k]); return 0; }
static void __cdecl e_rsget(u32 id, u32 *p) { *p = hrnd() & 7; lg(0xa0024); lg(id); lg(*p); }
static void __cdecl e_rsset(u32 id, u32 v) { lg(0xa0020); lg(id); lg(v); }
static float *mkmat(void) { float *m = amalloc(0x40); int k; for (k = 0; k < 16; k++) m[k] = (float)((int)(hrnd() % 2001) - 1000) / 250.0f; return m; }
static int __cdecl h_cube(void *r) { int v = (u32)(uintptr_t)r % 2 == 0; lg(0xa4df); lg((u32)(uintptr_t)r); lg(v); return v; }
static void *__cdecl h_getltm(void *fr) { lg(0xa0ff); lg(aptr(fr)); return fr; }
static void __cdecl h_invert(float *d, float *s) { int k; lg(0xa449); lg(aptr(d)); lg(aptr(s)); for (k = 0; k < 16; k++) { lg(((u32 *)s)[k]); d[k] = s[(k * 5) & 15] * -0.5f + 1.0f; } }
static void __cdecl h_mul(float *d, float *a, float *b) { int k; lg(0xa3cd); lg(aptr(d)); lg(aptr(a)); lg(aptr(b)); for (k = 0; k < 16; k++) { lg(((u32 *)a)[k]); lg(((u32 *)b)[k]); d[k] = a[k] * 0.75f + b[(k + 1) & 15]; } }
static void __cdecl h_black(void *a, void *b) { lg(0xa50a); lg(aptr(a)); lg(aptr(b)); }
static const char *NAMES[] = {"", "a", "uv", "weights", "fx_data_array_long_name", "NAME"};
#define NNAMES 6
#define MATOFF 0x30
/* user data section {count, entries}: entries of 16 bytes {name, format, numElements, data}; data == 0 marks a free entry */
static int32_t *mksect(void) {
  int32_t *s = amalloc(8), n = rnd() % 5, k; s[0] = n; s[1] = 0;
  if (n || rnd() % 2) { u32 *e = amalloc(16 * (n ? n : 1)); s[1] = (int32_t)(uintptr_t)e; for (k = 0; k < n; k++) { e[4 * k] = rnd() % 2 ? (u32)(uintptr_t)amalloc(8) : 0; e[4 * k + 1] = rnd() % 4; e[4 * k + 2] = rnd() % 5; e[4 * k + 3] = rnd() % 3 ? (u32)(uintptr_t)amalloc(8) : 0; } }
  return s;
}
static u8 *mkheader(int meshes) {
  u8 *h = amalloc(0x48 + 0x24 * (meshes ? meshes : 1) + 8); int k; memset(h, 0, 0x48 + 0x24 * (meshes ? meshes : 1));
  *(u32 *)(h + 4) = meshes;
  for (k = 0; k < meshes; k++) {
    u8 *mat = amalloc(0x60); u8 *data = amalloc(0x40); memset(mat, 0, 0x60); memset(data, 0, 0x40);
    *(u32 *)(data + 0x30) = rnd() % 5; *(u32 *)(mat + MATOFF) = rnd() % 5 ? (u32)(uintptr_t)data : 0;
    *(u32 *)(h + 0x48 + 0x24 * k) = (u32)(uintptr_t)mat;
  }
  return h;
}

enum { UDA, UDG, UDS, BUMP, INA, INS, RCB, DEF, APP, PLC, BMR, ENV, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_UserDataArrayAdd", 0x554140}, {"rw37_UserDataGeometryArrayAdd", 0x554120}, {"rw37_UserDataWorldSectorArrayAdd", 0x554320},
  {"rw37_rpMatFXSetupBumpMapTexture", 0x5540c0}, {"rw37_D3D9AtomicMatFXInstanceCallback", 0x553ee0}, {"rw37_D3D9WorldSectorMatFXInstanceCallback", 0x553f60},
  {"rw37_rwD3D9AtomicMatFXRenderCallback", 0x553a90}, {"rw37_rpMatFXD3D9AtomicMatFXDefaultRender", 0x5508d0},
  {"rw37_ApplyEnvMapTextureMatrix", 0x5520e0}, {"rw37_rpMatFXPipelinesCreate", 0x553ca0},
  {"rw37_rpMatFXD3D9AtomicMatFXBumpMapRender", 0x5522d0}, {"rw37_rpMatFXD3D9AtomicMatFXEnvRender", 0x551860}};
typedef u32 (__cdecl *G2)(u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32); typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF];

static void reset(u32 seed) {
  nlg = 0; ap = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  failmode = rnd() % 3 == 0 ? 2 + rnd() % 5 : 0;
  memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; EW(0x108) = (u32)(uintptr_t)s_malloc; EW(0x10c) = (u32)(uintptr_t)s_free; EW(0xf4) = (u32)(uintptr_t)s_strlen; EW(0xcc) = (u32)(uintptr_t)s_strcpy;
  W(0x7da774) = 0x10; W(0x7da77c) = 0x20; W(0x7da71c) = MATOFF; W(0x7da768) = rnd() % 4 != 0; W(0x7da76c) = (u32)(uintptr_t)h_defatomic; W(0x7da770) = (u32)(uintptr_t)h_defsector;
  W(0x62e76c) = 0; W(0x62e768) = 0; W(0x62e770) = 0; W(0x62e764) = 0; W(0x62e760) = 0;      /* device state caches of the exe */
  memset((void *)(uintptr_t)0x63a2d0, 0, 0x40);
  { static const u32 gl[] = {0x7da734, 0x7da738, 0x7da764, 0x7da754, 0x7da758, 0x7da75c, 0x7da760, 0x63a2c8, 0x63a2cc, 0x7da73c, 0x7da740, 0x7da744, 0x7da748, 0x7da74c, 0x7da750}; int k; for (k = 0; k < 15; k++) W(gl[k]) = 0; }
  fakedev_log = sink; fakedev_hr = 0; W(DEVVAR) = (u32)(uintptr_t)fakedev_init();
  fakedev_vtbl[70] = (void *)fd_validate; fakedev_vtbl[89] = (void *)fd_setfvf; fakedev_vtbl[109] = (void *)fd_psconst; EW(0x24) = (u32)(uintptr_t)e_rsget; EW(0x20) = (u32)(uintptr_t)e_rsset;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 17, r = 0;
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: { int32_t *sc = mksect(); r = ((G4)f[UDA])((u32)(uintptr_t)sc, rnd() % 3 ? (u32)(uintptr_t)NAMES[rnd() % NNAMES] : 0, rnd() % 6, rnd() % 7); ncalls[UDA]++; lg(r); lg(sc[0]); lg(aptr((void *)(uintptr_t)sc[1])); break; }
    case 1: case 2: { u8 *o = amalloc(0x80); int32_t *sc; u32 off = op == 1 ? 0x10 : 0x20; memset(o, 0, 0x80); sc = mksect(); memcpy(o + off, sc, 8);
      r = ((G4)f[op == 1 ? UDG : UDS])((u32)(uintptr_t)o, rnd() % 3 ? (u32)(uintptr_t)NAMES[rnd() % NNAMES] : 0, rnd() % 6, rnd() % 7); ncalls[op == 1 ? UDG : UDS]++; lg(r);
      lg(*(u32 *)(o + off)); lg(aptr((void *)(uintptr_t)*(u32 *)(o + off + 4))); break; }
    case 3: { u32 *tex = amalloc(16), *bump = amalloc(16); tex[0] = 0x1000 + rnd() % 64; bump[0] = 0x2000 + rnd() % 64;
      r = ((G2)f[BUMP])(rnd() % 4 ? (u32)(uintptr_t)tex : 0, rnd() % 5 ? (u32)(uintptr_t)bump : 0); ncalls[BUMP]++; lg(r); break; }
    case 15: case 16: { u8 *hdr = amalloc(0x40), *mesh = amalloc(0x60), *mat = amalloc(0x60), *md = amalloc(0x80), *tr = amalloc(0x40), *er = amalloc(0x40), *ras;
      u32 *tex = amalloc(16), *env = amalloc(16), k, idx = rnd() % 3 ? 0 : 1, sv[6]; float *cam = amalloc(0x10); float *cf = mkmat(); static const float cs[] = {0.0f, 0.5f, 1.0f, 0.999f, 0.004f, 0.25f};
      memset(hdr, 0, 0x40); memset(mesh, 0, 0x60); memset(mat, 0, 0x60); memset(md, 0, 0x80); memset(tr, 0, 0x40); memset(er, 0, 0x40);
      *(u32 *)(hdr + 8) = rnd() % 2 ? 0x1b0000u + rnd() % 3 : 0; *(u32 *)(hdr + 0xc) = 4 + rnd() % 3;
      *(u32 *)(mesh + 8) = (u32)(uintptr_t)mat; *(u32 *)(mat + MATOFF) = (u32)(uintptr_t)md; mat[7] = rnd() % 2 ? 0xff : 0x80; *(u32 *)(mesh + 0xc) = rnd() % 4 == 0; *(u32 *)(mesh + 0x10) = 0x5000 + rnd() % 3;
      for (k = 0x14; k < 0x24; k += 4) *(u32 *)(mesh + k) = rnd() % 100;
      for (k = 0; k < 2; k++) { u8 *e = md + 24 * k; *(float **)e = rnd() % 3 ? mkmat() : 0; { float c = cs[rnd() % 6]; memcpy(e + 8, &c, 4); } *(u32 *)(e + 0xc) = rnd() % 3 == 0; *(u32 *)(e + 4) = 0xe0 + k; }
      tr[0x23] = rnd() % 2 ? 0x20 : 0; er[0x23] = rnd() % 2 ? 0x20 : 0; tex[0] = (u32)(uintptr_t)tr; env[0] = rnd() % 8 ? (u32)(uintptr_t)er : 0; ras = tr; (void)ras;
      *(u32 *)cam = 0; ((u32 *)cam)[1] = (u32)(uintptr_t)cf; EW(0) = (u32)(uintptr_t)cam;
      for (k = 0; k < 6; k++) { sv[k] = W(0x7da73c + 4 * k); W(0x7da73c + 4 * k) = rnd() % 3 ? 0x5ade0000u + k : 0; }
      { u32 s7a = W(0x7da758), s7b = W(0x7da760), s7c = W(0x63a2cc);
        W(0x7da758) = rnd() % 3 == 0; W(0x7da760) = rnd() % 3 == 0; W(0x63a2cc) = rnd() % 2;
        W(0x62e76c) = rnd() % 3 ? 0x5ade0000u + rnd() % 6 : 0; W(0x62e768) = 0x5000 + rnd() % 3; fakedev_hr = 0;
        ((void (__cdecl *)(u32, u32, u32, u32, u32, u32))f[ENV])((u32)(uintptr_t)hdr, (u32)(uintptr_t)mesh, rnd() % 2 ? 0x84 : rnd() & 0xff, idx, rnd() % 5 ? (u32)(uintptr_t)tex : 0, rnd() % 6 ? (u32)(uintptr_t)env : 0);
        ncalls[ENV]++; lg(W(0x62e76c)); lg(W(0x62e768)); lg(W(0x7da760)); lg(W(0x63a2cc)); lg(tr[0x23 / 4]); lg(er[0x23 / 4]);
        W(0x7da758) = s7a; W(0x7da760) = s7b; W(0x63a2cc) = s7c; }
      for (k = 0; k < 6; k++) W(0x7da73c + 4 * k) = sv[k];
      break; }
    case 13: case 14: { u8 *hdr = amalloc(0x40), *mesh = amalloc(0x60), *mat = amalloc(0x60), *md = amalloc(0x60), *tx = amalloc(0x40), *bt = amalloc(0x40), *ev = amalloc(0x40); u32 k, sv[6], keepA = W(0x7da76c);
      float *cam = amalloc(0x10); float *cf = mkmat(); float *fr = rnd() % 3 ? mkmat() : 0;
      memset(hdr, 0, 0x40); memset(mesh, 0, 0x60); memset(mat, 0, 0x60); memset(md, 0, 0x60);
      *(u32 *)(hdr + 8) = rnd() % 2 ? 0x1b0000u + rnd() % 3 : 0; *(u32 *)(hdr + 0xc) = 4 + rnd() % 3; *(u32 *)(hdr + 0x30) = 0x57000000u + rnd() % 4; *(u32 *)(hdr + 0x34) = 0xdec10000u + rnd() % 3;
      *(u32 *)(mesh + 8) = (u32)(uintptr_t)mat; *(u32 *)(mat + MATOFF) = (u32)(uintptr_t)md; *(u32 *)(mesh + 0x10) = 0x5000 + rnd() % 3; for (k = 0x14; k < 0x24; k += 4) *(u32 *)(mesh + k) = rnd() % 100;
      *(float **)(md + 0x18) = fr; { float c = rnd() % 4 ? (float)(rnd() % 100) / 100.0f : 0.0f; if (rnd() % 8 == 0) c = 0.001f; memcpy(md + 0x20, &c, 4); } *(u32 *)(md + 0x24) = rnd() % 2;
      tx[0] = 0x11; *(u32 *)tx = 0x4000 + rnd() % 64; *(u32 *)ev = 0x4100 + rnd() % 64; *(u32 *)bt = 0x4200 + rnd() % 64;
      *(u32 *)cam = 0; ((u32 *)cam)[1] = (u32)(uintptr_t)cf; EW(0) = (u32)(uintptr_t)cam;
      for (k = 0; k < 6; k++) { sv[k] = W(0x7da73c + 4 * k); W(0x7da73c + 4 * k) = rnd() % 3 ? 0x5ade0000u + k : 0; }
      W(0x62e76c) = rnd() % 3 ? 0x5ade0000u + rnd() % 6 : 0; W(0x62e768) = 0x5000 + rnd() % 3; W(0x62e760) = rnd() % 3 ? 0x252 : rnd(); W(0x62e764) = 0xdec10000u + rnd() % 3; fakedev_hr = 0;
      ((void (__cdecl *)(u32, u32, u32, u32, u32, u32))f[BMR])((u32)(uintptr_t)hdr, (u32)(uintptr_t)mesh, rnd(), rnd() % 4 ? (u32)(uintptr_t)tx : 0, rnd() % 3 ? (u32)(uintptr_t)bt : 0, rnd() % 3 ? (u32)(uintptr_t)ev : 0);
      ncalls[BMR]++; lg(W(0x62e76c)); lg(W(0x62e768)); lg(W(0x62e760)); lg(W(0x62e764));
      for (k = 0; k < 6; k++) W(0x7da73c + 4 * k) = sv[k];
      W(0x7da76c) = keepA; break; }
    case 12: { u32 sv[6], k, keepA = W(0x7da76c), keepS = W(0x7da770), keepT = W(0x7da768); for (k = 0; k < 6; k++) sv[k] = W(0x7da73c + 4 * k);
      r = ((u32 (__cdecl *)(void))f[PLC])(); ncalls[PLC]++; lg(r);
      lg(W(0x7da734)); lg(W(0x7da738)); lg(W(0x7da76c)); lg(W(0x7da770)); lg(W(0x7da764)); lg(W(0x7da754)); lg(W(0x7da758)); lg(W(0x7da75c)); lg(W(0x7da760)); lg(W(0x63a2c8)); lg(W(0x63a2cc)); lg(W(0x7da768));
      for (k = 0; k < 6; k++) { lg(W(0x7da73c + 4 * k)); W(0x7da73c + 4 * k) = sv[k]; }
      W(0x7da76c) = keepA; W(0x7da770) = keepS; W(0x7da768) = keepT; break; }
    case 10: case 11: { u32 *tex = amalloc(16), stage = rnd() % 4, k; float *cam = amalloc(0x10), *cf = mkmat(), *fr = rnd() % 4 ? mkmat() : 0;
      tex[0] = 0x4000 + rnd() % 64; *(u32 *)(cam + 1) = (u32)(uintptr_t)cf; EW(0) = (u32)(uintptr_t)cam; ((u32 *)cam)[0] = 0; ((u32 *)cam)[1] = (u32)(uintptr_t)cf;
      r = ((G3)f[APP])((u32)(uintptr_t)tex, stage, (u32)(uintptr_t)fr); ncalls[APP]++; lg(r); (void)k; break; }
    case 8: case 9: { u8 *hdr = amalloc(0x40), *mesh = amalloc(0x60), *mat = amalloc(0x60), *md = amalloc(0x40), *tx = amalloc(0x60); u32 k, fl = rnd() % 3 == 0 ? 0 : (rnd() % 2 ? 0x84 : rnd() & 0xff);
      memset(hdr, 0, 0x40); memset(mesh, 0, 0x60); memset(mat, 0, 0x60); memset(md, 0, 0x40); memset(tx, 0, 0x60);
      *(u32 *)(hdr + 8) = rnd() % 2 ? 0x1b0000u + rnd() % 3 : 0; *(u32 *)(hdr + 0xc) = 4 + rnd() % 3;
      for (k = 0; k < 0x60; k += 4) *(u32 *)(tx + k) = 0x3f000000u + rnd() % 64; *(u32 *)(tx + 0xc) = rnd() % 2 ? 0x20000u : 0x40u;
      *(u32 *)(md + 0x30) = rnd() % 3 ? 5 : rnd() % 8; *(u32 *)md = rnd() % 4 ? (u32)(uintptr_t)tx : 0; *(u32 *)(mat + MATOFF) = rnd() % 6 ? (u32)(uintptr_t)md : 0; *(u32 *)(mesh + 8) = (u32)(uintptr_t)mat;
      *(u32 *)(mesh + 0x10) = 0x5000 + rnd() % 3; for (k = 0x14; k < 0x24; k += 4) *(u32 *)(mesh + k) = rnd() % 100;
      W(0x62e76c) = rnd() % 2; W(0x62e768) = 0x5000 + rnd() % 3; fakedev_hr = 0;
      ((void (__cdecl *)(u32, u32, u32, u32))f[DEF])((u32)(uintptr_t)hdr, (u32)(uintptr_t)mesh, fl, rnd() % 2 ? (u32)(uintptr_t)tx : 0); ncalls[DEF]++; lg(W(0x62e76c)); lg(W(0x62e768));
      for (k = 0; k < 16; k++) lg(W(0x63a2d0 + 4 * k)); break; }
    case 6: case 7: { int meshes = 1 + rnd() % 3, k; u8 *hdr = amalloc(0x58 + 0x24 * meshes + 8); memset(hdr, 0, 0x58 + 0x24 * meshes);
      *(u32 *)(hdr + 0x1c) = meshes; *(u32 *)(hdr + 0x20) = rnd() % 3 ? 0x1b0000u + rnd() % 3 : 0; *(u32 *)(hdr + 0x28) = 0x57000000u + rnd() % 4; *(u32 *)(hdr + 0x48) = 0x5000 + rnd() % 3;
      *(u32 *)(hdr + 0x4c) = 0xdec10000u + rnd() % 3;
      for (k = 0; k < meshes; k++) {
        u8 *mat = amalloc(0x60), *data = amalloc(0x40), *e = hdr + 0x58 + 0x24 * k; memset(mat, 0, 0x60); memset(data, 0, 0x40);
        *(u32 *)mat = rnd() % 4 ? 0x7e000000u + rnd() % 16 : 0; mat[7] = rnd() % 2 ? 0xff : 0x80; *(u32 *)(data + 0x30) = rnd() % 8; { u8 *tx = amalloc(0x60); memset(tx, 0, 0x60); *(u32 *)(tx + 0xc) = rnd() % 2 ? 0x20000u : 0x40u; *(u32 *)(tx + 0x30) = rnd(); *(u32 *)data = rnd() % 4 ? (u32)(uintptr_t)tx : 0; } *(u32 *)(data + 4) = 0xd1000000u + rnd() % 8;
        *(u32 *)(data + 0x18) = 0xd2000000u + rnd() % 8; *(u32 *)(data + 0x1c) = 0xd3000000u + rnd() % 8;
        *(u32 *)(mat + MATOFF) = rnd() % 6 ? (u32)(uintptr_t)data : 0; *(u32 *)(e + 8) = (u32)(uintptr_t)mat; *(u32 *)(e + 0xc) = rnd() % 4 == 0;
      }
      W(0x62e770) = rnd() % 3 ? 0x1b0000u + rnd() % 3 : 0; W(0x62e764) = 0xdec10000u + rnd() % 3; W(0x62e760) = rnd();
      ((void (__cdecl *)(u32, u32, u32, u32))f[RCB])((u32)(uintptr_t)hdr, (u32)(uintptr_t)amalloc(16), rnd(), rnd() % 16); ncalls[RCB]++; lg(W(0x62e770)); lg(W(0x62e764)); lg(W(0x62e760)); break; }
    case 4: case 5: { u8 *obj = amalloc(0x80); int meshes = 1 + rnd() % 3; u8 *h = mkheader(meshes); memset(obj, 0, 0x80); *(u32 *)(obj + 0x18) = (u32)(uintptr_t)amalloc(16);
      r = ((G3)f[op == 4 ? INA : INS])((u32)(uintptr_t)obj, (u32)(uintptr_t)h, op == 4 && rnd() % 3 == 0 ? 1 : 0); ncalls[op == 4 ? INA : INS]++; lg(r); break; }
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
  void (__cdecl *bind)(void *, void *); static void *tbl[80];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  printf("render callback jump table: %08x %08x %08x %08x %08x %08x\n", W(0x553c88), W(0x553c8c), W(0x553c90), W(0x553c94), W(0x553c98), W(0x553c9c));
  jmp_patch(0x509550, (void *)h_geomget); jmp_patch(0x509530, (void *)h_geomset); jmp_patch(0x509580, (void *)h_sectorget); jmp_patch(0x509560, (void *)h_sectorset);
  jmp_patch(0x5d5d90, (void *)h_iscompressed); jmp_patch(0x54ee50, (void *)h_maskcreate);
  jmp_patch(0x4fae50, (void *)h_clip); jmp_patch(0x4e3c50, (void *)h_getrs); jmp_patch(0x4e55d0, (void *)h_55d0); jmp_patch(0x4e3c00, (void *)h_setrs); jmp_patch(0x4e3c70, (void *)h_settss);
  jmp_patch(0x4db2d0, (void *)h_setstreams); jmp_patch(0x4e5860, (void *)h_valpha); jmp_patch(0x4e3d10, (void *)h_surf); (void)h_bump; /* 0x5522d0, 0x551860 run as original code (batch 3) */(void)h_env; jmp_patch(0x4e38b0, (void *)h_convnonpal);
  jmp_patch(0x550b50, (void *)h_dual); jmp_patch(0x550aa0, (void *)h_black); jmp_patch(0x4dfc90, (void *)h_hasalpha); jmp_patch(0x4eccd0, (void *)h_dynlock); jmp_patch(0x4ece50, (void *)h_dynunlock); jmp_patch(0x552850, (void *)h_calcuv);
  jmp_patch(0x4db270, (void *)h_setstreamsource); jmp_patch(0x4e5950, (void *)h_4e5950); jmp_patch(0x4e53a0, (void *)h_srcblend); jmp_patch(0x4e53f0, (void *)h_destblend);
  jmp_patch(0x4def80, (void *)h_pipecreate); jmp_patch(0x4e6f40, (void *)h_pipelock); jmp_patch(0x5003b0, (void *)h_fragatomic); jmp_patch(0x5009c0, (void *)h_fragsector);
  jmp_patch(0x4e7100, (void *)h_addfrag); jmp_patch(0x4e64f0, (void *)h_pipeunlock); jmp_patch(0x4deff0, (void *)h_pipedestroy); jmp_patch(0x4e7070, (void *)h_findnode);
  jmp_patch(0x4fb450, (void *)h_getinstcb); jmp_patch(0x4fb440, (void *)h_setinstcb); jmp_patch(0x4fb460, (void *)h_setrendercb); jmp_patch(0x4dbca0, (void *)h_getcaps);
  jmp_patch(0x4dbc70, (void *)h_createps);
  jmp_patch(0x4dfcb0, (void *)h_cube); jmp_patch(0x4d0ff0, (void *)h_getltm); jmp_patch(0x4d4490, (void *)h_invert); jmp_patch(0x4d3cd0, (void *)h_mul);
  jmp_patch(0x4db470, (void *)h_settransform); jmp_patch(0x4e3b30, (void *)h_flush); (void)h_def;      /* 0x5508d0 runs as original code now (batch 3) */
  bind = (void *)GetProcAddress(m, "mflrw_matfx_bind");
  if (!bind) { printf("mflrw_matfx_bind MISSING export\n"); return 1; }
  { static const u32 t[] = {0x7da774, 0x7da77c, 0x7da71c, 0x7da768, 0x7da76c, 0x7da770, 0x509550, 0x509530, 0x509580, 0x509560, 0x5d5d90, 0x54ee50,
                              0x4fae50, 0x4e3c50, 0x4e55d0, 0x4e3c00, 0x4e3c70, 0x4db2d0, 0x4e5860, 0x4e3d10, 0x5522d0, 0x551860, 0x550b50, 0x5508d0, 0x550aa0, 0x62e770, 0x62e764, 0x62e760, 0x7d2240,
                              0x4db470, 0x4e3b30, 0x62e76c, 0x62e768, 0x63a2d0,
                              0x4dfcb0, 0x4d0ff0, 0x4d4490, 0x4d3cd0, 0x5f5d48,
                              0x4def80, 0x4e6f40, 0x5003b0, 0x5009c0, 0x4e7100, 0x4e64f0, 0x4deff0, 0x4e7070, 0x4fb450, 0x4fb440, 0x4fb460, 0x4dbca0, 0x4dbc70,
                              0x7da734, 0x7da738, 0x7da764, 0x7da754, 0x7da758, 0x7da75c, 0x7da760, 0x63a2c8, 0x63a2cc, 0x7da73c, 0x553ee0, 0x553f60, 0x553a90,
                              0x5f5a80, 0x5f5ae0, 0x5f5b30, 0x5f5b70, 0x5f5bf8, 0x5f5ca8,
                              0x4dfc90, 0x4eccd0, 0x4ece50, 0x552850, 0x4db270, 0x4e5950, 0x4e53a0, 0x4e53f0, 0x4e38b0};
    int k; for (k = 0; k < 80; k++) tbl[k] = (void *)(uintptr_t)t[k]; }
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
