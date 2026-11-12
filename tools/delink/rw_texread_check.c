/* rw_texread_check.c: bit-exact check of the librw DLL's texread exports (tools/delink/rw_texread.txt, capi/rwtexread.cpp) against the ORIGINAL code of
   MFL.exe, both run natively (Wine) on the same CPU.  usage: rw_texread_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [va:byte ...]
   (build / run: tools/delink/rw_texread_check.sh).  Image mapping and arena style as rw_g3_check.c / rw_imgread_check.c.
   Harness side of the exe (logging stand-ins, every call and result goes to one log; DLL and original run the same seeded operation sequences from the
   same reset state, then log, arena, engine instance and image module are compared):
     engine (RwEngineInstance 0x7d2128) string functions strcpy +0xcc, strrchr +0xdc, strchr +0xe0, strcmp +0xe8, strlen +0xf4, RwMalloc +0x108 / RwRealloc +0x110
     (deterministic arena, sometimes NULL), RwErrorSet 0x4ea160, the extension filter FUN_00561060 and the format readers / path callbacks.
   Stage 1 (image path search): ImageGetScratchMem 0x4dde80, ImagePathForAllFullNames 0x4ddd30, ImageAttempRead 0x4ddf40, RwImageFindFileType 0x4ddf70
   (its callback 0x4ddfa0 runs as original code on both sides), RwImageRead 0x4ddc40.
   Extra arguments va:byte patch the ORIGINAL's code first (mutation test: must give DIFF).  Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
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

#define ENGVAR 0x7d2128u
#define IMGMODVAR 0x7d2724u
#define IMOD 0x300
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))

/* log */
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 shash(const char *s) { u32 h = 2166136261u; if (!s) return 0; while (*s) h = (h ^ (u8)*s++) * 16777619u; return h; }
static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hr;      /* deterministic decision stream, advanced only by stand-ins (equal sequences stay equal) */
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }

/* arena for RwMalloc / RwRealloc */
#define ARENA (1u << 20)
static u8 *arena; static u32 ap, failmode;
static void *amalloc(u32 n) { u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); } p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p; }
static void *__cdecl s_malloc(u32 n, u32 hint) { lg(0xa108); lg(n); lg(hint); if (n > 8192 || (failmode && hrnd() % failmode == 0)) { lg(0); return 0; } { void *p = amalloc(n); lg((u32)(uintptr_t)p); return p; } }
static void *__cdecl s_realloc(void *o, u32 n, u32 hint) {
  lg(0xa110); lg((u32)(uintptr_t)o); lg(n); lg(hint);
  if (n > 8192 || (failmode && hrnd() % failmode == 0)) { lg(0); return 0; }
  { void *p = amalloc(n); memcpy(p, o, EW(IMOD + 0x214) < n ? EW(IMOD + 0x214) : n); lg((u32)(uintptr_t)p); return p; }
}
static char *__cdecl s_strcpy(char *d, const char *s) { lg(0xa1cc); lg((u32)(uintptr_t)d); lg(shash(s)); return strcpy(d, s); }
static char *__cdecl s_strrchr(const char *s, int c) { char *r = strrchr(s, c); lg(0xa1dc); lg((u32)(uintptr_t)s); lg(c); lg((u32)(uintptr_t)r); return r; }
static char *__cdecl s_strchr(const char *s, int c) { char *r = strchr(s, c); lg(0xa1e0); lg((u32)(uintptr_t)s); lg(c); lg((u32)(uintptr_t)r); return r; }
static int __cdecl s_strcmp(const char *a, const char *b) { int r = strcmp(a, b); lg(0xa1e8); lg(shash(a)); lg(shash(b)); lg(r); return r; }
static u32 __cdecl s_strlen(const char *s) { lg(0xa1f4); lg((u32)(uintptr_t)s); return (u32)strlen(s); }
static void *__cdecl herr(int32_t *e) { lg(0xa4a0); lg(e[0]); lg(e[1]); return e; }
static int __cdecl hfilter(const char *name) { u32 h = shash(name); lg(0xa561); lg((u32)(uintptr_t)name); lg(h); return h % 5 != 0; }      /* FUN_00561060 */
static const char *PATHS[] = {"", ";", "a", "a;b", "C:\\x;;y", "dir\\;dir2\\;", ";;z", "abc\\def;ghi", "/p/q/;/r"};
static const char *NAMES[] = {"tex.bmp", "C:\\a\\tex.png", "\\x\\y.tga", "noext", "a.b.c.BMP", "", "x:", "d/e/f.ras", "a:b/c\\d.png", "t.", ".bmp", "dir/sub.dir/file", "Z:\\q.tga", "..\\x.RAS", "k.png", "TEX.BMP", "Tex.Bmp", "zz.bmp", "ZZ.BMP", "a`{@z.tga", "A`{@Z.TGA"};
#define NPATHS 9
#define NNAMES 21
/* images: RwImage {flags, width, height, depth, stride, pixels, palette} + plugin area, blocks of 0x40 from the arena */
static u32 *mkimage(void) { u32 *im = amalloc(0x40); im[0] = hrnd() % 2; im[1] = hrnd() % 20; im[2] = hrnd() % 9; im[3] = 8; im[4] = im[1]; im[5] = im[0] ? (u32)(uintptr_t)amalloc(16) : 0; im[6] = im[0] ? (u32)(uintptr_t)amalloc(16) : 0; return im; }
static u32 __cdecl rd_a(const char *name) { u32 r; lg(0xa0a0); lg((u32)(uintptr_t)name); lg(shash(name)); r = shash(name) % 3 == 0 ? (u32)(uintptr_t)mkimage() : 0; lg(r); return r; }
static u32 __cdecl rd_b(const char *name) { u32 r; lg(0xa0b0); lg((u32)(uintptr_t)name); lg(shash(name)); r = hrnd() % 2 ? (u32)(uintptr_t)mkimage() : 0; lg(r); return r; }
static void *__cdecl s_free(void *p) { lg(0xa10c); lg((u32)(uintptr_t)p); return 0; }
static void *__cdecl fl_alloc(void *list, u32 hint) { void *p; lg(0xa118); lg((u32)(uintptr_t)list); lg(hint); p = amalloc(0x40); lg((u32)(uintptr_t)p); return p; }
static void __cdecl fl_free(void *list, void *p) { lg(0xa11c); lg((u32)(uintptr_t)list); lg((u32)(uintptr_t)p); }
static int __cdecl h_makemask(u32 *im) { int r = hrnd() % 4 != 0; lg(0xa700); lg((u32)(uintptr_t)im); lg(r); return r; }
static int __cdecl h_applymask(u32 *a, u32 *b) { int r = hrnd() % 4 != 0; lg(0xa7d0); lg((u32)(uintptr_t)a); lg((u32)(uintptr_t)b); lg(r); return r; }
/* memory stream: chunk finder (found 5 of 6), reader, skipper */
static u8 sfile[1024]; static u32 slen, spos; static u32 stok[4];
static u32 chunklen = 77;
static int __cdecl h_findchunk(void *s, u32 type, u32 *len, u32 *ver) {
  int r = hrnd() % 6 != 0; u32 v = hrnd() % 7 == 0 ? (hrnd() % 2 ? 0x34fffu : 0x37003u) : 0x35000u + hrnd() % 0x2003;
  lg(0xa720); lg((u32)(uintptr_t)s); lg(type); lg(r); if (r) { *len = type == 1 ? chunklen : 12; *ver = v; lg(v); } return r;
}
static u32 __cdecl h_sread(void *s, void *buf, u32 n) { u32 k = n < slen - spos ? n : slen - spos; lg(0xa6e0); lg((u32)(uintptr_t)s); lg((u8 *)buf >= arena && (u8 *)buf < arena + ARENA ? (u32)(uintptr_t)buf : 0x57ac); lg(n); memcpy(buf, sfile + spos, k); spos += k; lg(k); return k; }
static void *__cdecl h_sskip(void *s, u32 n) { lg(0xa190); lg((u32)(uintptr_t)s); lg(n); spos = n > slen - spos ? slen : spos + n; return s; }
static void *__cdecl h_allocpix(u32 *im) {
  u32 w = im[1], h = im[2], d = im[3], stride = w * ((d + 7) / 8) + (hrnd() % 3 == 0 ? hrnd() % 5 : 0); lg(0xa610); lg((u32)(uintptr_t)im);
  im[4] = stride; im[5] = (u32)(uintptr_t)amalloc(stride * ((int32_t)h > 0 ? h : 0) + 8 + 1024); im[6] = d <= 8 ? (u32)(uintptr_t)amalloc(1024) : 0; im[0] |= 1; lg(stride); return im;
}
/* plugin registry of the image (0x62e848): two entries, constructors write the bytes at their offset, one fails sometimes */
static u32 entries[2][14];
static void *__cdecl p_ctor(unsigned char *o, int off, int size) { int i; lg(0xc7); lg((u32)(uintptr_t)o); lg(off); for (i = 0; i < size; i++) o[off + i] = (unsigned char)(off * 3 + i * 7); return off == 0x1c && hrnd() % 10 == 0 ? 0 : o; }
static void *__cdecl p_dtor(unsigned char *o, int off, int size) { lg(0xd7); lg((u32)(uintptr_t)o); lg(off); memset(o + off, 0xdd, size); return o; }
static void reg_setup(u32 reg) {
  u32 *a = entries[0], *b = entries[1]; memset(a, 0, 56); memset(b, 0, 56);
  a[0] = 0x1c; a[1] = 4; a[8] = (u32)(uintptr_t)p_ctor; a[9] = (u32)(uintptr_t)p_dtor; a[12] = (u32)(uintptr_t)b;
  b[0] = 0x20; b[1] = 8; b[8] = (u32)(uintptr_t)p_ctor; b[9] = (u32)(uintptr_t)p_dtor; b[13] = (u32)(uintptr_t)a;
  W(reg) = 0x28; W(reg + 4) = 0x28; W(reg + 8) = 0x28; W(reg + 0xc) = 0; W(reg + 0x10) = (u32)(uintptr_t)a; W(reg + 0x14) = (u32)(uintptr_t)b;
}
/* texture module (module offset var 0x7d2184): dictionaries and textures live in the arena */
#define TEXVAR 0x7d2184u
#define TMOD 0x700
static u8 *tpool[64], *dpool[8]; static int ntp, ndp;
static u8 *mktex(const char *name, u8 *dict) {
  u8 *t = amalloc(0x60); memset(t, 0, 0x60); strncpy((char *)t + 0x10, name, 31); *(int *)(t + 0x54) = 1;
  if (dict) { u32 *head = (u32 *)(dict + 8); *(u32 *)(t + 4) = (u32)(uintptr_t)dict; *(u32 *)(t + 0xc) = (u32)(uintptr_t)head; *(u32 *)(t + 8) = head[0]; *(u32 *)(uintptr_t)(head[0] + 4) = (u32)(uintptr_t)(t + 8); head[0] = (u32)(uintptr_t)(t + 8); }
  if (ntp < 64) tpool[ntp++] = t;
  return t;
}
static u8 *__cdecl t_find(const char *name) { u8 *t = ntp && hrnd() % 3 == 0 ? tpool[hrnd() % ntp] : 0; lg(0xa618); lg(shash(name)); lg((u32)(uintptr_t)t); return t; }
static u8 *__cdecl t_read(const char *name, const char *mask) {
  u8 *t = 0; lg(0xa614); lg(shash(name)); lg(shash(mask)); lg(mask != 0); lg(EW(TMOD + 0x1c)); lg(EW(TMOD + 0x20));
  if (hrnd() % 4 != 0) t = mktex(name, ndp && hrnd() % 3 == 0 ? dpool[hrnd() % ndp] : 0);
  lg((u32)(uintptr_t)t); return t;
}
static u32 __cdecl t_gen(void *r, void *im) { u32 v = hrnd() % 3 == 0 ? 0 : hrnd(); lg(0xa62c); lg((u32)(uintptr_t)r); lg((u32)(uintptr_t)im); lg(v); return v; }
static int __cdecl h_addplugin(void *reg, int a, int b, void *c, void *d, void *e) { int r = (int)hrnd(); lg(0xa9ad); lg((u32)(uintptr_t)reg); lg(a); lg(b); lg((u32)(uintptr_t)c); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)e); lg(r); return r; }
static int __cdecl h_strfr(char *buf, void *s) { int r = hrnd() % 7 != 0; lg(0xa4b0); lg((u32)(uintptr_t)s); if (r) strcpy(buf, NAMES[hrnd() % NNAMES]); lg(r); if (r) lg(shash(buf)); return r; }
static int __cdecl h_skipch(void *reg, void *s) { int r = hrnd() % 5 != 0; lg(0xa130); lg((u32)(uintptr_t)reg); lg((u32)(uintptr_t)s); lg(r); return r; }
static int __cdecl h_readch(void *reg, void *s, void *obj) { int r = hrnd() % 5 != 0; lg(0xa1ea); lg((u32)(uintptr_t)reg); lg((u32)(uintptr_t)s); lg((u32)(uintptr_t)obj); lg(r); return r; }
static int __cdecl h_devreq(void *dev, int req, u32 *out, int a, int b) { static const u32 ids[] = {0, 1, 2, 0x101}; *out = ids[hrnd() % 4]; lg(0xa2e2); lg((u32)(uintptr_t)dev); lg(req); lg(a); lg(b); lg(*out); return 1; }
static u8 *__cdecl h_tdcreate(void) {
  u8 *d = 0; u32 *head = (u32 *)(eng + TMOD); lg(0xa5a2);
  if (hrnd() % 8 != 0) {
    u32 *th, *li; d = amalloc(0x20); th = (u32 *)(d + 8); li = (u32 *)(d + 0x10); memset(d, 0, 0x20); d[0] = 6; th[0] = th[1] = (u32)(uintptr_t)th;
    li[0] = head[0]; li[1] = (u32)(uintptr_t)head; *(u32 *)(uintptr_t)(head[0] + 4) = (u32)(uintptr_t)li; head[0] = (u32)(uintptr_t)li;
  }
  lg((u32)(uintptr_t)d); return d;
}
static u8 *__cdecl h_tdadd(u8 *d, u8 *t) {
  u32 *head = (u32 *)(d + 8), *l = (u32 *)(t + 8); lg(0xa5d5); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)t);
  if (*(u32 *)(t + 4)) { ((u32 *)(uintptr_t)l[1])[0] = l[0]; ((u32 *)(uintptr_t)l[0])[1] = l[1]; }
  *(u32 *)(t + 4) = (u32)(uintptr_t)d; l[1] = (u32)(uintptr_t)head; l[0] = head[0]; ((u32 *)(uintptr_t)head[0])[1] = (u32)(uintptr_t)l; head[0] = (u32)(uintptr_t)l;
  return t;
}
static void *__cdecl h_forall(void *d, void *cb, void *arg) { lg(0xa5b5); lg((u32)(uintptr_t)d); lg((u32)(uintptr_t)cb); lg((u32)(uintptr_t)arg); return d; }
static int __cdecl h_tddestroy(void *d) { lg(0xa5ac); lg((u32)(uintptr_t)d); return 1; }
static int __cdecl h_native(void *s, u8 **out, u32 len) {
  int r = hrnd() % 6 != 0; lg(0xa0b0 + 1); lg((u32)(uintptr_t)s); lg(len);
  if (r) *out = hrnd() % 5 ? mktex(NAMES[hrnd() % NNAMES], 0) : 0;
  lg(r); if (r) lg((u32)(uintptr_t)*out); return r;
}
static int __cdecl h_setras(u32 *t, u32 r, int z) { int ok = hrnd() % 3 != 0; lg(0xa068); lg((u32)(uintptr_t)t); lg(r); lg(z); if (ok) t[0] = r; lg(ok); return ok; }
static void mktexhdr(int dict) {
  u32 hdr = dict ? (rnd() % 5) | ((rnd() % 4 == 0 ? rnd() % 3 : 0) << 16) | (rnd() % 8 == 0 ? 1u << 16 : 0)
                 : (rnd() % 9) | ((rnd() % 16) << 8) | (rnd() % 3 == 0 ? (rnd() % 16) << 12 : 0) | ((rnd() & 0xff) << 16) | ((rnd() & 0xff) << 24);
  slen = 64 + rnd() % 64; spos = 0; memset(sfile, 0, sizeof sfile); *(u32 *)sfile = hdr; chunklen = rnd() % 8 == 0 ? 2 + rnd() % 2 : 4;
}
static void mktexmod(void) {
  u32 *head = (u32 *)(eng + TMOD); int i, j, nd = rnd() % 4; ntp = ndp = 0;
  head[0] = head[1] = (u32)(uintptr_t)head;
  for (i = 0; i < nd; i++) {
    u8 *d = amalloc(0x20); u32 *th = (u32 *)(d + 8), *li = (u32 *)(d + 0x10); memset(d, 0, 0x20); d[0] = 6;
    th[0] = th[1] = (u32)(uintptr_t)th;
    li[0] = head[0]; li[1] = (u32)(uintptr_t)head; *(u32 *)(uintptr_t)(head[0] + 4) = (u32)(uintptr_t)li; head[0] = (u32)(uintptr_t)li;
    dpool[ndp++] = d;
    for (j = rnd() % 4; j; j--) mktex(NAMES[rnd() % NNAMES], d);
  }
  EW(TMOD + 0x10) = ndp && rnd() % 3 ? (u32)(uintptr_t)dpool[rnd() % ndp] : 0;
  EW(TMOD + 0x14) = (u32)(uintptr_t)t_read; EW(TMOD + 0x18) = (u32)(uintptr_t)t_find; EW(TMOD + 0x2c) = (u32)(uintptr_t)t_gen;
  EW(TMOD + 0x1c) = rnd() % 3; EW(TMOD + 0x20) = rnd() % 3; EW(0xb0) = (u32)(uintptr_t)h_native; EW(0x68) = (u32)(uintptr_t)h_setras;
  W(TEXVAR) = TMOD;
}
static void mkstream(void) {      /* header {width, height, depth, file stride} then random bytes */
  static const u32 dep[] = {4, 8, 16, 32}; u32 w = 1 + rnd() % 20, h = rnd() % 9 - 1, d = dep[rnd() % 4], s0 = w * ((d + 7) / 8), st = rnd() % 3 ? s0 : rnd() % (2 * s0 + 5), k;
  slen = 16 + rnd() % 500; spos = 0; chunklen = 77; for (k = 0; k < slen; k++) sfile[k] = (u8)rnd();
  ((u32 *)sfile)[0] = w; ((u32 *)sfile)[1] = h; ((u32 *)sfile)[2] = d; ((u32 *)sfile)[3] = st;
}
static int __cdecl cb_h(char *full, void *ctx) { int r = hrnd() % 4 != 0; lg(0xa0cb); lg(shash(full)); lg((u32)(uintptr_t)ctx); lg(r); return r; }

/* inputs (PATHS / NAMES above) */
static char pathbuf[64];
/* image format list: {ext, ext2, reader, next} entries */
static u8 ent[5][0x34];
static void ent_set(int i, const char *e1, const char *e2, void *rd) { memset(ent[i], 0, 0x34); strcpy((char *)ent[i], e1); strcpy((char *)ent[i] + 0x14, e2); *(u32 *)(ent[i] + 0x28) = (u32)(uintptr_t)rd; *(u32 *)(ent[i] + 0x30) = i < 4 ? (u32)(uintptr_t)ent[i + 1] : 0; }

enum { GS, PA, AR, FT, IR, SR, RM, TSC, TGC, TGM, TRP, TRD, TDF, TSM, TGMM, TSA, TSR, DSR, TRS, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_ImageGetScratchMem", 0x4dde80}, {"rw37_ImagePathForAllFullNames", 0x4ddd30}, {"rw37_ImageAttempRead", 0x4ddf40},
  {"rw37_RwImageFindFileType", 0x4ddf70}, {"rw37_RwImageRead", 0x4ddc40}, {"rw37_RwImageStreamRead", 0x4de740}, {"rw37_RwImageReadMaskedImage", 0x4de220},
  {"rw37_RwTexDictionarySetCurrent", 0x4d5e10}, {"rw37_RwTexDictionaryGetCurrent", 0x4d5e30}, {"rw37_RwTextureRasterGenerateMipmaps", 0x4d5f60}, {"rw37_RwTextureRegisterPlugin", 0x4d5f30},
  {"rw37_RwTextureRead", 0x4d5e40}, {"rw37_TextureDefaultFind", 0x4d7620}, {"rw37_RwTextureSetMipmapping", 0x4d5990}, {"rw37_RwTextureGetMipmapping", 0x4d59b0},
  {"rw37_RwTextureSetAutoMipmapping", 0x4d59c0}, {"rw37_RwTextureStreamRead", 0x4df350}, {"rw37_RwTexDictionaryStreamRead", 0x4df580}, {"rw37_RwTextureSetRaster", 0x4d59f0}};
typedef u32 (__cdecl *G0)(void); typedef u32 (__cdecl *G5)(u32, u32, u32, u32, u32); typedef u32 (__cdecl *G1)(u32); typedef u32 (__cdecl *G2)(u32, u32); typedef u32 (__cdecl *G4)(u32, u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF], nfound;
static u32 ctxv[2];

static void reset(u32 seed) {
  nlg = 0; ap = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA);
  failmode = rnd() % 3 == 0 ? 3 + rnd() % 5 : 0;
  memset(eng, 0, sizeof eng);
  W(ENGVAR) = (u32)(uintptr_t)eng; W(IMGMODVAR) = IMOD;
  EW(0xcc) = (u32)(uintptr_t)s_strcpy; EW(0xdc) = (u32)(uintptr_t)s_strrchr; EW(0xe0) = (u32)(uintptr_t)s_strchr; EW(0xe8) = (u32)(uintptr_t)s_strcmp;
  EW(0xf4) = (u32)(uintptr_t)s_strlen; EW(0x108) = (u32)(uintptr_t)s_malloc; EW(0x110) = (u32)(uintptr_t)s_realloc;
  EW(0x10c) = (u32)(uintptr_t)s_free; EW(0x118) = (u32)(uintptr_t)fl_alloc; EW(0x11c) = (u32)(uintptr_t)fl_free; EW(IMOD) = 0x33330000u; reg_setup(0x62e848);
  strcpy(pathbuf, PATHS[rnd() % NPATHS]);
  EW(IMOD + 4) = rnd() % 8 == 0 ? 0 : (u32)(uintptr_t)pathbuf;
  if (rnd() % 3 == 0) { u32 n = 1 + rnd() % 40; EW(IMOD + 0x210) = (u32)(uintptr_t)amalloc(n); EW(IMOD + 0x214) = n; }
  ent_set(0, ".bmp", ".BMP", rd_a); ent_set(1, ".png", ".PNG", rd_b); ent_set(2, ".tga", ".TGA", 0); ent_set(3, ".ras", ".RAS", rd_a); ent_set(4, "", ".q", rd_b);
  EW(IMOD + 0x21c) = rnd() % 10 ? (u32)(uintptr_t)ent[0] : 0;
  mktexmod();
}

static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 19, r = 0; const char *name = NAMES[rnd() % NNAMES];
    lg(0x57e90000u + s); lg(op);
    switch (op) {
    case 0: { u32 n = rnd() % 4 == 0 ? 0 : rnd() % 3 ? rnd() % 100 : rnd() % 4 == 0 ? 0x7fffffffu : rnd() % 70; r = ((G1)f[GS])(n); ncalls[GS]++; lg(r); break; }
    case 1: { u32 extra = rnd() % 4 ? rnd() % 24 : 0; ctxv[0] = ctxv[1] = 0x1234; r = ((G4)f[PA])((u32)(uintptr_t)name, extra, (u32)(uintptr_t)cb_h, (u32)(uintptr_t)ctxv); ncalls[PA]++; lg(r); break; }
    case 2: { ctxv[0] = (u32)(uintptr_t)(rnd() % 2 ? rd_a : rd_b); ctxv[1] = 0x777; r = ((G2)f[AR])((u32)(uintptr_t)name, (u32)(uintptr_t)ctxv); ncalls[AR]++; lg(r); lg(ctxv[1]); break; }
    case 3: r = ((G1)f[FT])((u32)(uintptr_t)name); ncalls[FT]++; lg(r); break;
    case 4: r = ((G1)f[IR])((u32)(uintptr_t)name); ncalls[IR]++; nfound += r != 0; lg(r); break;
    case 5: mkstream(); r = ((G1)f[SR])((u32)(uintptr_t)stok); ncalls[SR]++; lg(r); if (r) { int k; for (k = 0; k < 0x1c; k += 4) lg(*(u32 *)(uintptr_t)(r + k)); } lg(spos); break;
    case 6: { u32 pick = rnd() % 5; const char *mask = pick == 0 ? 0 : pick == 1 ? "" : NAMES[rnd() % NNAMES]; r = ((G2)f[RM])((u32)(uintptr_t)name, (u32)(uintptr_t)mask); ncalls[RM]++; lg(r); break; }
    case 7: { u32 d = ndp && rnd() % 4 ? (u32)(uintptr_t)dpool[rnd() % ndp] : 0; r =((G1)f[TSC])(d); ncalls[TSC]++; lg(r); r = ((G0)f[TGC])(); ncalls[TGC]++; lg(r); break; }
    case 8: r = ((G2)f[TGM])(0x11110000u + rnd() % 256, 0x22220000u + rnd() % 256); ncalls[TGM]++; lg(r); break;
    case 9: r = ((G5)f[TRP])(rnd(), rnd(), rnd(), rnd(), rnd()); ncalls[TRP]++; lg(r); break;
    case 10: case 11: { u32 pick = rnd() % 4; const char *mask = pick == 0 ? 0 : NAMES[rnd() % NNAMES]; int k;
      r = ((G2)f[TRD])((u32)(uintptr_t)name, (u32)(uintptr_t)mask); ncalls[TRD]++; lg(r); if (r) { lg(*(u32 *)(uintptr_t)(r + 4)); lg(*(u32 *)(uintptr_t)(r + 0x54)); }
      for (k = 0; k < ndp; k++) { lg(*(u32 *)(dpool[k] + 8)); lg(*(u32 *)(dpool[k] + 0xc)); } break; }
    case 12: r = ((G1)f[TDF])((u32)(uintptr_t)name); ncalls[TDF]++; lg(r); break;
    case 13: r = ((G1)f[TSM])(rnd() % 3); ncalls[TSM]++; lg(r); r = ((G0)f[TGMM])(); ncalls[TGMM]++; lg(r); r = ((G1)f[TSA])(rnd() % 3); ncalls[TSA]++; lg(r); lg(EW(TMOD + 0x1c)); lg(EW(TMOD + 0x20)); break;
    case 14: case 15: { int k; mktexhdr(0); r = ((G1)f[TSR])((u32)(uintptr_t)stok); ncalls[TSR]++; lg(r); if (r) { lg(*(u32 *)(uintptr_t)(r + 4)); lg(*(u32 *)(uintptr_t)(r + 0x50)); lg(*(u32 *)(uintptr_t)(r + 0x54)); } lg(spos); lg(EW(TMOD + 0x1c)); lg(EW(TMOD + 0x20));
      for (k = 0; k < ndp; k++) { lg(*(u32 *)(dpool[k] + 8)); lg(*(u32 *)(dpool[k] + 0xc)); } break; }
    case 18: { u8 *t = ntp ? tpool[rnd() % ntp] : 0; if (!t) break; *(u32 *)t = 0x7777; r = ((G2)f[TRS])((u32)(uintptr_t)t, rnd() % 3 ? 0x4000 + rnd() % 64 : 0); ncalls[TRS]++; lg(r); lg(*(u32 *)t); break; }
    case 16: case 17: mktexhdr(1); r = ((G1)f[DSR])((u32)(uintptr_t)stok); ncalls[DSR]++; lg(r); if (r) { lg(*(u32 *)(uintptr_t)(r + 8)); lg(*(u32 *)(uintptr_t)(r + 0xc)); } lg(spos); break;
    }
    lg(EW(IMOD + 0x210)); lg(EW(IMOD + 0x214));
  }
}

static u32 *save_log, nsave; static unsigned char *save_arena, save_eng[sizeof eng];
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  if (memcmp(save_eng, eng, sizeof eng)) { for (i = 0; i < sizeof eng && save_eng[i] == eng[i]; i++); printf("  seed %u: engine instance differs at +0x%x\n", seed, i); return 1; }
  if (memcmp(save_arena, arena, ARENA)) { for (i = 0; i < ARENA && save_arena[i] == arena[i]; i++); printf("  seed %u: arena differs at +0x%x\n", seed, i); return 1; }
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): dll %08x orig %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    return 1;
  }
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
  void (__cdecl *bind)(void *, void *, void *, void *);
  static void *tbl[24];
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  jmp_patch(0x4ea160, (void *)herr); jmp_patch(0x561060, (void *)hfilter); jmp_patch(0x4dd700, (void *)h_makemask); jmp_patch(0x4dd7d0, (void *)h_applymask);
  jmp_patch(0x4dc720, (void *)h_findchunk); jmp_patch(0x4dbe70, (void *)h_sread); jmp_patch(0x4dc190, (void *)h_sskip); jmp_patch(0x4dd610, (void *)h_allocpix); jmp_patch(0x4e9ad0, (void *)h_addplugin);
  jmp_patch(0x4ea4b0, (void *)h_strfr); jmp_patch(0x4f2130, (void *)h_skipch); jmp_patch(0x4f1ea0, (void *)h_readch); jmp_patch(0x4d2e20, (void *)h_devreq);
  jmp_patch(0x4d5a20, (void *)h_tdcreate); jmp_patch(0x4d5d50, (void *)h_tdadd); jmp_patch(0x4d5b50, (void *)h_forall); jmp_patch(0x4d5ac0, (void *)h_tddestroy);
  bind = (void *)GetProcAddress(m, "mflrw_texread_bind");
  if (!bind) { printf("mflrw_texread_bind MISSING export\n"); return 1; }
  { static const u32 t[] = {0x4ea160, 0x4f1bb0, 0x561060, 0x4ddfa0, 0x4dd700, 0x4dd7d0, 0x4dc720, 0x4dbe70, 0x4dc190, 0x4dd610, 0x62e848, 0x4b2c30, 0x4e9ad0, 0x62e6d0,
                              0x4ea4b0, 0x4f2130, 0x4f1ea0, 0x4d2e20, 0x4d5a20, 0x4d5d50, 0x4d5b50, 0x4d5ac0, 0x4df790, 0x62e6e8}; int k; for (k = 0; k < 24; k++) tbl[k] = (void *)(uintptr_t)t[k]; }
  bind((void *)ENGVAR, (void *)IMGMODVAR, (void *)TEXVAR, tbl);
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
  for (i = 0; i < NF; i++) printf("%-38s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  printf("RwImageRead found an image %ld times\n", nfound);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
