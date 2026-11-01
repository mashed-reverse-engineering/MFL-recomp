#include "mfl_tier2.h"
#include "rwp_common.h"

/* The aggregate collector context embedded at 0x37c of the world object (see 586300). */
typedef struct MflPairRef {
  int first;
  int second;
} MflPairRef;
typedef struct MflAggCtx {
  MflPairRef *pairs;   /* 0x0 */
  int f04;
  unsigned char pad08[0x8];
  int nrec;   /* 0x10 */
  unsigned char pad14[8];
  int nmat;   /* 0x1c */
  unsigned char pad20[0x3c];
  int f5c;   /* 0x5c */
  unsigned char pad60[0x10];
  float tol;   /* 0x70 */
  float f74;   /* 0x74 */
  int overflow;   /* 0x78 */
} MflAggCtx;
typedef struct MflWorldParams {
  unsigned char pad00[0xc014];
  float tol;   /* 0xc014 */
  int cap;   /* 0xc018 */
} MflWorldParams;
typedef struct MflWorld {
  MflWorldParams *params;   /* 0x0 */
  unsigned char pad04[8];
  float f0c;   /* 0xc */
} MflWorld;
typedef struct MflPhysWorld {
  unsigned char pad00[0x70];
  MflWorld **world;   /* 0x70 */
  unsigned char pad74[0x37c - 0x74];
  MflAggCtx agg;   /* 0x37c */
} MflPhysWorld;
/* An entry (20 bytes): two aggregate handles (object, 16 bit id) and a filter word at 0x10. */
typedef struct MflBroadEntry {
  MflAggHandle a;
  MflAggHandle b;   /* 0x8 */
  void *filter;   /* 0x10 */
} MflBroadEntry;
typedef struct MflShapeObj {
  unsigned char pad00[0x20];
  unsigned short slot;   /* 0x20 */
} MflShapeObj;

extern void rwp_sphere_0056c350(void *obj, int a, int b, float *box);
extern int rwp_volume_0056b180(void *owner, void *shape, float *box);
extern int rwp_aggregate_00586300(MflAggCtx *ctx, MflAggHandle *h, MflAggBox *src, MflAggBox *one, MflAggBox *ref, void *extra);
extern unsigned int rwp_aggregate_00586ce0(MflAggCtx *ctx, unsigned int split, unsigned int n_inner, void *f_id, int f_obj);
extern int rwp_constraints_00583050(MflPhysWorld *w, MflPairRef *pair, void *info_a, void *info_b);

/* Narrow phase for a list of n broad phase entries: for every entry the per-body slots (a map from the shape's 16 bit slot to a record, only
   when the handle is not the single object marker 0xffff) and the bounds of both handles are looked up, the aggregates of both sides are collected
   into the world's collector (586300 twice), the surviving pairs are produced by 586ce0 and each one is resolved by 583050.  Returns the sum of
   the 583050 results. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_convexhullmassprops_00583d20(MflPhysWorld *w, MflBroadEntry *entries, unsigned int n, unsigned short *slot_map, MflAggBox *recs,
                                     int *infos, int a7)
{
  int total = 0;
  unsigned int i, k, c1, c2, np;
  MflWorld *world;
  MflAggCtx *agg;
  MflBroadEntry *e;
  MflAggBox *rec_a, *rec_b;
  int *info_a, *info_b;
  unsigned short s;
  MflAggBox box_a, box_b;

  if (!n || !a7) return 0;
  world = *w->world;
  agg = &w->agg;
  agg->tol = world->params->tol;
  agg->f74 = world->f0c;
  for (i = 0, e = entries; i < n; i++, e++) {
    if (agg->overflow != 0) break;
    rec_a = 0;
    info_a = 0;
    if (e->a.id != 0xffff) {
      s = slot_map[((MflShapeObj *)e->a.ptr)->slot];
      if (s != 0) {
        rec_a = recs + s;
        info_a = infos + s;
      }
    }
    rec_b = 0;
    info_b = 0;
    if (e->b.id != 0xffff) {
      s = slot_map[((MflShapeObj *)e->b.ptr)->slot];
      if (s != 0) {
        rec_b = recs + s;
        info_b = infos + s;
      }
    }
    if (e->a.id == 0xffff) rwp_sphere_0056c350(e->a.ptr, 0, 0, (float *)&box_a);
    else rwp_volume_0056b180(world, e->a.ptr, (float *)&box_a);
    if (e->b.id == 0xffff) rwp_sphere_0056c350(e->b.ptr, 0, 0, (float *)&box_b);
    else rwp_volume_0056b180(world, e->b.ptr, (float *)&box_b);
    agg->f04 = 0;
    agg->nmat = 0;
    agg->nrec = 0;
    agg->f5c = 0;
    c1 = rwp_aggregate_00586300(agg, &e->a, rec_a, &box_a, &box_b, &e->b);
    if (!c1) continue;
    c2 = rwp_aggregate_00586300(agg, &e->b, rec_b, &box_b, &box_a, &e->a);
    if (!c2) continue;
    np = rwp_aggregate_00586ce0(agg, c1, c2, e->filter, world->params->cap);
    for (k = 0; k < np; k++)
      total += rwp_constraints_00583050(w, agg->pairs + k, info_a, info_b);
  }
  return total;
}
