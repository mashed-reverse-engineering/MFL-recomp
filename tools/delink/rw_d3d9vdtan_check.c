/* rw_d3d9vdtan_check.c: behavioural check of the librw DLL's d3d9vdtan export (tools/delink/rw_d3d9vdtan.txt: the tangent instancer FUN_00508600 of
   d3d9vertexdeclaration.obj, librw-mfl capi/d3d9vdtan.cpp) against the ORIGINAL code of MFL.exe, both run natively (Wine).
   usage: rw_d3d9vdtan_check.exe <mflrw.dll> <MFL.exe> [seeds=200] [steps=3000] [va:byte ...]  (build/run: tools/delink/rw_d3d9vdtan_check.sh; image mapping as rw_d3drs_check.c).
   RwV3dLength 0x4d53a0, _rwInvSqrt 0x4d5470 and _rpD3D9VertexDeclarationInstV3d 0x5063f0 get a jmp to a recorder at their original entry (the length / inverse root
   recorders return an UNROUNDED long double in st0, so a side that rounds it differs); RwEngineInstance 0x7d2128 points to a fake engine (RwMalloc / RwFree +0x108 /
   +0x10c on a bump arena); the index buffer is a fake whose Lock hands out the op's index array.  The instance recorder logs a hash of the tangent array it gets.
   Inputs: positions on a coarse grid with duplicates (some off by less / more than the 1e-6 merge distance), float normals or compressed byte normals, uvs, 1-3 meshes
   of triangle lists / strips (degenerate strip triangles, odd strip windings).  Per seed and control word (0x7f, 0x27f) one random op sequence runs twice from the same
   reset state, first through the DLL export, then through the original address.  Extra arguments va:byte patch the ORIGINAL's code first (negative fixtures). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <windows.h>
typedef uint32_t u32; typedef uint16_t u16; typedef uint8_t u8;
#define W(va) (*(volatile u32 *)(uintptr_t)(va))
#define W16(va) (*(volatile u16 *)(uintptr_t)(va))
#define FW(va) (*(volatile float *)(uintptr_t)(va))
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

#define ENGVAR 0x7d2128u
#define T 0xc0de0000u
static u32 *LG, nlg, cap;
static void lg(u32 v) { if (nlg == cap) { cap = cap ? cap * 2 : 1 << 20; LG = realloc(LG, cap * 4); } LG[nlg++] = v; }
static unsigned short cw_now(void) { unsigned short c; __asm__ volatile("fnstcw %0" : "=m"(c)); return c; }
static void cw_set(unsigned short c) { __asm__ volatile("fldcw %0" : : "m"(c)); }
static u32 rs;
static u32 rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static u32 hashw(const u8 *p, u32 n) { u32 h = 0x811c9dc5u, i; for (i = 0; i < n; i++) h = (h ^ p[i]) * 0x01000193u; return h; }
static u32 fbits(float f) { u32 u; memcpy(&u, &f, 4); return u; }

static u8 *arena, *oa; static u32 narena, noa;
#define ARENA 0x1000000u
static u32 aalloc(u32 n) { u8 *p; if (narena + n + 16 > ARENA) { printf("ARENA full\n"); ExitProcess(6); } p = arena + narena; narena += (n + 15) & ~15u; memset(p, 0xcd, n); return (u32)(uintptr_t)p; }
static u32 oalloc(u32 n) { u8 *p; if (noa + n + 16 > ARENA) { printf("OBJ ARENA full\n"); ExitProcess(6); } p = oa + noa; noa += (n + 15) & ~15u; memset(p, 0, n); return (u32)(uintptr_t)p; }
static u32 eng[0x50];
static u32 last_m, last_msz;      /* the tangent array (first RwMalloc of an op): hashed at Unlock = after the per-triangle pass */
static u32 __cdecl e_malloc(u32 size, u32 hint) { lg(T | 0x108); lg(size); lg(hint); last_msz = size; return last_m = aalloc(size); }
static void __cdecl e_free(u32 p) { lg(T | 0x10c); lg(p); }
static long double __cdecl r_len(const float *v) { long double x = v[0], y = v[1], z = v[2]; lg(T | 1); lg(fbits(v[0])); lg(fbits(v[1])); lg(fbits(v[2])); return sqrtl(x * x + y * y + z * z) * 1.0000001L; }
static long double __cdecl r_invsqrt(float x) { lg(T | 2); lg(fbits(x)); return x > 0 ? 1.0L / sqrtl((long double)x) * 1.0000001L : 1e30L; }
static u32 __cdecl r_instv3d(u32 a0, u32 a1, u32 tan, u32 nv, u32 a7) { u32 r = rnd(); lg(T | 3); lg(a0); lg(a1); lg(tan); lg(nv); lg(a7); lg(hashw((u8 *)(uintptr_t)tan, nv * 12)); lg(r); return r; }
static const struct { u32 va; void *fn; } STUB[] = { {0x4d53a0, r_len}, {0x4d5470, r_invsqrt}, {0x5063f0, r_instv3d} };
static void patch_jmp(u32 va, void *fn) { u8 *p = (u8 *)(uintptr_t)va; p[0] = 0xe9; *(u32 *)(p + 1) = (u32)(uintptr_t)fn - (va + 5); }

/* fake index buffer: Lock 11 hands out the op's indices, Unlock 12 */
static void *ibvt[16]; static u32 cur_idx;
static void ibtrap(void) { printf("TRAP unexpected index buffer method\n"); fflush(stdout); ExitProcess(5); }
static long __stdcall ib_lock(u32 b, u32 off, u32 len, u32 *pp, u32 fl) { lg(T | 0x10b); lg(off); lg(len); lg(fl); *pp = cur_idx; return 0; }
static int dbg_step = -1, dbg_seed, cur_step, cur_seedv, cur_cw;      /* dbg=<seed>,<step>: print the tangents after the triangle pass of that op (both sides, CW 0x7f) */
static long __stdcall ib_unlock(u32 b) {
  lg(T | 0x10c0); lg(hashw((u8 *)(uintptr_t)last_m, last_msz));
  if (cur_step == dbg_step && cur_seedv == dbg_seed && cur_cw == 0x7f) { u32 i; printf("dbg tangents:"); for (i = 0; i < last_msz / 4; i++) printf(" %08x", W(last_m + 4 * i)); printf("\n"); }
  return 0;
}
static u32 ibobj[2];

typedef u32 (__cdecl *G8)(u32, u32, u32, u32, u32, u32, u32, u32);
static void *fn[2];
static long ncalls;

static float coord(void) { static const float c[] = { 0.0f, 1.0f, -1.0f, 0.5f, 2.0f, -0.25f, 3.0f, 0.125f }; float f = c[rnd() % 8]; if (rnd() % 6 == 0) f += (rnd() % 2 ? 4e-7f : 3e-6f) * (rnd() % 2 ? 1 : -1); return f; }
static float uvf(void) { return rnd() % 4 ? (float)(rnd() % 64) / 16.0f : (float)(int32_t)rnd() / 3e9f; }
static void reset(u32 seed, unsigned short cw) { nlg = 0; rs = seed * 2654435761u + cw + 1; narena = 0; noa = 0; }
static void run(int side, u32 seed, int steps, unsigned short cw) {
  int s; u32 i;
  reset(seed, cw);
  for (s = 0; s < steps; s++) {
    u32 nv = rnd() % 4 ? rnd() % 24 + 3 : rnd() % 80 + 3, pos = oalloc(nv * 12), nrm = 0, cn = oalloc(nv * 4 + 4), uv = oalloc(nv * 8), hdr, nm = rnd() % 3 + 1, tot = 0, r;
    u32 ndist = rnd() % 3 ? nv / 2 + 1 : nv;      /* distinct positions: fewer = more merges */
    for (i = 0; i < nv; i++) {
      u32 src = i < ndist ? i : rnd() % ndist;
      if (i < ndist) { FW(pos + 12 * i) = coord(); FW(pos + 12 * i + 4) = coord(); FW(pos + 12 * i + 8) = coord(); }
      else { W(pos + 12 * i) = W(pos + 12 * src); W(pos + 12 * i + 4) = W(pos + 12 * src + 4); W(pos + 12 * i + 8) = W(pos + 12 * src + 8); if (rnd() % 5 == 0) FW(pos + 12 * i + 4 * (rnd() % 3)) += rnd() % 2 ? 5e-7f : 2e-5f; }
      W(cn + 4 * i) = rnd() % 3 && i >= ndist ? W(cn + 4 * src) : rnd();
      FW(uv + 8 * i) = uvf(); FW(uv + 8 * i + 4) = uvf();
    }
    if (rnd() % 2) { nrm = oalloc(nv * 12); for (i = 0; i < nv * 3; i++) FW(nrm + 4 * i) = rnd() % 3 ? (float)((int)(rnd() % 5) - 2) * 0.5f : (float)(int32_t)rnd() / 2e9f; }
    hdr = oalloc(0x40 + 0x24 * nm);
    W(hdr + 4) = nm; W(hdr + 0xc) = rnd() % 5 ? (rnd() % 2 ? 4 : 5) : 6; W(hdr + 0x3c) = nv;
    for (i = 0; i < nm; i++) {
      u32 inst = hdr + 0x40 + 0x24 * i, n = rnd() % 30 + 2, mv = rnd() % 3 ? 0 : rnd() % (nv / 2 + 1);
      if (W(hdr + 0xc) == 4) n = n / 3 * 3 + (rnd() % 8 == 0 ? 1 : 0);
      W(inst) = n; W(inst + 4) = mv; W(inst + 0x1c) = tot; tot += n;
    }
    cur_idx = oalloc(tot * 2 + 16);
    { u32 mvmax = 0;      /* a mesh's loop reads 2 indices past its end (the next mesh's): every index must be valid for every minVert */
      for (i = 0; i < nm; i++) if (W(hdr + 0x44 + 0x24 * i) > mvmax) mvmax = W(hdr + 0x44 + 0x24 * i);
      for (i = 0; i < tot + 4; i++) W16(cur_idx + 2 * i) = rnd() % 7 == 0 && i ? W16(cur_idx + 2 * (i - 1)) : rnd() % (nv - mvmax); }
    W(hdr + 0x38) = tot; W(hdr + 8) = (u32)(uintptr_t)ibobj;
    lg(0x57e90000u + s); lg(nv); lg(tot);
    cur_step = s; cur_seedv = seed; cur_cw = cw;
    if (s == dbg_step && seed == (u32)dbg_seed && cw == 0x7f && side == 0) {
      printf("dbg op: prim %u meshes %u\n", W(hdr + 0xc), nm);
      for (i = 0; i < nm; i++) printf("dbg mesh %u: n %u mv %u start %u\n", i, W(hdr + 0x40 + 0x24 * i), W(hdr + 0x44 + 0x24 * i), W(hdr + 0x5c + 0x24 * i));
      printf("dbg idx:"); for (i = 0; i < tot + 3; i++) printf(" %u", W16(cur_idx + 2 * i)); printf("\n");
      printf("dbg pos:"); for (i = 0; i < nv * 3; i++) printf(" %08x", W(pos + 4 * i)); printf("\n");
      printf("dbg uv:"); for (i = 0; i < nv * 2; i++) printf(" %08x", W(uv + 4 * i)); printf("\n");
    }
    cw_set(cw);
    r = ((G8)fn[side])(0xa0000000u + s, 0xa1000000u, pos, nrm, cn, uv, hdr, 0xa7000000u); ncalls++;
    lg(r); lg(cw_now());
    { u32 env[7]; __asm__ volatile("fnstenv %0" : "=m"(env)); lg(env[1] & 0x3800); lg(env[2] & 0xffff); __asm__ volatile("fldenv %0" : : "m"(env)); }      /* x87 TOP and tag word: a leaked register shows */
    cw_set(0x27f);
  }
}

static u32 *save_log, nsave; static int bad_dumps;
static int compare(u32 seed, unsigned short cw) {
  u32 i, n = nlg < nsave ? nlg : nsave;
  for (i = 0; i < n && save_log[i] == LG[i]; i++);
  if (i < n || nlg != nsave) {
    u32 k = i, st = 0;
    for (; k > 0; k--) if ((LG[k] & 0xffff0000u) == 0x57e90000u) { st = LG[k] & 0xffff; break; }
    printf("  seed %u cw %x: log differs at entry %u (step %u): dll %08x orig %08x (lengths %u / %u)\n", seed, cw, i, st, i < nsave ? save_log[i] : 0, i < nlg ? LG[i] : 0, nsave, nlg);
    if (bad_dumps++ < 2) for (k = i > 24 ? i - 24 : 0; k < i + 8 && k < n; k++) printf("    %6u  %08x %08x%s\n", k, save_log[k], LG[k], save_log[k] != LG[k] ? "  <" : "");
    return 1;
  }
  return 0;
}
static volatile int cur_seed;
static DWORD WINAPI watchdog(LPVOID p) { Sleep((DWORD)(uintptr_t)p); printf("TIMEOUT at seed %d\n", cur_seed); fflush(stdout); ExitProcess(3); return 0; }
static LONG WINAPI crash(EXCEPTION_POINTERS *e) { printf("CRASH %08lx at %p seed %d\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, cur_seed); fflush(stdout); ExitProcess(4); return 0; }

int main(int argc, char **argv) {
  HMODULE m = LoadLibraryA(argv[1]); int seeds = argc > 3 ? atoi(argv[3]) : 200, steps = argc > 4 ? atoi(argv[4]) : 3000, i, bad = 0, ci; u32 seed; long tot = 0;
  static const unsigned short cws[2] = { 0x7f, 0x27f };
  int (__cdecl *bind)(int, void *const *);
  static void *table[10];
  static const u32 tv[10] = { ENGVAR, 0x5ec3e0, 0x5dd330, 0x5ec3d8, 0x5ec3d0, 0x5ec3d4, 0x5df368, 0x4d53a0, 0x4d5470, 0x5063f0 };
  setvbuf(stdout, NULL, _IONBF, 0);
  SetUnhandledExceptionFilter(crash); SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
  CreateThread(NULL, 0, watchdog, (LPVOID)(uintptr_t)(600000u + (u32)seeds * (u32)steps), 0, NULL);
  if (!m || !load_image(argv[2])) { printf("cannot open dll/exe\n"); return 2; }
  arena = malloc(ARENA); oa = malloc(ARENA);
  eng[0x108 / 4] = (u32)(uintptr_t)e_malloc; eng[0x10c / 4] = (u32)(uintptr_t)e_free; W(ENGVAR) = (u32)(uintptr_t)eng;
  for (i = 0; i < 16; i++) ibvt[i] = (void *)ibtrap;
  ibvt[11] = (void *)ib_lock; ibvt[12] = (void *)ib_unlock; ibobj[0] = (u32)(uintptr_t)ibvt;
  for (i = 0; i < (int)(sizeof STUB / sizeof STUB[0]); i++) patch_jmp(STUB[i].va, STUB[i].fn);
  printf("constants: eps %g one %g eps2 %g k %g dot %g cos %g\n", FW(0x5ec3e0), FW(0x5dd330), *(double *)(uintptr_t)0x5ec3d8, FW(0x5ec3d0), FW(0x5ec3d4), FW(0x5df368));
  for (i = 5; i < argc; i++) if (sscanf(argv[i], "dbg=%d,%d", &dbg_seed, &dbg_step) == 2) printf("debug: seed %d step %d\n", dbg_seed, dbg_step);
  for (i = 5; i < argc; i++) { u32 va, by; if (sscanf(argv[i], "%x:%x", &va, &by) == 2) { printf("mutation: orig byte at %x %02x -> %02x\n", va, *(unsigned char *)(uintptr_t)va, by); *(unsigned char *)(uintptr_t)va = (unsigned char)by; } }
  bind = (void *)GetProcAddress(m, "mflrw_d3d9vdtan_bind");
  if (!bind) { printf("mflrw_d3d9vdtan_bind MISSING export\n"); return 1; }
  for (i = 0; i < 10; i++) table[i] = (void *)(uintptr_t)tv[i];
  if (bind(9, table) || !bind(10, table)) { printf("mflrw_d3d9vdtan_bind count check wrong (DIFF)\n"); return 1; }
  fn[1] = (void *)(uintptr_t)0x508600;
  fn[0] = (void *)GetProcAddress(m, "rw37__rpD3D9VertexDeclarationInstTangent");
  if (!fn[0]) { printf("rw37__rpD3D9VertexDeclarationInstTangent MISSING export\n"); return 1; }
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
  printf("%-45s %9ld calls (both sides)\n", "rw37__rpD3D9VertexDeclarationInstTangent", ncalls);
  printf("%ld log words compared; %s\n", tot, bad ? "DIFF" : "EQUAL");
  printf("DONE\n");
  return bad != 0;
}
