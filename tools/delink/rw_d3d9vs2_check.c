/* rw_d3d9vs2_check.c: bit-exact check of the librw DLL's d3d9vs2 exports (group d3d9vs2, librw-mfl capi/d3d9vs2.cpp) against the ORIGINAL code of MFL.exe, both run
   natively (Wine).
   usage: rw_d3d9vs2_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=300] [rev] [va:byte ...]   (build / run: tools/delink/rw_d3d9vs2_check.sh)
   Rows: rxD3D9VertexShaderDefaultMeshRenderCallBack 0x50b610, rxD3D9VertexShaderDefaultLightingCallBack 0x50b750.
   Stand-ins (logging, same both sides; values from a per-seed hidden stream): RwD3D9SetTexture 0x4e55d0, RwD3D9SetTextureStageState 0x4e3c70,
   _rwD3D9RenderStateFlushCache 0x4e3b30, RwFrameGetLTM 0x4d0ff0 (random matrices), RpAtomicGetWorldBoundingSphere 0x4f6240 (random sphere near the lights),
   RpLightGetConeAngle 0x4f4a70 (returns an 80-bit value in st0, also near acos 0.999), the three _rwD3D9VSGet*InLocalSpace rows 0x50c1a0 / 0x50c2e0 / 0x50c3d0,
   and a fake device (vtbl +0x1ac SetPixelShader, +0x170 SetVertexShader, +0x148 DrawIndexedPrimitive, +0x144 DrawPrimitive; every other slot 0 = crash).
   Per call the fixture rebuilds a random world (directional / ambient light list), atomic (geometry flags, sector ties) or sector with local light lists from a
   48-light pool: types 1 / 2 / 0x80 / 0x81 / 0x82 and others, flag bits, light frame words equal to the next engine frame or not, a light in several sectors, more
   than 15 lights of a kind.  Logged: every stand-in's arguments, the return value, the descriptor, the written constants (up to the return pointer + 2 vec4) and a
   hash of the whole 160-vec4 buffer, all light frame words, the engine frame word, the last-shader globals.  Each call runs at x87 control word 0x7f, 0x27f or
   0x37f.  "rev" = original first.  va:byte patches the ORIGINAL (must DIFF).  Exit 1 on DIFF / MISSING / TIMEOUT / CRASH. */
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

#define LASTVS 0x62e768u
#define LASTPS 0x62e76cu
#define DEVICE 0x7d2240u
#define ENGVAR 0x7d2128u
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static u32 rs, hr;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hrnd(void) { hr ^= hr << 13; hr ^= hr >> 17; hr ^= hr << 5; return hr; }
static void setcw(u32 cw) { __asm__ volatile("fninit; fldcw %0" :: "m"(cw)); }
static u32 pickcw(void) { static const u32 c[3] = {0x7f, 0x27f, 0x37f}; return c[rnd() % 3]; }
#define P(p) ((u32)(uintptr_t)(p))
static float fr(u32 r, float lo, float hi) { return lo + (hi - lo) * (float)(r & 0xffffff) / 16777216.0f; }
static u32 fbits(float f) { u32 u; memcpy(&u, &f, 4); return u; }

/* ---- arena (same addresses both sides) ---- */
#define NL 48
static u32 light[NL][16], wld[0x20], geo[8], atom[0x20], tie[4][4], sec[4][0x20], node[96][4], eng[0x10];
static u32 out[160 * 4], descw, mats[8][16], sph[4];
static u32 hdr[8], inst[12], mat[2], vtbl[0x80], devobj[2];
static void list_init(u32 *head) { head[0] = head[1] = P(head); }
static void list_add(u32 *head, u32 *link) { link[0] = head[0]; link[1] = P(head); ((u32 *)(uintptr_t)head[0])[1] = P(link); head[0] = P(link); }
static void build(int asAtomic) {
  int i, k, nn = 0;
  static const u8 types[] = {1, 1, 1, 2, 2, 0x80, 0x80, 0x80, 0x81, 0x81, 0x82, 0x82, 0, 3, 0x7f, 0x83};
  u16 next = (u16)(((u16 *)eng)[5] + 1);
  int many = rnd() % 5 == 0;      /* fixture round 2: dense mode, so the 15-light caps of all three lists are reached */
  for (i = 0; i < NL; i++) {
    u32 *l = light[i], r = rnd();
    for (k = 0; k < 16; k++) l[k] = rnd();
    ((u8 *)l)[1] = many ? types[r % 2 ? 0 : 5 + (r >> 4) % 7] : types[r % 16]; ((u8 *)l)[2] = many ? 3 : rnd() % 5 ? (u8)(rnd() & 3) : (u8)rnd();
    l[5] = fbits(many ? 40.0f : rnd() % 16 ? fr(rnd(), 0.0f, 12.0f) : rnd() % 2 ? 0.0f : fr(rnd(), -3.0f, 1e6f));
    l[6] = fbits(fr(rnd(), -1.0f, 2.0f)); l[7] = fbits(fr(rnd(), -1.0f, 2.0f)); l[8] = fbits(fr(rnd(), -1.0f, 2.0f));
    ((u16 *)l)[0x1e] = rnd() % 6 == 0 ? next : (u16)rnd();
  }
  list_init(wld + 0xf);      /* world + 0x3c; link at light + 0x34; a light goes in at most once */
  { u8 in[NL] = {0}; for (k = many ? 200 : rnd() % 4 == 0 ? 20 + rnd() % 25 : rnd() % 10; k > 0; k--) { i = rnd() % NL; if (!in[i]) { in[i] = 1; list_add(wld + 0xf, light[i] + 0xd); } } }
  for (i = 0; i < 4; i++) {
    list_init(sec[i] + 0x10);      /* sector + 0x40 */
    for (k = many ? 24 : rnd() % 4 == 0 ? 10 + rnd() % 30 : rnd() % 8; k > 0 && nn < 96; k--, nn++) {
      node[nn][2] = many ? P(light[(i * 24 + k) % NL]) : rnd() % 16 ? P(light[rnd() % NL]) : 0; list_add(sec[i] + 0x10, node[nn]);
    }
  }
  geo[2] = many ? rnd() | 0x30 : rnd() % 4 ? rnd() | 0x20 : rnd();
  wld[2] = many ? rnd() | 0x20 : rnd() % 4 ? rnd() | 0x20 : rnd();
  list_init(atom + 0x19);      /* atomic + 0x64 */
  for (k = many ? 4 : rnd() % 4 + (rnd() % 5 == 0 ? 0 : 1); k > 1; k--) { tie[k - 1][2] = P(sec[rnd() % 4]); list_add(atom + 0x19, tie[k - 1]); }
  atom[6] = P(geo);
  eng[1] = rnd() % 10 || !asAtomic ? P(wld) : 0;      /* no current world: only the atomic path survives it (a sector reads the world's flags) */
}

/* ---- stand-ins (same both sides) ---- */
static u32 __cdecl h_settex(u32 tex, u32 stage) { lg(0xa4e55d0); lg(tex); lg(stage); return hrnd(); }
static u32 __cdecl h_settss(u32 stage, u32 type, u32 value) { lg(0xa4e3c70); lg(stage); lg(type); lg(value); return hrnd(); }
static u32 __cdecl h_flush(void) { lg(0xa4e3b30); return hrnd(); }
static u32 __stdcall d_setps(u32 d, u32 a) { u32 r = hrnd(); lg(0xd1ac); lg(d); lg(a); lg(W(LASTPS)); lg(r); return r; }
static u32 __stdcall d_setvs(u32 d, u32 a) { u32 r = hrnd(); lg(0xd170); lg(d); lg(a); lg(W(LASTVS)); lg(r); return r; }
static u32 __stdcall d_dp(u32 d, u32 a, u32 b, u32 c) { u32 r = hrnd(); lg(0xd144); lg(d); lg(a); lg(b); lg(c); lg(r); return r; }
static u32 __stdcall d_dip(u32 d, u32 a, u32 b, u32 c, u32 e, u32 f, u32 g) { u32 r = hrnd(); lg(0xd148); lg(d); lg(a); lg(b); lg(c); lg(e); lg(f); lg(g); lg(r); return r; }
static u32 nmat;
static u32 __cdecl h_ltm(u32 frame) {
  u32 *m = mats[nmat++ & 7], k;
  for (k = 0; k < 16; k++) m[k] = fbits(fr(hrnd(), -1.0f, 1.0f));
  m[12] = fbits(fr(hrnd(), -10.0f, 10.0f)); m[13] = fbits(fr(hrnd(), -10.0f, 10.0f)); m[14] = fbits(fr(hrnd(), -10.0f, 10.0f));
  lg(0xa4d0ff0); lg(frame); lg(P(m)); return P(m);
}
/* fixture round 2: a callee may leave the sticky x87 invalid flag (IE, status bit 0) set; the original's `and eax, 0x4100` / `0x100` status tests must ignore it */
static void sticky_ie(void) { if (hrnd() % 8 == 0) __asm__ volatile("fldz\n\tfdiv %%st(0), %%st\n\tfstp %%st(0)" ::: "st"); }
static u32 __cdecl h_bsphere(u32 atomic) {
  sticky_ie();
  sph[0] = fbits(fr(hrnd(), -10.0f, 10.0f)); sph[1] = fbits(fr(hrnd(), -10.0f, 10.0f)); sph[2] = fbits(fr(hrnd(), -10.0f, 10.0f));
  sph[3] = fbits(hrnd() % 8 ? fr(hrnd(), 0.0f, 10.0f) : 0.0f);
  lg(0xa4f6240); lg(atomic); return P(sph);
}
static long double __cdecl h_cone(u32 l) {
  u32 r = hrnd(); long double a;
  sticky_ie();
  if (r % 4 == 0) a = 0.0447176L + (long double)fr(hrnd(), -0.002f, 0.002f) * 1.0000000123L;      /* around acos 0.999 */
  else if (r % 4 == 1) a = (long double)fr(hrnd(), 0.0f, 1.6f) * 1.0000000123L;                     /* extra bits below float precision */
  else a = r % 16 == 2 ? 0.0L : (long double)fr(hrnd(), -3.2f, 3.2f);
  lg(0xa4f4a70); lg(l); return a;
}
static void vec3(u32 o) { u32 *p = (u32 *)(uintptr_t)o; p[0] = fbits(fr(hrnd(), -1.0f, 1.0f)); p[1] = fbits(fr(hrnd(), -1.0f, 1.0f)); p[2] = fbits(fr(hrnd(), -1.0f, 1.0f)); }
static void __cdecl h_vsnormal(u32 in, u32 o) { lg(0xa50c1a0); lg(in); lg(o); vec3(o); }
static void __cdecl h_vspoint(u32 in, u32 o) { lg(0xa50c2e0); lg(in); lg(o); vec3(o); }
static void __cdecl h_vsradius(float r, u32 o) {
  u32 k = hrnd(); lg(0xa50c3d0); lg(fbits(r)); lg(o);
  *(float *)(uintptr_t)o = k % 16 == 0 ? 0.0f : k % 16 == 1 ? -fr(hrnd(), 0.0f, 5.0f) : fr(hrnd(), 0.01f, 20.0f);
}

enum { MESH, LIGHT, NF };
static const struct { const char *dll; u32 va; } F[NF] = {{"rw37_rxD3D9VertexShaderDefaultMeshRenderCallBack", 0x50b610},
  {"rw37_rxD3D9VertexShaderDefaultLightingCallBack", 0x50b750}};
static void *fn[2][NF]; static long ncalls[NF];

static void reset(u32 seed) {
  setcw(0x37f);      /* fixture round 1: inputs are built under the previous call's CW; without this the first step of side 1 inherited side 0's last CW */
  nlg = 0; nmat = 0; rs = seed * 2654435761u + 1; hr = seed * 40503u + 12345u;
  memset(eng, 0, sizeof eng); ((u16 *)eng)[5] = (u16)(seed % 7 == 0 ? 0xfff0 + seed % 16 : seed * 37);
  W(ENGVAR) = P(eng); W(DEVICE) = P(devobj); devobj[0] = P(vtbl); devobj[1] = 0xdef1ce;
  W(LASTPS) = 0; W(LASTVS) = 0;
}
static void run(int side, u32 seed, int steps) {
  void **f = fn[side]; int s, i;
  reset(seed);
  for (s = 0; s < steps; s++) {
    u32 op = rnd() % 5 == 0 ? MESH : LIGHT, ret;
    lg(0x57e90000u + s); lg(op);
    if (op == MESH) {
      for (i = 0; i < 8; i++) hdr[i] = rnd();
      for (i = 0; i < 12; i++) inst[i] = rnd();
      hdr[2] = rnd() % 2 ? 0 : rnd(); mat[0] = rnd() % 3 ? rnd() : 0; inst[2] = P(mat);
      if (rnd() % 2) W(LASTPS) = rnd() % 2 ? rnd() : 0;
      if (rnd() % 3 == 0) inst[4] = W(LASTVS);
      setcw(pickcw());
      ret = ((u32 (__cdecl *)(u32, u32))f[MESH])(P(hdr), P(inst));
      lg(ret); lg(W(LASTPS)); lg(W(LASTVS));
    } else {
      u32 type = rnd() % 5 ? 1 : rnd() % 2 ? 0 : rnd(), obj, n, h = 2166136261u;
      build(type == 1);
      for (i = 0; i < 160 * 4; i++) out[i] = rnd();
      descw = rnd();
      obj = type == 1 ? P(atom) : P(sec[rnd() % 4]);
      setcw(pickcw());
      ret = ((u32 (__cdecl *)(u32, u32, u32, u32))f[LIGHT])(obj, type, P(out), P(&descw));
      lg(ret - P(out)); lg(descw); lg(((u16 *)eng)[5]);
      n = ret >= P(out) && ret - P(out) < sizeof out - 32 ? (ret - P(out)) / 4 + 8 : 0;
      for (i = 0; i < (int)n; i++) lg(out[i]);
      for (i = 0; i < 160 * 4; i++) h = (h ^ out[i]) * 16777619u;
      lg(h);
      for (i = 0; i < NL; i++) lg(((u16 *)light[i])[0x1e]);
    }
    ncalls[op]++;
  }
}
static u32 *save_log, nsave;
static int compare(u32 seed) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0, op = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; op = LG[k + 1]; break; }
    printf("  seed %u: call log differs at entry %u (step %u op %u): first %08x second %08x (lengths %u / %u)\n", seed, i, st, op, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    { u32 j; printf("    context (first/second):"); for (j = i > 12 ? i - 12 : 0; j < i + 3 && j < n; j++) printf(" %08x/%08x", save_log[j], LG[j]); printf("\n"); }
    return 1;
  }
  return 0;
}
static volatile u32 cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %u\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) {
  printf("CRASH %08lx at %p seed %u\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 300, i, bad = 0, rev = 0; u32 seed; double tot = 0;
  void (__cdecl *bind)(void *); static void *tbl[16];
  /* table order = d3d9vs2.cpp enum V */
  static const u32 t[] = {LASTPS, LASTVS, DEVICE, 0x4e55d0, 0x4e3c70, 0x4e3b30, ENGVAR, 0x4d0ff0, 0x4f6240, 0x4f4a70, 0x50c1a0, 0x50c2e0, 0x50c3d0};
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(300000u + (u32)seeds * 3000u), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  for (i = 5; i < argc; i++) {
    u32 va, by;
    if (!strcmp(argv[i], "rev")) { rev = 1; printf("order: original first\n"); }
    else if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; }
  }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9vs2_bind");
  if (!bind) { printf("mflrw_d3d9vs2_bind MISSING export\n"); return 1; }
  jmp_patch(0x4e55d0, (void *)h_settex); jmp_patch(0x4e3c70, (void *)h_settss); jmp_patch(0x4e3b30, (void *)h_flush); jmp_patch(0x4d0ff0, (void *)h_ltm);
  jmp_patch(0x4f6240, (void *)h_bsphere); jmp_patch(0x4f4a70, (void *)h_cone); jmp_patch(0x50c1a0, (void *)h_vsnormal); jmp_patch(0x50c2e0, (void *)h_vspoint);
  jmp_patch(0x50c3d0, (void *)h_vsradius);
  vtbl[0x1ac / 4] = P(d_setps); vtbl[0x170 / 4] = P(d_setvs); vtbl[0x148 / 4] = P(d_dip); vtbl[0x144 / 4] = P(d_dp);
  for (i = 0; i < (int)(sizeof t / sizeof t[0]); i++) tbl[i] = (void *)(uintptr_t)t[i];
  bind(tbl);
  for (i = 0; i < NF; i++) { fn[rev][i] = (void *)GetProcAddress(m, F[i].dll); fn[!rev][i] = (void *)(uintptr_t)F[i].va; if (!fn[rev][i]) { printf("%s MISSING export\n", F[i].dll); bad++; } }
  if (bad) return 1;
  printf("DLL %s, %d seeds x %d steps\n", argv[1], seeds, steps);
  for (seed = 1; seed <= (u32)seeds; seed++) {
    cur_seed = seed;
    run(0, seed, steps);
    nsave = nlg; save_log = realloc(save_log, nlg * 4); memcpy(save_log, LG, nlg * 4);
    run(1, seed, steps);
    tot += nlg;
    if (compare(seed) && ++bad >= 5) break;
  }
  for (i = 0; i < NF; i++) printf("%-52s %9ld calls (both sides)\n", F[i].dll + 5, ncalls[i]);
  { char line[96]; sprintf(line, "%.0f log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL"); fputs(line, stdout); }      /* one write: the DLL's exit line must not split it */
  printf("DONE\n");
  return bad != 0;
}
