#include "mfl_tier2.h"
#include "rwp_common.h"
#include <math.h>

/* Shape data of a body shape (reached through shape->data at 0x5c): the feature callback at 0x1c fills the world-space face planes and edge
   directions of a shape; kind 2 is a half space whose normal is the third row of the body matrix; flat (0x39) is zero for round shapes. */
typedef struct MflShapeData {
  unsigned char pad00[0x1c];
  void (*features)(void *shape, float *xf, MflAxis *planes, float *edges);   /* 0x1c */
  unsigned char pad20[0x18];
  unsigned char kind;   /* 0x38 */
  unsigned char flat;   /* 0x39 */
  unsigned short n_edges;   /* 0x3a */
  unsigned short n_planes;   /* 0x3c */
} MflShapeData;
typedef struct MflPairShape {
  unsigned char pad00[0x4c];
  float radius;   /* 0x4c */
  unsigned char pad50[0xc];
  MflShapeData *data;   /* 0x5c */
} MflPairShape;
typedef struct MflPairBody {
  int id0, id1;
  MflPairShape *shape;   /* 0x8 */
  int id3;
  float *xf;   /* 0x10: 4x4 matrix, translation at floats 12..14 */
} MflPairBody;

extern float rwp_cylinder_0056fd20(float *out, float *in);
extern float rwp_aggregate_00589270(MflPairBody **pair, MflAxis *axis, float *out_n, float *out_t);
extern float rwp_aggregate_00589350(MflPairBody **pair, MflAxis *axis, float *out_n, float *out_t);
extern void rwp_aggregate_00589430(float *a, float *b, float *n, float *d);
extern float rwp_aggregate_005891c0(MflPairBody **pair, float *dir, float *out);

/* Separating-axis query for the two bodies of `pair`: finds the direction `dir` with the largest separation of the two shapes, the point `*offset`
   on that axis between them and the penetration `*depth` (separation minus both radii).  Two round shapes: the axis joins the body centres;
   a half space (kind 2) uses its normal; otherwise the face planes of both shapes (589270 / 589350) and the edge cross products are tried, or
   when the shapes report no features the closest-point direction (589430).  *offset is only written when the depth is not above t.
   Returns 1.  The unnamed a1 / a5 arguments are not used. */
/* TIER2: unvalidated 2026-11-10 authority=static */
int rwp_aggregate_00588cb0(void *a1, MflPairBody **pair, float t, float *dir, float *offset, float *depth, void *a5)
{
  MflShapeData *da = pair[0]->shape->data;
  MflShapeData *db = pair[1]->shape->data;
  float v, off, best, tt, tn[3], c[3], len2, inv, d;
  float ea[4][3], eb[4][3];
  MflAxis pa[4], pb[4], ax;
  int i, j;

  if (db->flat + da->flat == 0) {
    dir[0] = pair[0]->xf[12] - pair[1]->xf[12];
    dir[1] = pair[0]->xf[13] - pair[1]->xf[13];
    dir[2] = pair[0]->xf[14] - pair[1]->xf[14];
    v = rwp_cylinder_0056fd20(dir, dir);
    off = pair[1]->xf[13] * dir[1] + pair[1]->xf[12] * dir[0] + pair[1]->xf[14] * dir[2];
  } else if (da->kind == 2) {
    ax.n[0] = pair[0]->xf[8];
    ax.n[1] = pair[0]->xf[9];
    ax.n[2] = pair[0]->xf[10];
    ax.lo = -3.4028235e38f;
    ax.hi = 0.0f;
    v = rwp_aggregate_00589270(pair, &ax, dir, &off);
  } else if (db->kind == 2) {
    ax.n[0] = pair[1]->xf[8];
    ax.n[1] = pair[1]->xf[9];
    ax.n[2] = pair[1]->xf[10];
    ax.lo = -3.4028235e38f;
    ax.hi = 0.0f;
    v = rwp_aggregate_00589350(pair, &ax, dir, &off);
  } else {
    j = db->n_edges * da->n_edges + db->n_planes + da->n_planes;
    da->features(pair[0]->shape, pair[0]->xf, pa, &ea[0][0]);
    db->features(pair[1]->shape, pair[1]->xf, pb, &eb[0][0]);
    if (j == 0) {
      if (da->n_edges) rwp_aggregate_00589430(pair[1]->xf + 12, pair[0]->xf + 12, &ea[0][0], dir);
      else rwp_aggregate_00589430(pair[0]->xf + 12, pair[1]->xf + 12, &eb[0][0], dir);
      v = rwp_aggregate_005891c0(pair, dir, &off);
    } else {
      off = 0.0f;
      best = -3.4028235e38f;
      for (i = 0; i < da->n_planes; i++) {
        v = rwp_aggregate_00589270(pair, &pa[i], tn, &tt);
        if (v > best) {
          off = tt;
          dir[0] = tn[0];
          dir[1] = tn[1];
          dir[2] = tn[2];
          best = v;
        }
      }
      for (i = 0; i < db->n_planes; i++) {
        v = rwp_aggregate_00589350(pair, &pb[i], tn, &tt);
        if (v > best) {
          off = tt;
          dir[0] = tn[0];
          dir[1] = tn[1];
          dir[2] = tn[2];
          best = v;
        }
      }
      for (i = 0; i < da->n_edges; i++) {
        for (j = 0; j < db->n_edges; j++) {
          c[0] = eb[j][2] * ea[i][1] - eb[j][1] * ea[i][2];
          c[1] = eb[j][0] * ea[i][2] - ea[i][0] * eb[j][2];
          c[2] = eb[j][1] * ea[i][0] - eb[j][0] * ea[i][1];
          len2 = c[1] * c[1] + c[0] * c[0] + c[2] * c[2];
          if (len2 > 9.999999974752427e-07f) {
            inv = 1.0f / sqrt(len2);
            c[0] = c[0] * inv;
            c[1] = c[1] * inv;
            c[2] = inv * c[2];
            v = rwp_aggregate_005891c0(pair, c, &tt);
            if (v > best) {
              off = tt;
              dir[0] = c[0];
              dir[1] = c[1];
              dir[2] = c[2];
              best = v;
            }
          }
        }
      }
      if (best > -3.4028235e38f) v = best;
      else {
        rwp_aggregate_00589430(pair[0]->xf + 12, pair[1]->xf + 12, &eb[0][0], dir);
        v = rwp_aggregate_005891c0(pair, dir, &off);
      }
    }
  }
  d = v - pair[0]->shape->radius - pair[1]->shape->radius;
  *depth = d;
  if (!(d > t)) *offset = (pair[1]->shape->radius - pair[0]->shape->radius + v) * 0.5f + off;
  return 1;
}
