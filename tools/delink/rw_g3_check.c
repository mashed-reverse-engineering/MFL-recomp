/* rw_g3_check.c: behavioural + byte-layout check of the librw DLL's g3 exports (tools/delink/rw_g3.txt: RW 3.7 raster / texture / texdict layer, capi/g3.cpp)
   against the ORIGINAL code of MFL.exe, both run natively (Wine).  usage: rw_g3_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]
   (build/run: tools/delink/rw_g3_check.sh; image mapping as rw_g2_check.c).  The originals run in place with their internal calls (RwTexDictionaryDestroy ->
   RwTextureDestroy -> RwRasterDestroy -> rwPluginRegistryDeInitObject, RwTextureSetName -> _rwerror / RwErrorSet); the DLL is bound to the same exe globals
   (mflrw_exe_bind), so both sides work on the same engine instance, module globals and plugin registries.  Everything outside is a logging stand-in of the harness:
   the free lists (engine +0x118 / +0x11c: deterministic arena, LIFO reuse, fresh blocks 0xcd, sometimes NULL), the D3D9 driver's standard functions
   (+0x58 create, +0x5c destroy, +0x78 sub raster, +0x84 / +0x88 lock / unlock, +0xa4 / +0xa8 palette, +0xb8 mip levels: success / out-write chosen per step),
   rwstrncpy / rwstrlen (+0xd0 / +0xf4) and the plugin constructors / destructors (two per registry; a constructor fails sometimes).
   Per seed one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the original addresses:
   the call logs (every callback with its arguments, every result, the 0x80 byte block of every object touched after each step) and the final arena, engine
   instance, registries and error state must be identical.  Extra arguments va:byte patch the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32;
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

/* exe globals */
#define ENGVAR 0x7d2128u
#define RASMODVAR 0x7d21d8u
#define TEXMODVAR 0x7d2184u
#define ERRMODVAR 0x7d518cu
#define REG_TEX 0x62e6d0u
#define REG_DICT 0x62e6e8u
#define REG_RAS 0x62e718u
#define RASMOD 0x300
#define TEXMOD 0x400
#define ERRMOD 0x600
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))

/* log */
static u32 *LG, nlg, cap; static long nhit, nlong;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static void lgobj(const void *p) { int i; lg(0x0b1ec700); lg((u32)(uintptr_t)p); if (p) for (i = 0; i < 0x80; i += 4) lg(*(const u32 *)((const char *)p + i)); }

/* arena = the free lists */
#define BLK 0x80
#define NBLK 8192
#define ARENA (NBLK * BLK)
static unsigned char *arena;      /* malloc'd once (a static block would push the harness's bss over 0x4b0000) */
static void *freed[NBLK]; static int nfreed, nbump, alloc_fail;
static void *__cdecl fl_alloc(void *list, u32 hint) {
  void *p;
  lg(0xa1); lg((u32)(uintptr_t)list); lg(hint);
  if (alloc_fail) { lg(0); return 0; }
  if (nfreed) p = freed[--nfreed]; else { if (nbump == NBLK) { printf("arena full\n"); exit(2); } p = arena + BLK * nbump++; memset(p, 0xcd, BLK); }
  lg((u32)(uintptr_t)p); return p;
}
static void __cdecl fl_free(void *list, void *p) { lg(0xf1); lg((u32)(uintptr_t)list); lg((u32)(uintptr_t)p); freed[nfreed++] = p; }

/* driver stand-ins */
static int drv_ok, drv_write;
static u32 __cdecl d_create(void *out, unsigned char *r, int flags) {
  lg(0xd58); lg((u32)(uintptr_t)out); lg((u32)(uintptr_t)r); lg(flags);
  if (!drv_ok) return 0;
  r[0x20] = flags & 7; r[0x21] = flags & 0xf8; r[0x23] = (unsigned char)(flags >> 8); *(u32 *)(r + 0x18) = *(u32 *)(r + 0x0c) * 4;
  memset(r + 0x34, 0x3a, 0x24); return 1;
}
static u32 __cdecl d_destroy(void *out, void *r, int f) { lg(0xd5c); lg((u32)(uintptr_t)out); lg((u32)(uintptr_t)r); lg(f); return 1; }
static u32 __cdecl d_sub(void *sub, void *r, int f) { lg(0xd78); lg((u32)(uintptr_t)sub); lg((u32)(uintptr_t)r); lg(f); return drv_ok; }
static u32 __cdecl d_lock(u32 *out, unsigned char *r, int f) { lg(0xd84); lg((u32)(uintptr_t)out ? *out : 0xdeadu); lg((u32)(uintptr_t)r); lg(f); if (drv_write) { *out = 0x10000000u + f; r[0x22] |= 2; } return drv_ok; }
static u32 __cdecl d_unlock(void *out, unsigned char *r, int f) { lg(0xd88); lg((u32)(uintptr_t)out); lg((u32)(uintptr_t)r); lg(f); r[0x22] &= ~2; return 1; }
static u32 __cdecl d_lockpal(u32 *out, unsigned char *r, int f) { lg(0xda4); lg(*out); lg((u32)(uintptr_t)r); lg(f); if (drv_write) { *out = 0x20000000u + f; r[0x22] |= 0x18; } return drv_ok; }
static u32 __cdecl d_unlockpal(void *out, unsigned char *r, int f) { lg(0xda8); lg((u32)(uintptr_t)out); lg((u32)(uintptr_t)r); lg(f); r[0x22] = (unsigned char)(r[0x22] * 5 + 0x3b); return 1; }      /* all 8 bits vary before the original's mask */
static u32 __cdecl d_levels(u32 *out, void *r, int f) { lg(0xdb8); lg(*out); lg((u32)(uintptr_t)r); lg(f); if (drv_write) *out = 7; return drv_ok; }
static char *__cdecl s_ncpy(char *d, const char *s, u32 n) { lg(0x5d0); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)s); lg(n); return strncpy(d, s, n); }
static u32 __cdecl s_len(const char *s) { lg(0x5f4); lg((u32)(uintptr_t)s); nlong += strlen(s) >= 32; return (u32)strlen(s); }

/* plugin registries: two entries each (RwPluginRegEntry layout: offset 0, size 4, ctor 0x20, dtor 0x24, next 0x30, prev 0x34) */
static u32 entries[6][14]; static int ctor_fail_off = -1;
static void *__cdecl p_ctor(unsigned char *o, int off, int size) { int i; lg(0xc7); lg((u32)(uintptr_t)o); lg(off); lg(size); for (i = 0; i < size; i++) o[off + i] = (unsigned char)(off * 3 + i * 7); return off == ctor_fail_off ? 0 : o; }
static void *__cdecl p_dtor(unsigned char *o, int off, int size) { lg(0xd7); lg((u32)(uintptr_t)o); lg(off); lg(size); memset(o + off, 0xdd, size); return o; }
static void reg_setup(u32 reg, int e, int size, int off1, int sz1, int off2, int sz2) {
  u32 *a = entries[e], *b = entries[e + 1];
  memset(a, 0, 56); memset(b, 0, 56);
  a[0] = off1; a[1] = sz1; a[8] = (u32)(uintptr_t)p_ctor; a[9] = (u32)(uintptr_t)p_dtor; a[12] = (u32)(uintptr_t)b;
  b[0] = off2; b[1] = sz2; b[8] = (u32)(uintptr_t)p_ctor; b[9] = (u32)(uintptr_t)p_dtor; b[13] = (u32)(uintptr_t)a;
  W(reg) = size; W(reg + 4) = size; W(reg + 8) = size; W(reg + 0xc) = 0; W(reg + 0x10) = (u32)(uintptr_t)a; W(reg + 0x14) = (u32)(uintptr_t)b;
}

/* the functions under test: [0] DLL export, [1] original */
enum { RC, RD, RSUB, RLEV, RLOCK, RUNL, RLPAL, RUPAL, RCTX, ROFF, TAUTO, DCRE, DDES, TCRE, TDES, TDES2, TNAME, TMASK, DADD, DFIND, NF };
static const struct { const char *dll; u32 va; } F[NF] = {
  {"rw37_RwRasterCreate", 0x4d7950}, {"rw37_RwRasterDestroy", 0x4d77e0}, {"rw37_RwRasterSubRaster", 0x4d78f0}, {"rw37_RwRasterGetNumLevels", 0x4d7880},
  {"rw37_RwRasterLock", 0x4d79f0}, {"rw37_RwRasterUnlock", 0x4d7790}, {"rw37_RwRasterLockPalette", 0x4d7850}, {"rw37_RwRasterUnlockPalette", 0x4d77b0},
  {"rw37_RwRasterGetCurrentContext", 0x4d7750}, {"rw37_RwRasterGetOffset", 0x4d7770}, {"rw37_RwTextureGetAutoMipmapping", 0x4d59e0},
  {"rw37_RwTexDictionaryCreate", 0x4d5a20}, {"rw37_RwTexDictionaryDestroy", 0x4d5ac0}, {"rw37_RwTextureCreate", 0x4d5b90}, {"rw37_RwTextureDestroy", 0x4d5bf0},
  {"rw37_destroyTexture", 0x4df790}, {"rw37_RwTextureSetName", 0x4d5c70}, {"rw37_RwTextureSetMaskName", 0x4d5ce0}, {"rw37_RwTexDictionaryAddTexture", 0x4d5d50},
  {"rw37_RwTexDictionaryFindNamedTexture", 0x4d5d90}};
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32); typedef u32 (__cdecl *G3)(u32, u32, u32); typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NF];
static long ncalls[NF];

static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static unsigned char *rp[256], *tp[256], *dp[64]; static int nr, nt, nd;
static char names[64][48];
static void pool_del(unsigned char **pool, int *n, int i) { pool[i] = pool[--*n]; }

static void reset(u32 seed) {
  int i;
  nlg = 0; nfreed = 0; nbump = 0; alloc_fail = 0; drv_ok = 1; drv_write = 1; ctor_fail_off = -1; nr = nt = nd = 0; rs = seed * 2654435761u + 1;
  memset(eng, 0, sizeof eng); memset(arena, 0, ARENA);
  W(ENGVAR) = (u32)(uintptr_t)eng; W(RASMODVAR) = RASMOD; W(TEXMODVAR) = TEXMOD; W(ERRMODVAR) = ERRMOD;
  EW(0x58) = (u32)(uintptr_t)d_create; EW(0x5c) = (u32)(uintptr_t)d_destroy; EW(0x78) = (u32)(uintptr_t)d_sub; EW(0x84) = (u32)(uintptr_t)d_lock;
  EW(0x88) = (u32)(uintptr_t)d_unlock; EW(0xa4) = (u32)(uintptr_t)d_lockpal; EW(0xa8) = (u32)(uintptr_t)d_unlockpal; EW(0xb8) = (u32)(uintptr_t)d_levels;
  EW(0xd0) = (u32)(uintptr_t)s_ncpy; EW(0xf4) = (u32)(uintptr_t)s_len; EW(0x118) = (u32)(uintptr_t)fl_alloc; EW(0x11c) = (u32)(uintptr_t)fl_free;
  EW(RASMOD + 0x60) = 0x11110060;      /* raster free list id */
  for (i = 0; i < 10; i++) EW(RASMOD + 4 * i) = 0xc0c00000u + i;      /* raster context stack */
  EW(TEXMOD) = (u32)(uintptr_t)(eng + TEXMOD); EW(TEXMOD + 4) = (u32)(uintptr_t)(eng + TEXMOD);      /* empty texdict list */
  EW(TEXMOD + 8) = 0x22220008; EW(TEXMOD + 0xc) = 0x2222000c;      /* texture / texdict free list ids */
  EW(ERRMOD) = 0; EW(ERRMOD + 4) = 0x80000000u;      /* no error yet */
  reg_setup(REG_RAS, 0, 0x60, 0x34, 0x24, 0x58, 8); reg_setup(REG_TEX, 2, 0x60, 0x58, 8, 0x5c, 4); reg_setup(REG_DICT, 4, 0x20, 0x18, 4, 0x1c, 4);
  for (i = 0; i < 64; i++) {      /* names: 0..40 chars of letters (both cases), digits, '_', '@'/'`'/'{' (around the a-z range), high-bit chars */
    static const char set[] = "abcxyzABCXYZ019_@`{[\x80\xe1\xfa\xc1";
    int len = rnd() % 8 == 0 ? 31 + rnd() % 3 : rnd() % 41, k;
    for (k = 0; k < len; k++) names[i][k] = set[rnd() % (sizeof set - 1)];
    names[i][len] = 0;
  }
}
static char qbuf[48];
static const char *query(void) {      /* a name of a texture with flipped case / one char changed / cut, or a random name */
  unsigned char *t; int k, n;
  if (!nt || rnd() % 4 == 0) return names[rnd() % 64];
  t = tp[rnd() % nt]; memcpy(qbuf, t + 0x10, 32); qbuf[32] = 0; n = (int)strlen(qbuf);
  for (k = 0; k < n; k++) if (rnd() % 3 == 0) { char c = qbuf[k]; if (c >= 'a' && c <= 'z') qbuf[k] = c - 32; else if (c >= 'A' && c <= 'Z') qbuf[k] = c + 32; }
  if (n && rnd() % 6 == 0) qbuf[rnd() % n] ^= 0x20;
  if (n && rnd() % 6 == 0) qbuf[rnd() % n] = 0;
  if (rnd() % 8 == 0) { qbuf[n] = 'q'; qbuf[n + 1] = 0; }
  return qbuf;
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 22, r = 0; unsigned char *a = 0, *b = 0;
    drv_ok = rnd() % 6 != 0; drv_write = rnd() % 5 != 0; alloc_fail = rnd() % 40 == 0; ctor_fail_off = rnd() % 25 == 0 ? (int)(rnd() % 4 == 0 ? 0x34 : 0x58) : -1;
    if (ctor_fail_off == 0x58 && rnd() % 2) ctor_fail_off = 0x18;
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: case 1: {      /* RwRasterCreate */
      int w = rnd() % 3 ? (int)(rnd() % 2049) : (int)rnd(), h = (int)(rnd() % 1025), d = (int)((u32[]){0, 8, 16, 24, 32}[rnd() % 5]);
      int fl = (rnd() % 8) | (rnd() % 3 == 0 ? 0x80 : 0) | (rnd() % 4 == 0 ? (rnd() & 0x78) : 0) | ((rnd() % 16) << 8) | (rnd() % 3 == 0 ? 0x8000 : 0) | (rnd() % 5 == 0 ? 0x1000 : 0);
      r = ((G4)f[RC])(w, h, d, fl); ncalls[RC]++; lg(r); lgobj((void *)(uintptr_t)r);
      if (r && nr < 256) rp[nr++] = (unsigned char *)(uintptr_t)r;
      break; }
    case 2: if (!nr) break; { int i = rnd() % nr; a = rp[i]; pool_del(rp, &nr, i); r = ((G1)f[RD])((u32)(uintptr_t)a); ncalls[RD]++; lg(r); break; }
    case 3: if (!nr) break; {      /* RwRasterSubRaster */
      int rect[4]; rect[0] = (int)(rnd() % 70000) - 35000; rect[1] = (int)(rnd() % 600) - 100; rect[2] = (int)(rnd() % 512); rect[3] = (int)(rnd() % 512);
      a = rp[rnd() % nr]; b = rp[rnd() % nr];
      r = ((G3)f[RSUB])((u32)(uintptr_t)a, (u32)(uintptr_t)b, (u32)(uintptr_t)rect); ncalls[RSUB]++; lg(r); lgobj(a); break; }
    case 4: if (!nr) break; a = rp[rnd() % nr]; r = ((G1)f[RLEV])((u32)(uintptr_t)a); ncalls[RLEV]++; lg(r); break;
    case 5: if (!nr) break; a = rp[rnd() % nr]; r = ((G3)f[RLOCK])((u32)(uintptr_t)a, rnd() % 3 ? rnd() % 12 : rnd(), rnd() % 8); ncalls[RLOCK]++; lg(r); lgobj(a); break;
    case 6: if (!nr) break; a = rp[rnd() % nr]; r = ((G1)f[RUNL])((u32)(uintptr_t)a); ncalls[RUNL]++; lg(r); lgobj(a); break;
    case 7: if (!nr) break; a = rp[rnd() % nr]; r = ((G2)f[RLPAL])((u32)(uintptr_t)a, rnd() % 3 ? rnd() % 8 : rnd()); ncalls[RLPAL]++; lg(r); lgobj(a); break;
    case 8: if (!nr) break; a = rp[rnd() % nr]; r = ((G1)f[RUPAL])((u32)(uintptr_t)a); ncalls[RUPAL]++; lg(r); lgobj(a); break;
    case 9: EW(RASMOD + 0x28) = rnd() % 10; r = ((G0)f[RCTX])(); ncalls[RCTX]++; lg(r);
            if (nr) { short xy[2] = {0x1234, 0x5678}; a = rp[rnd() % nr]; r = ((G3)f[ROFF])((u32)(uintptr_t)a, (u32)(uintptr_t)&xy[0], (u32)(uintptr_t)&xy[1]); ncalls[ROFF]++; lg(r); lg(*(u32 *)xy); }
            break;
    case 10: EW(TEXMOD + 0x20) = rnd(); r = ((G0)f[TAUTO])(); ncalls[TAUTO]++; lg(r); break;
    case 11: case 12: {      /* RwTextureCreate with a raster of the pool (it moves into the texture) or NULL */
      int i = nr && rnd() % 4 ? (int)(rnd() % nr) : -1; a = i >= 0 ? rp[i] : 0;
      r = ((G1)f[TCRE])((u32)(uintptr_t)a); ncalls[TCRE]++; lg(r); lgobj((void *)(uintptr_t)r);
      if (r) { if (i >= 0) pool_del(rp, &nr, i); if (nt < 256) tp[nt++] = (unsigned char *)(uintptr_t)r; }
      break; }
    case 13: case 14: if (!nt) break; {      /* RwTextureDestroy / destroyTexture; a texture with references left stays */
      int i = rnd() % nt, k = op == 13 ? TDES : TDES2; a = tp[i]; int refs = *(int *)(a + 0x54); u32 ras = *(u32 *)a, dict = *(u32 *)(a + 4);
      r = ((G1)f[k])((u32)(uintptr_t)a); ncalls[k]++; lg(r);
      if (refs <= 1) pool_del(tp, &nt, i); else lgobj(a);
      if (dict) lgobj((void *)(uintptr_t)dict);
      (void)ras; break; }
    case 15: if (!nt) break; a = tp[rnd() % nt]; (*(int *)(a + 0x54))++; break;      /* RwTextureAddRef (a macro in RW 3.7) */
    case 16: case 17: if (!nt) break; a = tp[rnd() % nt]; r = ((G2)f[op == 16 ? TNAME : TMASK])((u32)(uintptr_t)a, (u32)(uintptr_t)names[rnd() % 64]); ncalls[op == 16 ? TNAME : TMASK]++;
             lg(r); lgobj(a); lg(EW(ERRMOD)); lg(EW(ERRMOD + 4)); if (rnd() % 4 == 0) { EW(ERRMOD) = 0; EW(ERRMOD + 4) = 0x80000000u; } break;
    case 18: r = ((G0)f[DCRE])(); ncalls[DCRE]++; lg(r); lgobj((void *)(uintptr_t)r); if (r && nd < 64) dp[nd++] = (unsigned char *)(uintptr_t)r;
             if (r && rnd() % 2) EW(TEXMOD + 0x10) = r; break;      /* RwTexDictionarySetCurrent */
    case 19: if (!nd) break; {      /* RwTexDictionaryDestroy: its textures with one reference go, the others keep a dangling dict: dropped from the pool */
      int i = rnd() % nd, k; a = dp[i]; pool_del(dp, &nd, i);
      for (k = nt - 1; k >= 0; k--) if (*(u32 *)(tp[k] + 4) == (u32)(uintptr_t)a) pool_del(tp, &nt, k);
      r = ((G1)f[DDES])((u32)(uintptr_t)a); ncalls[DDES]++; lg(r); lg(EW(TEXMOD + 0x10)); lg(EW(TEXMOD)); lg(EW(TEXMOD + 4)); break; }
    case 20: if (!nd || !nt) break; a = dp[rnd() % nd]; b = tp[rnd() % nt];
             { u32 old = *(u32 *)(b + 4); r = ((G2)f[DADD])((u32)(uintptr_t)a, (u32)(uintptr_t)b); ncalls[DADD]++; lg(r); lgobj(a); lgobj(b); if (old) lgobj((void *)(uintptr_t)old); } break;
    case 21: if (!nd) break; { int n = 1 + rnd() % 4; a = dp[rnd() % nd]; while (n--) { const char *q = query(); r = ((G2)f[DFIND])((u32)(uintptr_t)a, (u32)(uintptr_t)q); ncalls[DFIND]++; nhit += r != 0; lg(r); } } break;
    }
  }
}

static u32 *save_log, nsave; static unsigned char *save_arena, save_eng[sizeof eng], save_reg[0x60];
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  if (memcmp(save_eng, eng, sizeof eng)) { for (i = 0; i < sizeof eng && save_eng[i] == eng[i]; i++); printf("  seed %u: engine instance differs at +0x%x\n", seed, i); return 1; }
  if (memcmp(save_reg, (void *)REG_TEX, 0x60)) { printf("  seed %u: plugin registries differ\n", seed); return 1; }
  if (memcmp(save_arena, arena, ARENA)) { for (i = 0; i < ARENA && save_arena[i] == arena[i]; i++); printf("  seed %u: object memory differs at block %u +0x%x\n", seed, i / BLK, i % BLK); return 1; }
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): dll %08x orig %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    return 1;
  }
  return 0;
}

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0; u32 seed; long tot = 0;
  void (__cdecl *bind)(void *, void *, void *, void *, void *, void *, void *);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  bind = (void *)GetProcAddress(m, "mflrw_exe_bind");
  if (!bind) { printf("mflrw_exe_bind MISSING export\n"); return 1; }
  bind((void *)ENGVAR, (void *)RASMODVAR, (void *)TEXMODVAR, (void *)REG_RAS, (void *)REG_TEX, (void *)REG_DICT, (void *)0x4ea160);
  for (i = 0; i < NF; i++) { fn[0][i] = (void *)GetProcAddress(m, F[i].dll); fn[1][i] = (void *)(uintptr_t)F[i].va; if (!fn[0][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA); memcpy(save_eng, eng, sizeof eng); memcpy(save_reg, (void *)REG_TEX, 0x60);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-38s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  printf("FindNamedTexture hits %ld, names of 32+ chars (error path) %ld\n", nhit, nlong);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
