#include "mfl_tier2.h"

typedef struct MflShapeOps {
  unsigned char pad00[0x14];
  int (*support_point)(void *shape, float *dir, float *out);   /* 0x14 */
} MflShapeOps;
typedef struct MflShape {
  unsigned char pad00[0x5c];
  MflShapeOps *ops;   /* 0x5c */
} MflShape;

/* Interval [*lo, *hi] of a shape along dir from its support point query (ops->support_point): hi from the support point in direction
   dir, lo from the one in direction -dir; each is only written when the query succeeds (the second only after the first). */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
void rwp_sphere_0056c9b0(MflShape *shape, float *dir, float *lo, float *hi)
{
  float sup[3], neg[3];

  if (shape->ops->support_point(shape, dir, sup) != 0) {
    hi[0] = dir[1] * sup[1] + sup[0] * dir[0] + dir[2] * sup[2];
    neg[0] = dir[0] * -1.0f;
    neg[1] = dir[1] * -1.0f;
    neg[2] = dir[2] * -1.0f;
    if (shape->ops->support_point(shape, neg, sup) != 0) {
      lo[0] = dir[1] * sup[1] + sup[0] * dir[0] + dir[2] * sup[2];
    }
  }
}
