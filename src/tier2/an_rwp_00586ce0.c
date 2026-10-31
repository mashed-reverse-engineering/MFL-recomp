#include "mfl_tier2.h"

/* A pair filter: a bit set indexed by stride * first id + second id (the bits start at 0xc). */
typedef struct MflPairFilter {
  int pad0;
  int stride;   /* 0x4 */
  int pad8;
  unsigned int bits[1];   /* 0xc */
} MflPairFilter;
/* An object's descriptor: a 16 bit id at 0x5a. */
typedef struct MflItemObj {
  unsigned char pad00[0x5a];
  unsigned short id;   /* 0x5a */
} MflItemObj;
/* One entry of the item list (20 bytes): key, 16 bit id at 4, owner object at 8. */
typedef struct MflItem {
  int key;
  unsigned short id;   /* 0x4 */
  unsigned short pad06;
  MflItemObj *obj;   /* 0x8 */
  int pad0c[2];
} MflItem;
/* The matching box of an item (32 bytes): min at 0, max at 0x10. */
typedef struct MflBox {
  float min[3];
  float pad0c;
  float max[3];
  float pad1c;
} MflBox;
/* The pair collector. */
typedef struct MflBroadCtx {
  MflItem **out;   /* 0x0: output pairs (two item pointers each) */
  unsigned int count;   /* 0x4 */
  unsigned int capacity;   /* 0x8 */
  MflItem *items;   /* 0xc */
  unsigned char pad10[0x14];
  MflBox *boxes;   /* 0x24 */
  unsigned char pad28[0x48];
  float tol;   /* 0x70 */
  unsigned char pad74[4];
  int full;   /* 0x78 */
} MflBroadCtx;

/* Pairs every item i < split (outer) with the n_inner items split .. split + n_inner - 1 (inner): a pair is skipped when either item
   is marked (obj == 1), both have the same key and id, both ids are 0xffff, a pair filter (by id, or by the owner objects' ids) marks it,
   or the boxes are apart by more than the tolerance on an axis (the same box always counts as overlapping).  The pairs go to ctx->out until
   ctx->capacity is reached (ctx->full is set); ctx->count receives the number of pairs.  Returns that number. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
unsigned int rwp_aggregate_00586ce0(MflBroadCtx *ctx, unsigned int split, unsigned int n_inner, MflPairFilter *f_id, MflPairFilter *f_obj)
{
  unsigned int n = 0, i, j, idx;
  MflItem *a = ctx->items, *b;
  MflBox *box_a = ctx->boxes, *box_b;
  MflItem **out = ctx->out;
  unsigned int cap = ctx->capacity;
  float tol = ctx->tol;

  if (!(split > 0)) {
    ctx->count = 0;
    return 0;
  }
  for (i = 0; i < split; i++, a++, box_a++) {
    if (!(n < cap)) break;
    if (a->obj == (MflItemObj *)1) continue;
    b = &ctx->items[split];
    box_b = &ctx->boxes[split];
    for (j = 0; j < n_inner; j++, b++, box_b++) {
      if (b->obj == (MflItemObj *)1) continue;
      if (a->key == b->key && a->id == b->id) continue;
      if (a->id == 0xffff && b->id == 0xffff) continue;
      if (f_id) {
        idx = f_id->stride * (a->id & 0xffff) + b->id;
        if (f_id->bits[idx >> 5] & (1 << (idx & 0x1f))) continue;
      }
      if (f_obj) {
        idx = f_obj->stride * a->obj->id + b->obj->id;
        if (f_obj->bits[idx >> 5] & (1 << (idx & 0x1f))) continue;
      }
      if (box_a != box_b) {
        if (!(box_a->max[0] - box_b->min[0] <= tol)) continue;
        if (!(box_b->max[0] - box_a->min[0] <= tol)) continue;
        if (!(box_a->max[1] - box_b->min[1] <= tol)) continue;
        if (!(box_b->max[1] - box_a->min[1] <= tol)) continue;
        if (!(box_a->max[2] - box_b->min[2] <= tol)) continue;
        if (!(box_b->max[2] - box_a->min[2] <= tol)) continue;
      }
      out[n * 2] = a;
      out[n * 2 + 1] = b;
      n++;
      if (n >= cap) {
        ctx->full = 1;
        break;
      }
    }
  }
  ctx->count = n;
  return n;
}
