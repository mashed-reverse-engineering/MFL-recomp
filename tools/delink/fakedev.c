/* fakedev.c: see fakedev.h.  Included by the check sources (#include "fakedev.c"), so it needs no separate compile step. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "fakedev.h"

void *fakedev_vtbl[FAKEDEV_SLOTS];
void (*fakedev_log)(uint32_t w);
int32_t fakedev_hr;
static void *fakedev_obj[2] = { fakedev_vtbl, 0 };

static void fakedev_trap(int n) { printf("TRAP unexpected method %d\n", n); fflush(stdout); ExitProcess(5); }
#define FD_TRAP(n) static long __stdcall fd_trap_##n(void *self) { fakedev_trap(n); return (long)0x8876086c; }
#define FD_R10(p) FD_TRAP(p##0) FD_TRAP(p##1) FD_TRAP(p##2) FD_TRAP(p##3) FD_TRAP(p##4) FD_TRAP(p##5) FD_TRAP(p##6) FD_TRAP(p##7) FD_TRAP(p##8) FD_TRAP(p##9)
FD_R10() FD_R10(1) FD_R10(2) FD_R10(3) FD_R10(4) FD_R10(5) FD_R10(6) FD_R10(7) FD_R10(8) FD_R10(9) FD_R10(10) FD_R10(11)
#define FD_T10(p) (void *)fd_trap_##p##0, (void *)fd_trap_##p##1, (void *)fd_trap_##p##2, (void *)fd_trap_##p##3, (void *)fd_trap_##p##4, \
                  (void *)fd_trap_##p##5, (void *)fd_trap_##p##6, (void *)fd_trap_##p##7, (void *)fd_trap_##p##8, (void *)fd_trap_##p##9
static void *const fd_traps[120] = { FD_T10(), FD_T10(1), FD_T10(2), FD_T10(3), FD_T10(4), FD_T10(5), FD_T10(6), FD_T10(7), FD_T10(8), FD_T10(9),
                                     FD_T10(10), FD_T10(11) };

/* ---- lane D recorders (d3drs) */
static long __stdcall fd_SetMaterial(void *self, const uint32_t *m) { int i; fakedev_log(FAKEDEV_TAG | 49); for (i = 0; i < 17; i++) fakedev_log(m[i]); return fakedev_hr; }
static long __stdcall fd_SetRenderState(void *self, uint32_t s, uint32_t v) { fakedev_log(FAKEDEV_TAG | 57); fakedev_log(s); fakedev_log(v); return fakedev_hr; }
static long __stdcall fd_SetTexture(void *self, uint32_t st, void *t) { fakedev_log(FAKEDEV_TAG | 65); fakedev_log(st); fakedev_log((uint32_t)(uintptr_t)t); return fakedev_hr; }
static long __stdcall fd_SetTextureStageState(void *self, uint32_t st, uint32_t ty, uint32_t v) { fakedev_log(FAKEDEV_TAG | 67); fakedev_log(st); fakedev_log(ty); fakedev_log(v); return fakedev_hr; }
static long __stdcall fd_SetSamplerState(void *self, uint32_t st, uint32_t ty, uint32_t v) { fakedev_log(FAKEDEV_TAG | 69); fakedev_log(st); fakedev_log(ty); fakedev_log(v); return fakedev_hr; }
static long __stdcall fd_SetCurrentTexturePalette(void *self, uint32_t p) { fakedev_log(FAKEDEV_TAG | 73); fakedev_log(p); return fakedev_hr; }

/* ---- lane D2 recorders (d3dim2d): 4 GetAvailableTextureMem, 26 CreateVertexBuffer, 27 CreateIndexBuffer, 81 DrawPrimitive, 82 DrawIndexedPrimitive,
   and the vertex / index buffer objects they create (Release 2, Lock 11, Unlock 12; any other buffer method = TRAP).  All locks point into one shared
   scratch arena at the lock offset; the range is filled with 0xa5 at Lock and its hash is logged at Unlock (= the bytes the caller wrote). */
uint32_t fakedev_texmem = 0x4000000;
int32_t fakedev_createhr, fakedev_lockhr;
struct fakedev_buf { void **vtbl; uint32_t id, len, usage, fmt, pool, refs, lockoff, locklen; };
static void *fd_bufvtbl[16];
static struct fakedev_buf *fd_bufs;      /* heap, not .bss: the checks map the exe image over a big .bss block of their own */
static int fd_nbuf;
static unsigned char *fd_scratch;
static void fakedev_buftrap(int n) { printf("TRAP unexpected buffer method %d\n", n); fflush(stdout); ExitProcess(5); }
#define FB_TRAP(n) static long __stdcall fb_trap_##n(void *self) { fakedev_buftrap(n); return (long)0x8876086c; }
FB_TRAP(0) FB_TRAP(1) FB_TRAP(3) FB_TRAP(4) FB_TRAP(5) FB_TRAP(6) FB_TRAP(7) FB_TRAP(8) FB_TRAP(9) FB_TRAP(10) FB_TRAP(13) FB_TRAP(14) FB_TRAP(15)
static unsigned long __stdcall fb_Release(struct fakedev_buf *b) { fakedev_log(FAKEDEV_TAG | 0x1002); fakedev_log(b->id); if (b->refs) b->refs--; fakedev_log(b->refs); return b->refs; }
static long __stdcall fb_Lock(struct fakedev_buf *b, uint32_t off, uint32_t len, void **pp, uint32_t flags) {
  uint32_t n = len ? len : b->len - off;
  fakedev_log(FAKEDEV_TAG | 0x100b); fakedev_log(b->id); fakedev_log(off); fakedev_log(len); fakedev_log(flags); fakedev_log((uint32_t)fakedev_lockhr);
  if (fakedev_lockhr < 0) return fakedev_lockhr;
  if (off > b->len || n > b->len - off || off + n > FAKEDEV_SCRATCH) { fakedev_log(0xbad10cc); return (long)0x8876086c; }      /* out of the buffer: D3DERR_INVALIDCALL */
  memset(fd_scratch + off, 0xa5, n); b->lockoff = off; b->locklen = n;
  *pp = fd_scratch + off;
  return fakedev_lockhr;
}
static long __stdcall fb_Unlock(struct fakedev_buf *b) {
  uint32_t h = 0x811c9dc5u, i;
  for (i = 0; i < b->locklen; i++) h = (h ^ fd_scratch[b->lockoff + i]) * 0x01000193u;
  fakedev_log(FAKEDEV_TAG | 0x100c); fakedev_log(b->id); fakedev_log(b->lockoff); fakedev_log(b->locklen); fakedev_log(h);
  b->locklen = 0;
  return 0;
}
static long fd_create(uint32_t len, uint32_t usage, uint32_t fmt, uint32_t pool, void **pp) {
  struct fakedev_buf *b;
  fakedev_log(len); fakedev_log(usage); fakedev_log(fmt); fakedev_log(pool); fakedev_log((uint32_t)fakedev_createhr);
  if (fakedev_createhr != 0 || fd_nbuf == FAKEDEV_BUFS) { if (fd_nbuf == FAKEDEV_BUFS) printf("fakedev: buffer pool full\n"); *pp = 0; return fakedev_createhr ? fakedev_createhr : (long)0x8007000e; }
  b = &fd_bufs[fd_nbuf]; b->vtbl = fd_bufvtbl; b->id = 0xb0f00000u + fd_nbuf++; b->len = len; b->usage = usage; b->fmt = fmt; b->pool = pool; b->refs = 1; b->lockoff = 0; b->locklen = 0;
  *pp = b; fakedev_log(b->id);
  return 0;
}
static uint32_t __stdcall fd_GetAvailableTextureMem(void *self) { fakedev_log(FAKEDEV_TAG | 4); fakedev_log(fakedev_texmem); return fakedev_texmem; }
static long __stdcall fd_CreateVertexBuffer(void *self, uint32_t len, uint32_t usage, uint32_t fvf, uint32_t pool, void **pp, void *share) { fakedev_log(FAKEDEV_TAG | 26); fakedev_log((uint32_t)(uintptr_t)share); return fd_create(len, usage, fvf, pool, pp); }
static long __stdcall fd_CreateIndexBuffer(void *self, uint32_t len, uint32_t usage, uint32_t fmt, uint32_t pool, void **pp, void *share) { fakedev_log(FAKEDEV_TAG | 27); fakedev_log((uint32_t)(uintptr_t)share); return fd_create(len, usage, fmt, pool, pp); }
static long __stdcall fd_DrawPrimitive(void *self, uint32_t type, uint32_t start, uint32_t count) { fakedev_log(FAKEDEV_TAG | 81); fakedev_log(type); fakedev_log(start); fakedev_log(count); return fakedev_hr; }
static long __stdcall fd_DrawIndexedPrimitive(void *self, uint32_t type, int32_t base, uint32_t minIdx, uint32_t nv, uint32_t start, uint32_t count) {
  fakedev_log(FAKEDEV_TAG | 82); fakedev_log(type); fakedev_log((uint32_t)base); fakedev_log(minIdx); fakedev_log(nv); fakedev_log(start); fakedev_log(count); return fakedev_hr;
}
/* ---- lane D3 recorders (d3dlights): 51 SetLight (index + the 26 words of the D3DLIGHT9), 53 LightEnable */
static long __stdcall fd_SetLight(void *self, uint32_t i, const uint32_t *l) { int k; fakedev_log(FAKEDEV_TAG | 51); fakedev_log(i); for (k = 0; k < 26; k++) fakedev_log(l[k]); return fakedev_hr; }
static long __stdcall fd_LightEnable(void *self, uint32_t i, int32_t e) { fakedev_log(FAKEDEV_TAG | 53); fakedev_log(i); fakedev_log((uint32_t)e); return fakedev_hr; }
/* ---- lane D4 recorders (d3dxform): 44 SetTransform (state + the 16 words of the matrix), 87 SetVertexDeclaration, 100 SetStreamSource */
static long __stdcall fd_SetTransform(void *self, uint32_t st, const uint32_t *m) { int k; fakedev_log(FAKEDEV_TAG | 44); fakedev_log(st); for (k = 0; k < 16; k++) fakedev_log(m[k]); return fakedev_hr; }
static long __stdcall fd_SetVertexDeclaration(void *self, uint32_t d) { fakedev_log(FAKEDEV_TAG | 87); fakedev_log(d); return fakedev_hr; }
static long __stdcall fd_SetStreamSource(void *self, uint32_t s, uint32_t vb, uint32_t off, uint32_t stride) { fakedev_log(FAKEDEV_TAG | 100); fakedev_log(s); fakedev_log(vb); fakedev_log(off); fakedev_log(stride); return fakedev_hr; }
/* ---- lane D5 recorders (d3d9pipe): 92 SetVertexShader, 104 SetIndices, 107 SetPixelShader */
static long __stdcall fd_SetVertexShader(void *self, uint32_t s) { fakedev_log(FAKEDEV_TAG | 92); fakedev_log(s); return fakedev_hr; }
static long __stdcall fd_SetIndices(void *self, uint32_t ib) { fakedev_log(FAKEDEV_TAG | 104); fakedev_log(ib); return fakedev_hr; }
static long __stdcall fd_SetPixelShader(void *self, uint32_t s) { fakedev_log(FAKEDEV_TAG | 107); fakedev_log(s); return fakedev_hr; }
void fakedev_reset_bufs(void) { fd_nbuf = 0; fakedev_createhr = 0; fakedev_lockhr = 0; }

void *fakedev_init(void) {
  int i;
  for (i = 0; i < FAKEDEV_SLOTS; i++) fakedev_vtbl[i] = fd_traps[i];
  fakedev_vtbl[49] = (void *)fd_SetMaterial; fakedev_vtbl[57] = (void *)fd_SetRenderState; fakedev_vtbl[65] = (void *)fd_SetTexture;
  fakedev_vtbl[67] = (void *)fd_SetTextureStageState; fakedev_vtbl[69] = (void *)fd_SetSamplerState; fakedev_vtbl[73] = (void *)fd_SetCurrentTexturePalette;
  /* ---- other lanes: add their slots here */
  fakedev_vtbl[4] = (void *)fd_GetAvailableTextureMem; fakedev_vtbl[26] = (void *)fd_CreateVertexBuffer; fakedev_vtbl[27] = (void *)fd_CreateIndexBuffer;
  fakedev_vtbl[81] = (void *)fd_DrawPrimitive; fakedev_vtbl[82] = (void *)fd_DrawIndexedPrimitive;
  fakedev_vtbl[51] = (void *)fd_SetLight; fakedev_vtbl[53] = (void *)fd_LightEnable;
  fakedev_vtbl[44] = (void *)fd_SetTransform; fakedev_vtbl[87] = (void *)fd_SetVertexDeclaration; fakedev_vtbl[100] = (void *)fd_SetStreamSource;
  fakedev_vtbl[92] = (void *)fd_SetVertexShader; fakedev_vtbl[104] = (void *)fd_SetIndices; fakedev_vtbl[107] = (void *)fd_SetPixelShader;
  { void *const t[16] = { (void *)fb_trap_0, (void *)fb_trap_1, (void *)fb_Release, (void *)fb_trap_3, (void *)fb_trap_4, (void *)fb_trap_5, (void *)fb_trap_6, (void *)fb_trap_7,
                          (void *)fb_trap_8, (void *)fb_trap_9, (void *)fb_trap_10, (void *)fb_Lock, (void *)fb_Unlock, (void *)fb_trap_13, (void *)fb_trap_14, (void *)fb_trap_15 };
    for (i = 0; i < 16; i++) fd_bufvtbl[i] = t[i]; }
  if (!fd_bufs) { fd_bufs = calloc(FAKEDEV_BUFS, sizeof *fd_bufs); fd_scratch = malloc(FAKEDEV_SCRATCH); }
  fakedev_reset_bufs();
  return fakedev_obj;
}
