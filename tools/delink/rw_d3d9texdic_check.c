/* rw_d3d9texdic_check.c: behavioural check of the librw DLL's d3d9texdic exports (tools/delink/rw_d3d9texdic.txt: the native texture reader / size of d3d9texdic.obj,
   librw-mfl capi/d3d9texdic.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9texdic_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3d9texdic_check.sh; image mapping as rw_d3drs_check.c).
   The exe device global 0x7d2240 holds the fake device of tools/delink/fakedev.c with this check's CreateTexture 23 / CreateCubeTexture 25 recorders; the textures
   are this check's fakes (2D: GetLevelCount 13, GetLevelDesc 17, LockRect 19, UnlockRect 20; cube: the same slots with the face argument).  Every exe callee
   (RwStreamFindChunk / Read, the format checks, RwRasterCreate / Destroy / Lock / Unlock / LockPalette / UnlockPalette, RwD3D9RasterCreate, RwTextureCreate /
   SetName / SetMaskName) gets a jmp to a recorder at its original entry; the stream is a scripted byte sequence (a plausible native texture most of the time,
   truncated sometimes).  Both sides make the same calls into the same recorders.
   Per seed and control word (0x7f, 0x27f) one random operation sequence runs twice from the same reset state, first through the DLL exports, then through the
   original addresses.  Logged after every op: the recorder / device calls, the return value, the control word, the size or texture out word, the last raster's
   bytes +0x21 / +0x23 and its extension (0x20 bytes), the texture's filter / address word.
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
#define RASEXT 0x947144u
#define CAPS 0x947600u
#define EXT 0x40u
#define T 0xc0de0000u

static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hashw(const u8 *p, u32 n) { u32 h = 0x811c9dc5u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 0x01000193u; return h; }

static u8 *oa; static u32 noa;
#define ARENA 0x2000000u
static u32 oalloc(u32 n) { u8 *p; if (noa + n + 16 > ARENA) { printf("OBJ ARENA full\n"); ExitProcess(6); } p = oa + noa; noa += (n + 15) & ~15u; memset(p, 0, n); return (u32)(uintptr_t)p; }

/* fake textures: {vtbl, id, levels, kind} */
static void *tex2d_vt[22], *texcube_vt[22];
static void fdt_trap(int n) { printf("TRAP unexpected texture method %d\n", n); fflush(stdout); ExitProcess(5); }
#define TT(n) static long __stdcall tt_##n(void *s) { fdt_trap(n); return (long)0x8876086c; }
TT(0) TT(1) TT(2) TT(3) TT(4) TT(5) TT(6) TT(7) TT(8) TT(9) TT(10) TT(11) TT(12) TT(14) TT(15) TT(16) TT(18) TT(21)
static u32 lvpitch(u32 t, u32 l) { return (W(t + 4) * 7 + l * 13) % 97 + (l & 1) * 0x100; }
static u32 lvheight(u32 t, u32 l) { return (W(t + 4) * 3 + l * 5) % 11; }
static u32 __stdcall t_count(u32 t) { lg(T | 0x40d); lg(W(t + 4)); lg(W(t + 8)); return W(t + 8); }
static long __stdcall t_desc(u32 t, u32 l, u32 *d) { int i; lg(T | 0x411); lg(W(t + 4)); lg(l); for (i = 0; i < 8; i++) d[i] = 0x11110000u + i; d[7] = lvheight(t, l); return 0; }
static long __stdcall t_lock(u32 t, u32 l, u32 *lr, void *rect, u32 fl) { lg(T | 0x413); lg(W(t + 4)); lg(l); lg((u32)(uintptr_t)rect); lg(fl); lr[0] = lvpitch(t, l); lr[1] = 0xb17e0000u; return 0; }
static long __stdcall t_unlock(u32 t, u32 l) { lg(T | 0x414); lg(W(t + 4)); lg(l); return 0; }
static long __stdcall c_desc(u32 t, u32 l, u32 *d) { return t_desc(t, l, d); }
static long __stdcall c_lock(u32 t, u32 face, u32 l, u32 *lr, void *rect, u32 fl) { lg(T | 0x513); lg(face); return t_lock(t, l, lr, rect, fl); }
static long __stdcall c_unlock(u32 t, u32 face, u32 l) { lg(T | 0x514); lg(face); return t_unlock(t, l); }
static void tex_vt_init(void) {
  void *tr[22] = { tt_0, tt_1, tt_2, tt_3, tt_4, tt_5, tt_6, tt_7, tt_8, tt_9, tt_10, tt_11, tt_12, 0, tt_14, tt_15, tt_16, 0, tt_18, 0, 0, tt_21 }; int i;
  for (i = 0; i < 22; i++) tex2d_vt[i] = texcube_vt[i] = tr[i];
  tex2d_vt[13] = texcube_vt[13] = (void *)t_count; tex2d_vt[17] = (void *)t_desc; texcube_vt[17] = (void *)c_desc;
  tex2d_vt[19] = (void *)t_lock; tex2d_vt[20] = (void *)t_unlock; texcube_vt[19] = (void *)c_lock; texcube_vt[20] = (void *)c_unlock;
}
static u32 ntex;
static u32 mktex(int cube) { u32 t = oalloc(16); W(t) = (u32)(uintptr_t)(cube ? texcube_vt : tex2d_vt); W(t + 4) = 0x7e000000u + ++ntex; W(t + 8) = rnd() % 5 ? rnd() % 6 : rnd() % 12; W(t + 12) = cube; return t; }
static int32_t texhr;
static long __stdcall d_createtex(void *d, u32 w, u32 h, u32 lv, u32 usage, u32 fmt, u32 pool, u32 *pp, void *share) {
  lg(FAKEDEV_TAG | 23); lg(w); lg(h); lg(lv); lg(usage); lg(fmt); lg(pool); lg((u32)(uintptr_t)share); lg((u32)texhr);
  if (texhr >= 0) { *pp = mktex(0); lg(W(*pp + 4)); } return texhr;
}
static long __stdcall d_createcube(void *d, u32 e, u32 lv, u32 usage, u32 fmt, u32 pool, u32 *pp, void *share) {
  lg(FAKEDEV_TAG | 25); lg(e); lg(lv); lg(usage); lg(fmt); lg(pool); lg((u32)(uintptr_t)share); lg((u32)texhr);
  if (texhr >= 0) { *pp = mktex(1); lg(W(*pp + 4)); } return texhr;
}

/* the scripted stream */
static u8 sbuf[0x10000]; static u32 slen, spos, cur_fmt, last_r, last_t, lastlock, lastpal;
static u32 __cdecl r_findchunk(u32 s, u32 type, u32 *len, u32 *ver) {
  static const u32 vers[] = { 0x36003, 0x35000, 0x37002, 0x34fff, 0x37003, 0x36003, 0x36003 }; u32 ok = rnd() % 12 ? 1 : 0;
  lg(T | 1); lg(s); lg(type); lg(ok); if (ok) { *len = rnd() % 0x1000; *ver = vers[rnd() % 7]; lg(*len); lg(*ver); } return ok;
}
static u32 __cdecl r_read(u32 s, u8 *buf, u32 n) { u32 k = n < slen - spos ? n : slen - spos; lg(T | 2); lg(s); lg(n); lg(k); memcpy(buf, sbuf + spos, k); spos += k; return k; }
static u32 __cdecl r_checkvalid(u32 fmt) { u32 r = rnd() % 8 ? 1 : 0; lg(T | 3); lg(fmt); lg(r); return r; }
static u32 __cdecl r_automip(u32 fmt) { u32 r = rnd() % 2; lg(T | 4); lg(fmt); lg(r); return r; }
static u32 __cdecl r_automipcube(u32 fmt) { u32 r = rnd() % 2; lg(T | 5); lg(fmt); lg(r); return r; }
static u32 mkraster(u32 w, u32 h, u32 flags, u32 fmt) {
  u32 r = oalloc(EXT + 0x20), e = r + EXT;
  W(r + 0xc) = rnd() % 10 ? w : w + 1; W(r + 0x10) = rnd() % 10 ? h : h ^ 2; W8(r + 0x21) = rnd(); W8(r + 0x23) = rnd() % 8 ? flags >> 8 : rnd();
  W8(e + 8) = rnd(); W8(e + 9) = rnd() % 3 ? 0 : rnd(); W8(e + 0xa) = rnd() % 3 ? 0 : rnd(); W(e + 0x18) = rnd() % 8 ? fmt : rnd() % 4;
  last_r = r; return r;
}
static u32 __cdecl r_rascreate(u32 w, u32 h, u32 d, u32 flags) { u32 r = 0; lg(T | 6); lg(w); lg(h); lg(d); lg(flags); if (rnd() % 12) r = mkraster(w, h, flags, cur_fmt); lg(r); return r; }
static u32 __cdecl r_d3drascreate(u32 w, u32 h, u32 fmt, u32 flags) { u32 r = 0; lg(T | 7); lg(w); lg(h); lg(fmt); lg(flags); if (rnd() % 12) r = mkraster(w, h, flags, fmt); lg(r); return r; }
static u32 __cdecl r_lockpal(u32 r, u32 m) { lg(T | 8); lg(r); lg(m); lastpal = oalloc(0x400); memset((void *)(uintptr_t)lastpal, 0xcd, 0x400); return lastpal; }
static u32 __cdecl r_unlockpal(u32 r) { lg(T | 9); lg(r); lg(hashw((u8 *)(uintptr_t)lastpal, 0x400)); return r; }
static u32 __cdecl r_rasdestroy(u32 r) { lg(T | 10); lg(r); return 1; }
static u32 __cdecl r_lock(u32 r, u32 l, u32 m) { lg(T | 11); lg(r); lg(l); lg(m); lastlock = oalloc(0x100); memset((void *)(uintptr_t)lastlock, 0xcd, 0x100); return lastlock; }
static u32 __cdecl r_unlock(u32 r) { lg(T | 12); lg(r); lg(hashw((u8 *)(uintptr_t)lastlock, 0x100)); return r; }
static u32 __cdecl r_texcreate(u32 r) { u32 t = 0; lg(T | 13); lg(r); if (rnd() % 15) { t = oalloc(0x60); W(t + 0x50) = rnd(); } last_t = t; lg(t); return t; }
static u32 __cdecl r_setname(u32 t, const char *n) { lg(T | 14); lg(t); lg(hashw((const u8 *)n, strnlen(n, 32))); return t; }
static u32 __cdecl r_setmask(u32 t, const char *n) { lg(T | 15); lg(t); lg(hashw((const u8 *)n, strnlen(n, 32))); return t; }
static const struct { u32 va; void *fn; } STUB[] = {
  {0x4dc720, r_findchunk}, {0x4dbe70, r_read}, {0x4e3660, r_checkvalid}, {0x4d7950, r_rascreate}, {0x4e37c0, r_automip}, {0x4e3860, r_automipcube},
  {0x4e32c0, r_d3drascreate}, {0x4d7850, r_lockpal}, {0x4d77b0, r_unlockpal}, {0x4d77e0, r_rasdestroy}, {0x4d79f0, r_lock}, {0x4d7790, r_unlock},
  {0x4d5b90, r_texcreate}, {0x4d5c70, r_setname}, {0x4d5ce0, r_setmask}};
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

enum { GETSIZE, READ, NALL };
static const struct { const char *dll; u32 va; } F[NALL] = { {"rw37__rwD3D9NativeTextureGetSize", 0x4ed270}, {"rw37_rwD3D9NativeTextureRead", 0x4ed780} };
typedef u32 (__cdecl *G3)(u32, u32, u32);
static void *fn[2][NALL];
static long ncalls[NALL], nok[NALL];

static void put(const void *p, u32 n) { if (slen + n <= sizeof sbuf) { memcpy(sbuf + slen, p, n); slen += n; } }
static void put32(u32 v) { put(&v, 4); }
static void mkstream(void) {
  static const u32 rfs[] = { 0x0500, 0x0600, 0x8500, 0x9500, 0x1500, 0x2500, 0x4500, 0x0000, 0x9000, 0x8000, 0x0400, 0x0200 };
  static const u32 fmts[] = { 0x31545844, 0x33545844, 0x35545844, 21, 22, 25, 26, 0x29, 50 };
  u8 nat[0x48], rh[0x10]; u32 i, f, l, faces, levels;
  slen = spos = 0;
  for (i = 0; i < 0x48; i++) nat[i] = rnd();
  *(u32 *)nat = rnd() % 10 ? 9 : rnd() % 12;
  for (i = 8; i < 0x48; i++) if (rnd() % 9 == 0) nat[i] = 0;
  rh[0xe] = rnd() % 4 ? 4 : rnd(); rh[0xf] = rnd() % 4 == 0 ? rnd() : (rnd() % 2 ? 8 : 0) | (rnd() % 3 ? 0 : 2) | (rnd() % 5 ? 0 : 4) | (rnd() % 2);
  *(u32 *)rh = rfs[rnd() % 12] | (rnd() % 6 ? 0 : rnd() & 0x10000); *(u32 *)(rh + 4) = cur_fmt = fmts[rnd() % 9];
  *(u16 *)(rh + 8) = 1 << rnd() % 9; *(u16 *)(rh + 0xa) = 1 << rnd() % 9; rh[0xc] = rnd() % 3 ? 32 : 16; rh[0xd] = rnd() % 5;
  put(nat, 0x48); put(rh, 0x10);
  if (rnd() % 10 == 0) return;
  if (*(u32 *)rh & 0x6000) for (i = 0; i < 0x400 / 4; i++) put32(rnd());
  faces = rnd() % 3 ? 1 : 6; levels = rh[0xd] ? rh[0xd] : 1;
  for (f = 0; f < faces; f++) for (l = 0; l < levels; l++) {
    u32 sz = rnd() % 6 ? rnd() % 0x100 : rnd() % 0x40; put32(sz);
    for (i = 0; i < sz; i++) { u8 b = rnd(); put(&b, 1); }
  }
  if (rnd() % 8 == 0) slen -= rnd() % (slen - 0x58 + 1);      /* truncated */
}
static u32 mkgetsize(void) {
  u32 t = oalloc(0x60), r, e;
  if (rnd() % 10 == 0) return t;
  r = oalloc(EXT + 0x20); e = r + EXT; W(t) = r;
  W8(r + 0x23) = rnd() % 3 ? (rnd() % 2 ? 0x40 : 0x20) | (rnd() & 0x9f) : rnd();
  W8(e + 9) = rnd() % 3 ? 0 : rnd() % 2 ? 1 : rnd(); W8(e + 0xa) = rnd() % 2 ? 0 : rnd();
  W(e) = mktex((W8(e + 9) & 0xf) != 0);
  return t;
}
static void reset(u32 seed, unsigned short cw) {
  nlg = 0; rs = seed * 2654435761u + cw + 1; noa = 0; ntex = 0; last_r = last_t = 0; fakedev_hr = 0; slen = spos = 0;
}
static void run(int side, u32 seed, int steps, unsigned short cw) {
  void **f = fn[side]; int s, i;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 a0 = 0, a1 = 0, r, out = oalloc(4); int k = rnd() % 3 == 0 ? GETSIZE : READ;
    W(CAPS + 0x44) = rnd() % 3 ? 0x30000 : rnd() % 2 ? 0 : 0x10000;
    texhr = rnd() % 8 == 0 ? (int32_t)0x8876086c : 0;
    last_r = last_t = 0;
    if (k == GETSIZE) { a0 = out; a1 = mkgetsize(); } else { mkstream(); a0 = 0x57000000u + rnd() % 4; a1 = out; }
    lg(0x57e90000u + s); lg(k); lg(a0); lg(a1);
    cw_set(cw);
    r = ((G3)f[k])(a0, a1, 0); ncalls[k]++; if (r) nok[k]++;
    lg(r); lg(cw_now());
    cw_set(0x27f);
    lg(W(out)); lg(spos);
    if (last_r) { lg(W8(last_r + 0x21)); lg(W8(last_r + 0x23)); for (i = 0; i < 0x20; i += 4) lg(W(last_r + EXT + i)); }
    if (last_t) lg(W(last_t + 0x50));
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
  static void *table[18];
  static const u32 tv[18] = { DEVVAR, RASEXT, CAPS, 0x4dc720, 0x4dbe70, 0x4e3660, 0x4d7950, 0x4e37c0, 0x4e3860, 0x4e32c0,
    0x4d7850, 0x4d77b0, 0x4d77e0, 0x4d79f0, 0x4d7790, 0x4d5b90, 0x4d5c70, 0x4d5ce0 };
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  oa = malloc(ARENA);
  fakedev_log = sink; W(DEVVAR) = (u32)(uintptr_t)fakedev_init(); tex_vt_init();
  fakedev_vtbl[23] = (void *)d_createtex; fakedev_vtbl[25] = (void *)d_createcube;
  W(RASEXT) = EXT;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  /* self-test: the original GetSize on a 2D texture with 3 levels locks 3 levels */
  reset(1, 0x7f);
  { u32 t = oalloc(0x60), r = oalloc(EXT + 0x20), out = oalloc(4), k, n = 0; W(t) = r; W(r + EXT) = mktex(0); W(W(r + EXT) + 8) = 3;
    ((G3)(uintptr_t)F[GETSIZE].va)(out, t, 0);
    for (k = 0; k < nlg; k++) if (LG[k] == (T | 0x413)) n++;
    printf("selftest: original _rwD3D9NativeTextureGetSize locked %u levels, size %u (%s)\n", n, W(out), n == 3 ? "ok" : "WRONG");
    if (n != 3) { printf("selftest DIFF\n"); return 1; } }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9texdic_bind");
  if (!bind) { printf("mflrw_d3d9texdic_bind MISSING export\n"); return 1; }
  for (i = 0; i < 18; i++) table[i] = (void *)(uintptr_t)tv[i];
  if (bind(17, table) || !bind(18, table)) { printf("mflrw_d3d9texdic_bind count check wrong (DIFF)\n"); return 1; }
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
  for (i = 0; i < NALL; i++) printf("%-45s %9ld calls (both sides), %ld returned TRUE\n", F[i].dll, ncalls[i], nok[i]);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
