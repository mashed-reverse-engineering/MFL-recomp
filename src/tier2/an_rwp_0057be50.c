#include "mfl_tier2.h"

extern void rwp_sphere_0056c350(void *obj, int a, int b, float *box);
extern int rwp_volume_0056b420(void *owner, void *shape);
extern int rwp_volume_0056b180(void *owner, void *shape, float *box);

/* The parameter block behind the world pointer: the overlap tolerance is at 0xc014. */
typedef struct MflWorldParams {
  unsigned char pad00[0xc014];
  float tol;   /* 0xc014 */
} MflWorldParams;
/* An entry of the pair table (20 bytes). */
typedef struct MflPairRec {
  void *a;
  int tag;   /* 0x4 */
  void *b;   /* 0x8 */
  unsigned short flags;   /* 0xc */
  unsigned short pad0e;
  int pad10;   /* 0x10 */
} MflPairRec;
/* The pair table: records at 0x80, count at 0x84, capacity at 0x88. */
typedef struct MflPairStore {
  unsigned char pad00[0x80];
  MflPairRec *recs;   /* 0x80 */
  unsigned int count;   /* 0x84 */
  unsigned int capacity;   /* 0x88 */
} MflPairStore;
/* An object of the first set: its 8 float bounds come from rwp_sphere_0056c350; a 16 bit id at 0x5a. */
typedef struct MflObjA {
  unsigned char pad00[0x5a];
  unsigned short id;   /* 0x5a */
} MflObjA;
/* An object of the second set: owner at 0x24. */
typedef struct MflObjB {
  unsigned char pad00[0x24];
  void *owner;   /* 0x24 */
} MflObjB;
/* Pairs already known: a bit set indexed by stride * object index + id. */
typedef struct MflPairFilter {
  int pad0;
  int stride;   /* 0x4 */
  int pad8;
  unsigned int bits[1];   /* 0xc */
} MflPairFilter;
typedef struct MflBatchCtx {
  MflWorldParams **world;   /* 0x0 */
  MflPairStore *store;   /* 0x4 */
  MflObjA **list;   /* 0x8 */
  unsigned int count;   /* 0xc */
} MflBatchCtx;

/* Broad phase: every object of ctx->list is tested against the n objects objs (bounds overlap within the world tolerance on all
   three axes, pair not yet marked in flt) and the overlapping pairs are appended to the pair table until it is full.  Returns the
   number of pairs added. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
unsigned int rwp_batch_0057be50(MflBatchCtx *ctx, MflObjB **objs, unsigned int n, MflPairFilter *flt)
{
  MflPairStore *st = ctx->store;
  unsigned int start, i, j, bit_idx;
  MflObjA **cursor;
  MflObjA *obj;
  MflPairRec *rec;
  float a[8], b[8];
  int tag;
  MflWorldParams *wp;

  start = st->count;
  if (!(start < st->capacity)) return 0;
  cursor = ctx->list;
  for (i = 0; i < ctx->count; i++, cursor++) {
    obj = *cursor;
    if (!obj) continue;
    *(unsigned short *)&tag = 0xffff;
    rwp_sphere_0056c350(obj, 0, 0, a);
    for (j = 0; j < n; j++) {
      if (flt) {
        bit_idx = flt->stride * (rwp_volume_0056b420(objs[j]->owner, objs[j]) & 0xffff) + (*cursor)->id;
        if (flt->bits[bit_idx >> 5] & (1 << (bit_idx & 0x1f))) continue;
      }
      if (!rwp_volume_0056b180(objs[j]->owner, objs[j], b)) continue;
      wp = *ctx->world;
      if (!(a[4] - b[0] <= wp->tol)) continue;
      if (!(b[4] - a[0] <= wp->tol)) continue;
      if (!(a[5] - b[1] <= wp->tol)) continue;
      if (!(b[5] - a[1] <= wp->tol)) continue;
      if (!(a[6] - b[2] <= wp->tol)) continue;
      if (!(b[6] - a[2] <= wp->tol)) continue;
      rec = &st->recs[st->count];
      st->count = st->count + 1;
      rec->a = obj;
      rec->tag = tag;
      rec->flags = 0x8000;
      rec->b = objs[j];
      rec->pad10 = 0;
      if (st->count == st->capacity) return st->count - start;
    }
  }
  return st->count - start;
}
