#include "mfl_tier2.h"

extern int rwp_pipeline_00570f20(void *d);
extern int rwp_pipeline_00571070(void *d);
extern int rwp_capsule_00575b20(int size, int align, int tag);
extern int rwp_capsule_00575af0(void *p, int v, int n);
extern int rwp_batch_005782f0();
extern int rwp_batch_005793d0(void *ar, int n);
extern int rwp_batch_00578f10(void *ar, void *out, void *w, void *d);
extern int rwp_pipeline_00570e80(void *ar, void *out, void *d);
extern int rwp_batch_00579330(void *ar, void *out, int n8, int nc, int n14);
extern int rwp_batch_00577da0(void *ar, void *o1, void *o2, void *o3, int a, int b, int c, int d);
extern int rwp_grid_00577a00(void *ar, void *o1, void *o2, void *o3, int a, int b, int c);
extern int rwp_batch_0057ab00();
extern int rwp_batch_0057b460();
extern char LAB_0057b9c0[];
extern char LAB_005797f0[];
extern char LAB_00579ea0[];

/* The description of a world to build (the same layout as MflWorldLayout of rwp_pipeline_00571070; only the fields used here). */
typedef struct MflWorldDesc {
  unsigned int flags;   /* 0x0 */
  unsigned char pad04[4];
  int n8;   /* 0x8 */
  int nc;   /* 0xc */
  int n10;   /* 0x10 */
  int n14;   /* 0x14 */
  int n18;   /* 0x18 */
  int n1c;   /* 0x1c */
  int n20;   /* 0x20 */
  unsigned char pad24[4];
  int n28;   /* 0x28 */
  int n2c;   /* 0x2c */
  unsigned char pad30[0xc];
  int n3c;   /* 0x3c */
} MflWorldDesc;

/* A bump allocator over a block: base, size, cur. */
typedef struct MflArena {
  char *base;   /* 0x0 */
  int size;   /* 0x4 */
  int cur;   /* 0x8 */
} MflArena;

typedef struct MflWorldBuf {
  unsigned char pad00[4];
  int c04;   /* 0x4 */
  int c08;   /* 0x8 */
  float f0c;   /* 0xc */
  float f10;   /* 0x10 */
  float f14;   /* 0x14 */
  float f18;   /* 0x18 */
  float f1c;   /* 0x1c */
  int i20;   /* 0x20 */
  float f24;   /* 0x24 */
  float f28;   /* 0x28 */
  unsigned char pad2c[0x18];
  int i44;   /* 0x44 */
  int i48;   /* 0x48 */
  int i4c;   /* 0x4c */
  int i50;   /* 0x50 */
  int i54;   /* 0x54 */
  int i58;   /* 0x58 */
  float f5c;   /* 0x5c */
  float f60;   /* 0x60 */
  int i64;   /* 0x64 */
  int i68;   /* 0x68 */
  unsigned int flags;   /* 0x6c */
  int i70;   /* 0x70 */
  unsigned int p74;   /* 0x74 */
  int z78;   /* 0x78 */
  int n7c;   /* 0x7c */
  unsigned int p80;   /* 0x80 */
  int z84;   /* 0x84 */
  int n88;   /* 0x88 */
  unsigned int p8c;   /* 0x8c */
  int z90;   /* 0x90 */
  int n94;   /* 0x94 */
  unsigned int p98;   /* 0x98 */
  unsigned int p9c;   /* 0x9c */
  int za0;   /* 0xa0 */
  int na4;   /* 0xa4 */
  unsigned int pa8;   /* 0xa8 */
  int zac;   /* 0xac */
  int nb0;   /* 0xb0 */
  unsigned int pb4;   /* 0xb4 */
  int zb8;   /* 0xb8 */
  int nbc;   /* 0xbc */
  int ic0;   /* 0xc0 */
  int nc4;   /* 0xc4 */
  int zc8;   /* 0xc8 */
  unsigned int pcc;   /* 0xcc */
  unsigned int pd0;   /* 0xd0 */
  unsigned int pd4;   /* 0xd4 */
  unsigned int pd8;   /* 0xd8 */
  unsigned int pdc;   /* 0xdc */
  int ne0;   /* 0xe0 */
  unsigned char be4[0x10];   /* 0xe4 */
  int fnf4;   /* 0xf4 */
  int fnf8;   /* 0xf8 */
  unsigned char bfc[0x6c];   /* 0xfc */
  unsigned char b168[0x15c];   /* 0x168 */
  unsigned char b2c4[0xa0];   /* 0x2c4 */
  unsigned char b364[0x18];   /* 0x364 */
  unsigned char b37c[0x64];   /* 0x37c */
  unsigned int p3e0;   /* 0x3e0 */
  int z3e4;   /* 0x3e4 */
  int n3e8;   /* 0x3e8 */
  unsigned char b3ec[0x10];   /* 0x3ec */
  unsigned char b3fc[4];   /* 0x3fc */
  unsigned char b400[4];   /* 0x400 */
  int fn404;   /* 0x404 */
  int pad408;
  MflArena a0;   /* 0x408 .. */
  MflArena a1;
} MflWorldBuf;

/* zero n bytes at the arena address a */
#define ZERO(a, n) rwp_capsule_00575af0((void *)(a), 0, (n))
#define ALIGN16(x) (((x) + 15) & ~15)

/* Builds a physics world in memory: allocates the two arenas (sizes from rwp_pipeline_00570f20 / 00571070), sets the tuning constants and carves
   every array of the world out of the arenas (16 byte aligned, zeroed where the original zeroes). Returns w. */
/* TIER2: unvalidated 2026-11-02 authority=diffemu */
MflWorldBuf *rwp_pipeline_00571200(MflWorldBuf *w, int a1, MflWorldDesc *d, float dt)
{
  MflArena *ar;
  MflArena *a1p;
  char *blk;
  int n, i, off1, off2;

  w->flags = d->flags;
  n = rwp_pipeline_00570f20(d);
  w->a0.size = n;
  blk = (char *)rwp_capsule_00575b20(n, 0x10, 0x30900);
  w->a0.base = blk;
  w->a0.cur = (int)blk;
  n = rwp_pipeline_00571070(d) - w->a0.size;
  w->a1.size = n;
  a1p = &w->a1;
  blk = (char *)rwp_capsule_00575b20(n, 0x10, 0x30900);
  w->a1.cur = (int)blk;
  a1p->base = blk;
  w->i58 = rwp_batch_005782f0();
  w->i54 = 0;
  w->i50 = 0;
  w->i4c = 0;
  w->f0c = dt * 0.1f;
  w->f10 = 0.6f;
  w->f18 = 10000.0f;
  w->f1c = 0.05f;
  w->f24 = 0.05f;
  w->f28 = -0.16f;
  w->f14 = dt + dt;
  w->i20 = d->n3c;
  w->f5c = 1e-5f;
  w->f60 = 1e-5f;
  w->c04 = 100;
  w->c08 = 300;
  w->i68 = 1;
  w->i44 = 400;
  w->i48 = 400;
  w->i70 = a1;
  a1p->cur = ALIGN16(a1p->cur);
  w->p74 = a1p->cur;
  ZERO(a1p->cur, d->n10 * 4);
  a1p->cur = ALIGN16(a1p->cur + d->n10 * 4);
  w->n7c = d->n10;
  w->z78 = 0;
  a1p->cur = ALIGN16(a1p->cur);
  w->p80 = a1p->cur;
  ZERO(a1p->cur, d->n1c * 20);
  a1p->cur = ALIGN16(a1p->cur + d->n1c * 20);
  w->n88 = d->n1c;
  w->z84 = 0;
  a1p->cur = ALIGN16(a1p->cur);
  w->pb4 = a1p->cur;
  ZERO(a1p->cur, d->nc * 4);
  a1p->cur = ALIGN16(a1p->cur + d->nc * 4);
  w->nbc = d->nc;
  w->zb8 = 0;
  w->ic0 = rwp_batch_005793d0(a1p, d->n20);
  if (d->flags & 8) { w->a0.cur = (int)w->a0.base; ar = &w->a0; } else ar = a1p;
  rwp_batch_00578f10(ar, w->b37c, w, d);
  w->n3e8 = d->n20;
  a1p->cur = ALIGN16(a1p->cur);
  w->p3e0 = a1p->cur;
  a1p->cur = ALIGN16(a1p->cur + d->n20 * 20);
  w->z3e4 = 0;
  if (d->flags & 2) { ar = &w->a0; if (!(d->flags & 8)) w->a0.cur = (int)w->a0.base; } else ar = a1p;
  ar->cur = ALIGN16(ar->cur);
  w->pd8 = ar->cur;
  ar->cur = ALIGN16(ar->cur + 0x30);
  ar->cur = ALIGN16(ar->cur + (d->n10 - 1) * 40);
  w->pdc = ar->cur;
  ar->cur = ALIGN16(ar->cur + d->n8 * 16);
  w->ne0 = d->n8;
  a1p->cur = ALIGN16(a1p->cur);
  w->pcc = a1p->cur;
  ZERO(a1p->cur, d->n10 * 4);
  a1p->cur = ALIGN16(a1p->cur + d->n10 * 4);
  w->pd0 = a1p->cur;
  ZERO(a1p->cur, d->nc * 4);
  a1p->cur = ALIGN16(a1p->cur + d->nc * 4);
  w->pd4 = a1p->cur;
  ZERO(a1p->cur, d->n10 * 40);
  a1p->cur = ALIGN16(a1p->cur + d->n10 * 40);
  w->nc4 = d->n10;
  w->zc8 = 0;
  if (d->flags & 4) { ar = &w->a0; if (!(d->flags & 0xa)) w->a0.cur = (int)w->a0.base; } else ar = a1p;
  rwp_pipeline_00570e80(ar, &w->f1c, d);
  if (d->flags & 1) { w->a0.cur = (int)w->a0.base; ar = &w->a0; } else ar = a1p;
  rwp_batch_00579330(ar, w->be4, d->n8, d->nc, d->n14);
  a1p->cur = ALIGN16(a1p->cur);
  w->p8c = a1p->cur;
  ZERO(a1p->cur, d->n20 * 7 * 32);
  a1p->cur = ALIGN16(a1p->cur + d->n20 * 7 * 32);
  w->n94 = d->n20;
  w->z90 = 0;
  if (d->flags & 0x20) { w->a0.cur = (int)w->a0.base; ar = &w->a0; } else ar = a1p;
  ar->cur = ALIGN16(ar->cur);
  w->p9c = ar->cur;
  ZERO(ar->cur, d->n20 * 4);
  ar->cur = ALIGN16(ar->cur + d->n20 * 4);
  w->na4 = d->n20;
  w->za0 = 0;
  ar->cur = ALIGN16(ar->cur);
  w->pa8 = ar->cur;
  ZERO(ar->cur, d->n20 * 16);
  ar->cur = ALIGN16(ar->cur + d->n20 * 16);
  w->nb0 = d->n20 * 4;
  w->zac = 0;
  if (d->flags & 0x10) { ar = &w->a0; if (!(d->flags & 0x20)) w->a0.cur = (int)w->a0.base; } else ar = a1p;
  rwp_batch_00577da0(ar, w->bfc, w->b168, w->b400, d->n28 * 4, d->n14, d->n18 + d->n20, d->n10);
  rwp_grid_00577a00(ar, w->b364, w->b2c4, w->b3fc, d->n28 * 4, d->n14, d->n10);
  if (d->flags & 0x40) { a1p = &w->a0; if (!(d->flags & 0x30)) w->a0.cur = (int)w->a0.base; }
  a1p->cur = ALIGN16(a1p->cur);
  w->p98 = a1p->cur;
  ZERO(a1p->cur, d->n20 * 64);
  a1p->cur = ALIGN16(a1p->cur + d->n20 * 64);
  for (i = 0, off1 = 0, off2 = 0; i < d->n20; i++, off1 += 0xe0, off2 += 0x40)
    *(unsigned int *)(w->p8c + off1 + 0xb0) = w->p98 + off2;
  w->i44 = d->n2c;
  w->i48 = d->n28;
  w->i64 = 0;
  w->fn404 = (int)LAB_0057b9c0;
  if (w->i58 != 2) {
    w->fnf4 = (int)LAB_005797f0;
    w->fnf8 = (int)LAB_00579ea0;
    return w;
  }
  w->fnf4 = (int)rwp_batch_0057ab00;
  w->fnf8 = (int)rwp_batch_0057b460;
  return w;
}
