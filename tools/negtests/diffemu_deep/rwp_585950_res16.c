#include "mfl_tier2.h"
/* NEGATIVE FIXTURE (do not install): pre-fix text of recomp/automatch_near/an_rwp_00585950.c (git ada96ece^): `float res[16]`, the callee 586f20 writes 0xec bytes */

typedef struct MflShape {
  unsigned char pad00[0x50];
  int id;   /* 0x50 */
  unsigned short type;   /* 0x54: 1 = point, 2 = ..., other = volume */
  unsigned short mask;   /* 0x56 */
  int v58;   /* 0x58 */
  int v5c;   /* 0x5c */
  float normal[3];   /* 0x60 */
  int v6c;   /* 0x6c */
  float lo0;   /* 0x70 */
  float lo1;   /* 0x74 */
  float radius;   /* 0x78 */
  unsigned char flags;   /* 0x7c: bit 6 = oriented by the filter word */
  unsigned char pad7d;
  unsigned short filter;   /* 0x7e */
} MflShape;
typedef struct MflShapePair {
  MflShape a;
  MflShape b;   /* 0x80 */
  float pos[3];   /* 0x100 */
  int v10c;   /* 0x10c */
  float margin;   /* 0x110 */
} MflShapePair;
typedef struct MflHitRec {
  int z[3];
  int v2c;
  int v30;
  int pad34[5];
} MflHitRec;
typedef struct MflHit {
  float point[3];   /* 0x0 */
  unsigned char pad0c[0x20 - 0xc];
  MflHitRec recs[3];   /* 0x20 */
  unsigned char pad98[0xac - 0x98];
  unsigned int count;   /* 0xac */
  unsigned char pad_b0[4];
  int id_a;   /* 0xb4 */
  int v_b8;
  int id_b;   /* 0xbc */
  int v_c0;
  float lo0;   /* 0xc4 */
  float lo1;   /* 0xc8 */
  unsigned char kind_a;   /* 0xcc */
  unsigned char kind_b;   /* 0xcd */
  unsigned short pad_ce;
} MflHit;

extern int rwp_aggregate_00586f20(int id_a, int type_a, int id_b, int type_b, float diff, int v10c, float *pos, float *margin, int *kind_a,
                                  int *kind_b, MflShapePair *pair, float *res);
extern void rwp_aggregate_00585170(MflShapePair *pair, float *res, unsigned int count, float limit, float sum, MflHit *out);

int rwp_aggregate_00585950(MflShapePair *p, float limit, MflHit *out)
{
  float sum, diff, marg;
  int kind_a, kind_b;
  unsigned int count, i;
  float res[16];
  float lo1;
  unsigned short ta = p->a.type, tb = p->b.type;
  unsigned short fa;

  sum = p->a.radius + p->b.radius;
  diff = p->b.radius - p->a.radius;
  marg = p->margin + sum;
  kind_a = ta == 1 ? 0 : (ta != 2) + 1;
  kind_b = tb == 1 ? 0 : (tb != 2) + 1;
  count = rwp_aggregate_00586f20(p->a.id, ta, p->b.id, tb, diff, p->v10c, p->pos, &marg, &kind_a, &kind_b, p, res);
  if (marg - sum > limit) return 0;
  if (count < 1) return 0;
  out->point[0] = p->pos[0];
  out->point[1] = p->pos[1];
  out->point[2] = p->pos[2];
  out->id_a = p->a.v58;
  out->v_b8 = p->a.v5c;
  out->id_b = p->b.v58;
  out->v_c0 = p->b.v5c;
  out->lo0 = p->a.lo0 <= p->b.lo0 ? p->a.lo0 : p->b.lo0;
  lo1 = p->a.lo1 <= p->b.lo1 ? p->a.lo1 : p->b.lo1;
  out->kind_a = kind_a;
  out->kind_b = kind_b;
  out->lo1 = lo1;
  out->pad_ce = 0;
  out->count = 0;
  rwp_aggregate_00585170(p, res, count, limit, sum, out);
  if (out->count == 0) return 0;
  for (i = 0; i < out->count; i++) {
    out->recs[i].v2c = p->a.v6c;
    out->recs[i].v30 = p->b.v6c;
    out->recs[i].z[2] = 0;
    out->recs[i].z[1] = 0;
    out->recs[i].z[0] = 0;
  }
  if (p->a.flags & 0x40) {
    fa = p->a.filter;
    if (fa != 0xe) {
      if ((~fa & p->a.mask) != 0 || (kind_a == 2 && (fa & 1))) {
        out->point[0] = -p->a.normal[0];
        out->point[1] = -p->a.normal[1];
        out->point[2] = -p->a.normal[2];
      }
      return 1;
    }
  }
  if (p->b.flags & 0x40) {
    fa = p->b.filter;
    if (fa != 0xe) {
      if ((~fa & p->b.mask) != 0 || (kind_b == 2 && (fa & 1))) {
        out->point[0] = p->b.normal[0];
        out->point[1] = p->b.normal[1];
        out->point[2] = p->b.normal[2];
      }
      return 1;
    }
  }
  return 1;
}
