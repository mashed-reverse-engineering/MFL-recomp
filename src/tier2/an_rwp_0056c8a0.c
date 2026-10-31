#include "mfl_tier2.h"

typedef struct MflShapeOps {
  unsigned char pad00[0x20];
  int (*support_interval)(void *shape, float *dir, float *lo, float *hi);   /* 0x20 */
} MflShapeOps;
typedef struct MflShape {
  unsigned char pad00[0x5c];
  MflShapeOps *ops;   /* 0x5c */
} MflShape;

/* Interval [*lo, *hi] of a shape along the direction dir given in world space: dir is rotated into the frame of xf (when given) for the
   shape's own query (ops->support_interval), the result is shifted by the translation of xf along dir. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_sphere_0056c8a0(MflShape *shape, float *xf, float *dir, float *lo, float *hi)
{
  float shift[2];
  float local[3];
  float *d;
  int r;

  shift[0] = 0.0f;
  d = dir;
  if (xf) {
    shift[0] = xf[13] * dir[1] + xf[12] * dir[0] + xf[14] * dir[2];
    local[0] = xf[1] * dir[1] + xf[0] * dir[0] + xf[2] * dir[2];
    local[1] = xf[5] * dir[1] + xf[4] * dir[0] + xf[6] * dir[2];
    local[2] = xf[9] * dir[1] + xf[8] * dir[0] + xf[10] * dir[2];
    d = local;
  }
  r = shape->ops->support_interval(shape, d, lo, hi);
  lo[0] = shift[0] + lo[0];
  hi[0] = shift[0] + hi[0];
  return r;
}
