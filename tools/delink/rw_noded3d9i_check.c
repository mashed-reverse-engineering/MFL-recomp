/* rw_noded3d9i_check.c: bit-exact check of the librw DLL's noded3d9i exports (group noded3d9i, librw-mfl capi/noded3d9i.cpp) against the ORIGINAL code of MFL.exe, both
   run natively (Wine).
   usage: rw_noded3d9i_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [ws|at] [cw:<hex>] [va:byte ...]   (build / run: tools/delink/rw_noded3d9i_check.sh)
   Rows: see F[] below.  No device: the vertex buffers and the vertex declaration are fake COM objects of this file (buffer Lock 11 / Unlock 12, declaration
   GetDeclaration 4, every other slot = TRAP + exit: an unfilled slot never passes silently).  Every exe callee of the rows except the CRT qsort and the element sort
   compare 0x500310 (both run as original code on both sides) is a logging stand-in: usage flag getters, declaration / buffer create and destroy, dynamic geometry
   list, the six element writers.  The engine instance, world, sector, atomic, geometry, morph targets and resource entry header are blocks of the check's arena; the
   three device caps fields the rows read (DevCaps, DevCaps2, DeclTypes) are preset per call.  The atomic row runs at x87 control word 0x7f, 0x27f or 0x37f, chosen
   per call (cw:<hex> forces one).  "rev" runs the original first; ws / at restrict the steps to one row.  va:byte patches the ORIGINAL's code first (mutation test:
   must give DIFF).  mut@<file> runs a whole list of mutations in one process (see run_mutlist; driver: agents/k1_w6/mutbatch.py).
   Exit 1 on DIFF / MISSING / TRAP / TIMEOUT / CRASH. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u32; typedef uint8_t u8; typedef uint16_t u16;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
#define B(va) (*(volatile u8 *)(uintptr_t)(va))
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
#define CAPS 0x947600u
static unsigned char eng[0x1000] __attribute__((aligned(16)));
#define EW(off) (*(u32 *)(eng + (off)))
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
#define ARENA (1u << 22)
static u8 *arena; static u32 ap;
static void *amalloc(u32 n) {
  u8 *p; u32 sz = (n + 16 + 15) & ~15u; if (ap + sz > ARENA) { printf("arena full\n"); ExitProcess(5); }
  p = arena + ap; memset(p, 0xcd, sz); ap += sz; return p;
}
static u8 *rblock(u32 n) { u8 *p = amalloc(n); u32 k; for (k = 0; k < n; k++) p[k] = (u8)rnd(); return p; }
#define P(p) ((u32)(uintptr_t)(p))

/* fake vertex buffers and vertex declaration */
struct HBuf { void **vtbl; u32 id; };
struct HDecl { void **vtbl; u32 n; u8 el[8 * 24]; };
static void *hb_vtbl[16], *hd_vtbl[16];
static void hb_trap(int n) { printf("TRAP unexpected buffer / declaration method %d\n", n); fflush(stdout); ExitProcess(5); }
#define HT(n) static long __stdcall ht_##n(void *self) { hb_trap(n); return 0; }
HT(0) HT(1) HT(2) HT(3) HT(4) HT(5) HT(6) HT(7) HT(8) HT(9) HT(10) HT(11) HT(12) HT(13) HT(14) HT(15)
static int lock_may_fail;      /* atomic row only: the world sector row's pointer slot is not preset, a Lock that leaves it alone would compare stack garbage */
static long __stdcall hb_lock(struct HBuf *b, u32 off, u32 size, u32 *pp, u32 flags) {
  lg(0xb10c); lg(b->id); lg(off); lg(size); lg(flags);
  if (lock_may_fail && hrnd() % 8 == 0) { lg(0); return (long)0x8876086c; }
  *pp = 0x20000000u + (b->id & 0xfff) * 0x10000u + off; return 0;
}
static long __stdcall hb_unlock(struct HBuf *b) { lg(0xb10d); lg(b->id); return 0; }
static long __stdcall hd_get(struct HDecl *d, u8 *el, u32 *num) { lg(0xdec1); lg(d->n); memcpy(el, d->el, 8 * d->n); *num = d->n; return 0; }
static void init_vtbls(void) {
  static long (__stdcall *const t[16])(void *) = { ht_0, ht_1, ht_2, ht_3, ht_4, ht_5, ht_6, ht_7, ht_8, ht_9, ht_10, ht_11, ht_12, ht_13, ht_14, ht_15 };
  int i; for (i = 0; i < 16; i++) { hb_vtbl[i] = (void *)t[i]; hd_vtbl[i] = (void *)t[i]; }
  hb_vtbl[11] = (void *)hb_lock; hb_vtbl[12] = (void *)hb_unlock; hd_vtbl[4] = (void *)hd_get;
}
static u32 nbuf;
static struct HBuf *mkbuf(void) { struct HBuf *b = amalloc(sizeof *b); b->vtbl = hb_vtbl; b->id = 0xb0f00000u + nbuf++; return b; }

/* stand-ins (jmp patched over the exe functions; the DLL reaches the same addresses through its bind table) */
static u32 __cdecl h_wsusage(u32 sector) { u32 r = (hrnd() % 2 ? 0x10000 : 0) | (hrnd() % 4 == 0 ? hrnd() & ~0x10000u : 0); lg(0xa509580); lg(sector); lg(r); return r; }
static u32 __cdecl h_geomusage(u32 geom) {
  static const u32 t[] = {0, 0, 1, 2, 4, 8, 0x10, 0x20, 6, 0xe, 0xff0, 0xff2, 0xfff, 0x1000, 0x800, 0x12, 0x30};
  u32 r = (hrnd() % 4 == 0 ? hrnd() & 0xfff : t[hrnd() % 17]) | (hrnd() % 2 ? 0x10000 : 0) | (hrnd() % 6 == 0 ? hrnd() & 0xfffef000u : 0);
  lg(0xa509550); lg(geom); lg(r); return r;
}
static u32 __cdecl h_createdecl(u8 *el, u32 *pout) {
  struct HDecl *d = amalloc(sizeof *d); u32 n = 0, r = hrnd() % 8 != 0;
  lg(0xa4db9e0); lg(P(pout));
  for (;;) { lg(*(u32 *)(el + 8 * n)); lg(*(u32 *)(el + 8 * n + 4)); n++; if (*(u16 *)(el + 8 * n - 8) == 0xff || n == 24) break; }
  d->vtbl = hd_vtbl; d->n = n; memset(d->el, 0, sizeof d->el); memcpy(d->el, el, 8 * n); *pout = P(d); lg(r); return r;
}
static u32 __cdecl h_createvb(u32 stride, u32 size, u32 *pvb, u32 *poff) {
  u32 r = hrnd() % 6 != 0; lg(0xa4ec6d0); lg(stride); lg(size); lg(P(pvb)); lg(P(poff)); lg(r);
  if (r) { *pvb = P(mkbuf()); *poff = hrnd() % 3 ? 0 : hrnd() % 4096; lg(*poff); }
  return r;
}
static void __cdecl h_destroyvb(u32 stride, u32 size, u32 vb, u32 off) { lg(0xa4ec870); lg(stride); lg(size); lg(vb); lg(off); }
static u32 __cdecl h_dyncreate(u32 size, u32 *pvb) { u32 r = hrnd() % 6 != 0; lg(0xa4ecbc0); lg(size); lg(P(pvb)); lg(r); if (r) *pvb = P(mkbuf()); return r; }
static void __cdecl h_dyndestroy(u32 vb) { lg(0xa4ecca0); lg(vb); }
static void __cdecl h_adddyn(u32 geom) { lg(0xa504430); lg(geom); }
static u32 __cdecl h_v3d(u32 type, u32 mem, u32 src, u32 n, u32 stride) { u32 r = hrnd() % 32; lg(0xa5063f0); lg(type); lg(mem); lg(src); lg(n); lg(stride); return r; }
static u32 __cdecl h_v3dcomp(u32 type, u32 mem, u32 src, u32 n, u32 stride) { u32 r = hrnd() % 32; lg(0xa506ad0); lg(type); lg(mem); lg(src); lg(n); lg(stride); return r; }
static u32 __cdecl h_morph(u32 type, u32 mem, u32 a, u32 b, u32 scale, u32 n, u32 stride) {
  u32 r = hrnd() % 32; lg(0xa507480); lg(type); lg(mem); lg(a); lg(b); lg(scale); lg(n); lg(stride); return r;
}
static u32 __cdecl h_v2d(u32 type, u32 mem, u32 src, u32 n, u32 stride) { u32 r = hrnd() % 32; lg(0xa507e00); lg(type); lg(mem); lg(src); lg(n); lg(stride); return r; }
static u32 __cdecl h_color(u32 mem, u32 src, u32 n, u32 stride) { u32 r = hrnd(); lg(0xa508400); lg(mem); lg(src); lg(n); lg(stride); return r; }
static u32 __cdecl h_tangent(u32 type, u32 mem, u32 a, u32 b, u32 c, u32 tex, u32 hdr, u32 stride) {
  u32 r = hrnd(); lg(0xa508600); lg(type); lg(mem); lg(a); lg(b); lg(c); lg(tex); lg(hdr); lg(stride); return r;
}

enum { WS, AT, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_D3D9WorldSectorDefaultInstanceCallback", 0x5005b0}, {"rw37_D3D9AtomicDefaultInstanceCallback", 0x4ff980}};
typedef u32 (__cdecl *G3)(u32, u32, u32);
static void *fn[2][NF]; static long ncalls[NF], nmorph, nre; static int only = -1; static u32 forced_cw;

static void reset(u32 seed) {
  nlg = 0; ap = 0; nbuf = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u; memset(arena, 0, ARENA); memset(eng, 0, sizeof eng);
  W(ENGVAR) = P(eng); W(CAPS + 0x1c) = 0; W(CAPS + 0xd4) = 0; W(CAPS + 0xec) = 0;
}
static void mkcaps(void) { W(CAPS + 0x1c) = (rnd() & ~0x10000u) | (rnd() % 4 ? 0x10000 : 0); B(CAPS + 0xd4) = (u8)rnd(); B(CAPS + 0xec) = (u8)rnd(); }
static void setcw(u32 cw) { __asm__ volatile("fldcw %0" :: "m"(cw)); }
static u32 sbits(void) {      /* interpolator floats: products that are 0, underflow to 0, denormal, nan, or ordinary */
  static const u32 v[] = {0, 0x80000000u, 0x3f800000, 0x3f000000, 0x0da24260 /* 1e-30 */, 0x1e3ce508 /* 1e-20 */, 1, 0x7fc00000, 0x7f800000, 0x40400000, 0x3dcccccd, 0x3eaaaaab,
                          0x00800000, 0x3f800001, 0xbf800000};
  return rnd() % 8 == 0 ? rnd() : v[rnd() % 15];
}
/* a declaration for the reinstance path: every element the row may look for (position, normal, colour, texture sets 0..7, tangent) plus decoys with another usage
   index, in random order, streams 0 / 1 */
static struct HDecl *mkdecl(void) {
  static const u8 us[17][2] = {{0, 0}, {3, 0}, {0xa, 0}, {6, 0}, {5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, {5, 7}, {0, 1}, {3, 1}, {0xa, 1}, {6, 1}, {5, 8}};
  struct HDecl *d = amalloc(sizeof *d); u32 i, j, one = rnd() % 3 == 0, ord[17];
  for (i = 0; i < 17; i++) ord[i] = i;
  for (i = 16; i > 0; i--) { j = rnd() % (i + 1); { u32 t = ord[i]; ord[i] = ord[j]; ord[j] = t; } }
  memset(d->el, 0, sizeof d->el); d->vtbl = hd_vtbl; d->n = 18;
  for (i = 0; i < 17; i++) {
    u8 *e = d->el + 8 * i; *(u16 *)e = (u16)(one ? 0 : rnd() % 2); *(u16 *)(e + 2) = (u16)(rnd() % 64); e[4] = (u8)(rnd() % 17); e[5] = (u8)rnd(); e[6] = us[ord[i]][0]; e[7] = us[ord[i]][1];
  }
  *(u16 *)(d->el + 8 * 17) = 0xff; d->el[8 * 17 + 4] = 0x11;
  return d;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = only >= 0 ? (u32)only : rnd() % 3 ? AT : WS, k, r = 0, cw;
    lg(0x57e90000u + s); lg(op);
    if (op == WS) {      /* D3D9WorldSectorDefaultInstanceCallback */
      u32 nm = 1 + rnd() % 4; u8 *sector = rblock(0x90), *hdr = rblock(0x40 + nm * 0x24), *w = rblock(0x60);
      *(u32 *)(hdr + 4) = nm; *(u32 *)(hdr + 8) = nm + 1 /* not read by the rows: a mutated mesh count offset then walks one mesh too far instead of crashing */; *(u16 *)(sector + 0x82) = (u16)(rnd() % 4 ? rnd() % 300 : rnd());
      for (k = 0; k < nm; k++) { *(u32 *)(hdr + 0x44 + k * 0x24) = rnd() % 500; *(u32 *)(hdr + 0x58 + k * 0x24) = rnd() % 500; }
      EW(4) = P(w); mkcaps(); lock_may_fail = 0;
      r = ((G3)f[WS])(P(sector), P(hdr), rnd()); ncalls[WS]++; lg(r);
    } else {      /* D3D9AtomicDefaultInstanceCallback */
      static const u32 lk[] = {0, 0xfff, 2, 4, 8, 0x10, 0x20, 6, 0xff0, 0xff2, 0x1000, 0xf000, 1};      /* 1: no element, so mask mutations that add bit 0 show */
      u32 nm = 1 + rnd() % 4, re = rnd() % 5 < 2, nmt = rnd() % 3 ? 1 : rnd() % 4 ? 2 + rnd() % 2 : rnd() % 2 ? 0 : (u32)-1, numTex = rnd() % 4 ? rnd() % 3 : rnd() % 9, i;
      u8 *atomic = rblock(0x80), *geom = rblock(0x70), *hdr = rblock(0x40 + nm * 0x24), *mts = rblock(8 * 0x1c) + 2 * 0x1c, *pipe = rblock(0x40);
      *(u32 *)(atomic + 0x18) = P(geom); *(u16 *)(atomic + 0x50) = (u16)(rnd() % 5 - 1); *(u16 *)(atomic + 0x52) = (u16)(rnd() % 5 - 1);
      *(u32 *)(atomic + 0x58) = sbits(); *(u32 *)(atomic + 0x5c) = sbits();
      *(u32 *)(pipe + 0x2c) = rnd() % 2 ? 0x12e : rnd() % 2 ? 0x12f : rnd(); *(u32 *)(atomic + 0x6c) = rnd() % 3 == 0 ? 0 : P(pipe);
      *(u16 *)(geom + 0xc) = (u16)(rnd() % 4 == 0 ? rnd() : lk[rnd() % 13]); *(u32 *)(geom + 0x14) = rnd() % 200; *(u32 *)(geom + 0x18) = nmt; *(u32 *)(geom + 0x1c) = numTex; *(u32 *)(geom + 0x20) = numTex + 1;      /* +0x20 is not read by the row: target of the count offset mutation */
      *(u32 *)(geom + 0x5c) = P(mts); *(u32 *)(hdr + 4) = nm; *(u32 *)(hdr + 8) = nm + 1;
      for (k = 0; k < nm; k++) { *(u32 *)(hdr + 0x44 + k * 0x24) = rnd() % 500; *(u32 *)(hdr + 0x58 + k * 0x24) = rnd() % 500; }
      for (i = 0; i < 2; i++) {
        u8 *st = hdr + 0x10 + i * 0x10;
        *(u32 *)st = re || rnd() % 3 ? P(mkbuf()) : 0; *(u32 *)(st + 4) = rnd() % 2 ? 0 : rnd() % 64; *(u32 *)(st + 8) = rnd() % 40;
        *(u16 *)(st + 0xc) = (u16)(rnd() % 4 == 0 ? rnd() & 0xfff : lk[rnd() % 13]); st[0xe] = (u8)(rnd() % 2); st[0xf] = (u8)(rnd() % 3 == 0);
      }
      if (re) { *(u32 *)(hdr + 0x34) = P(mkdecl()); nre++; }
      mkcaps(); lock_may_fail = 1;
      setcw(forced_cw ? forced_cw : rnd() % 3 == 0 ? 0x7f : rnd() % 2 ? 0x27f : 0x37f);
      r = ((G3)f[AT])(P(atomic), P(hdr), re ? 1 + rnd() % 3 : 0); ncalls[AT]++;
      __asm__ volatile("fnstcw %0" : "=m"(cw)); lg(cw & 0xffff); setcw(0x27f);
      lg(r);
    }
  }
}
static u32 *save_log, nsave; static unsigned char *save_arena, save_eng[sizeof eng];
static int quiet;      /* mutation list mode: only the verdict lines */
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  if (quiet) return nlg != nsave || memcmp(save_log, LG, nlg * 4) || memcmp(save_eng, eng, sizeof eng) || memcmp(save_arena, arena, ARENA);
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
static volatile u32 cur_seed, deadline; static char cur_mut[256];
/* a verdict line in one write (stdout is unbuffered: printf writes in pieces, and the DLL's exit message once landed inside a MUT line) */
static void mutsay(const char *verdict) { char b[400]; int n = snprintf(b, sizeof b, "\nMUT %s: %s\n", cur_mut, verdict); fwrite(b, 1, n, stdout); }
static DWORD WINAPI watchdog(LPVOID p) {
  if (!deadline) deadline = GetTickCount() + (DWORD)(uintptr_t)p;
  while ((int32_t)(deadline - GetTickCount()) > 0) Sleep(100);
  if (cur_mut[0]) mutsay("TIMEOUT");
  printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  if (cur_mut[0]) { char v[64]; snprintf(v, sizeof v, "CRASH %08lx at %p", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress); mutsay(v); }
  printf("CRASH %08lx at %p seed %u; eax %08lx ecx %08lx edx %08lx esi %08lx edi %08lx\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed,
         e->ContextRecord->Eax, e->ContextRecord->Ecx, e->ContextRecord->Edx, e->ContextRecord->Esi, e->ContextRecord->Edi);
  fflush(stdout); ExitProcess(4); return 0; }
static long tot;
/* seeds 1..n, both sides, compared; stops after maxbad differing seeds; returns their number */
static int run_seeds(int seeds, int steps, int maxbad) {
  u32 seed; int bad = 0;
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4); memcpy(save_arena, arena, ARENA); memcpy(save_eng, eng, sizeof eng);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= maxbad) break;
  }
  return bad;
}
/* mutation list mode (mut@<file>, lines `va:byte [va:byte ...]  # comment`): per line the original is patched, the seeds run until the first difference, the bytes are
   restored and one line `MUT <spec>: DIFF|EQUAL` is printed (CRASH / TIMEOUT lines come from the handlers and end the process: the caller starts again behind that
   line).  After every mutation 3 unpatched seeds must be EQUAL, else the process state is not trusted any more: MUTSTOP, exit, the caller starts again. */
static int run_mutlist(const char *path, int seeds, int steps) {
  FILE *f = fopen(path, "r"); char line[512];
  if (!f) { printf("cannot open mutation list %s\n", path); return 2; }
  quiet = 1;
  while (fgets(line, sizeof line, f)) {
    u32 va[8], by[8], old[8], n = 0, k; char *p = strchr(line, '#'), *q; int bad;
    if (p) *p = 0;
    for (q = strtok(line, " \t\r\n"); q && n < 8; q = strtok(0, " \t\r\n")) if (sscanf(q, "%x:%x", &va[n], &by[n]) == 2) n++;
    if (!n) continue;
    cur_mut[0] = 0; for (k = 0; k < n; k++) sprintf(cur_mut + strlen(cur_mut), "%s%x:%02x", k ? " " : "", va[k], by[k]);
    for (k = 0; k < n; k++) { old[k] = B(va[k]); B(va[k]) = (u8)by[k]; }
    deadline = GetTickCount() + 120000;
    bad = run_seeds(seeds, steps, 1);
    for (k = 0; k < n; k++) B(va[k]) = (u8)old[k];
    mutsay(bad ? "DIFF" : "EQUAL");
    deadline = GetTickCount() + 120000;
    if (run_seeds(3, steps, 1)) { printf("MUTSTOP: unpatched seeds differ after %s\n", cur_mut); return 7; }
    cur_mut[0] = 0;
  }
  printf("MUTDONE\n");
  return 0;
}

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; const char *mutlist = 0;
  void (__cdecl *bind)(void *, void *); static void *tbl[17];
  /* table order = enum E of noded3d9i.cpp */
  static const u32 t[17] = {0x509580, 0x509550, CAPS, 0x4db9e0, 0x4ec6d0, 0x4ec870, 0x4ecbc0, 0x4ecca0, 0x504430, 0x5d38b0, 0x500310,
                            0x5063f0, 0x506ad0, 0x507480, 0x507e00, 0x508400, 0x508600};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 400u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); save_arena = malloc(ARENA); init_vtbls();
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (!strcmp(argv[i], "ws")) { only = WS; printf("rows: world sector only\n"); }
    else if (!strcmp(argv[i], "at")) { only = AT; printf("rows: atomic only\n"); }
    else if (!strncmp(argv[i], "mut@", 4)) mutlist = argv[i] + 4;
    else if (!strncmp(argv[i], "cw:", 3)) { forced_cw = strtoul(argv[i] + 3, 0, 16); printf("x87 control word forced to %x\n", forced_cw); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_noded3d9i_bind");
  if (!bind) { printf("mflrw_noded3d9i_bind MISSING export\n"); return 1; }
  jmp_patch(0x509580, (void *)h_wsusage); jmp_patch(0x509550, (void *)h_geomusage); jmp_patch(0x4db9e0, (void *)h_createdecl); jmp_patch(0x4ec6d0, (void *)h_createvb);
  jmp_patch(0x4ec870, (void *)h_destroyvb); jmp_patch(0x4ecbc0, (void *)h_dyncreate); jmp_patch(0x4ecca0, (void *)h_dyndestroy); jmp_patch(0x504430, (void *)h_adddyn);
  jmp_patch(0x5063f0, (void *)h_v3d); jmp_patch(0x506ad0, (void *)h_v3dcomp); jmp_patch(0x507480, (void *)h_morph); jmp_patch(0x507e00, (void *)h_v2d);
  jmp_patch(0x508400, (void *)h_color); jmp_patch(0x508600, (void *)h_tangent);
  for (i = 0; i < 17; i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind((void *)ENGVAR, tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  if (mutlist) return run_mutlist(mutlist, seeds, steps);
  bad = run_seeds(seeds, steps, 5);
  for (i = 0; i < NF; i++) printf("%-44s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  printf("atomic reinstance calls %ld (both sides)\n", nre);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
