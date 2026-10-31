#include "mfl_tier2.h"

extern float *rwp_sphere_0056c5d0(void *shape, void *xf, float *dir, float *out);

typedef struct MflShapeRef {
  int id0, id1;
  void *shape;   /* 0x8 */
  int id3;
  void *xf;   /* 0x10 */
} MflShapeRef;

/* Support points of the two shapes of a pair (pair[0], pair[1]) along dir (mode > 0) or against it: out[0..2] = support A - support B,
   out[3..5] = support B (kept for the caller).  *offset, when given, is added to the difference.
   Returns the difference projected on dir. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_aggchildren_0058a3f0(float *out, float *dir, int mode, MflShapeRef **pair, float *offset)
{
  float neg[3], supa[3];
  double d0, acc;
  float d1, d2;

  neg[0] = -dir[0];
  neg[1] = -dir[1];
  neg[2] = -dir[2];
  if (mode > 0) {
    rwp_sphere_0056c5d0(pair[0]->shape, pair[0]->xf, dir, supa);
    rwp_sphere_0056c5d0(pair[1]->shape, pair[1]->xf, neg, &out[3]);
  } else {
    rwp_sphere_0056c5d0(pair[0]->shape, pair[0]->xf, neg, supa);
    rwp_sphere_0056c5d0(pair[1]->shape, pair[1]->xf, dir, &out[3]);
  }
  d0 = supa[0] - out[3];
  out[0] = d0;
  d1 = supa[1] - out[4];
  out[1] = d1;
  d2 = supa[2] - out[5];
  out[2] = d2;
  if (offset) {
    out[0] = d0 + offset[0];
    out[1] = offset[1] + d1;
    out[2] = offset[2] + d2;
  }
  acc = dir[1] * out[1];
  acc = acc + out[0] * dir[0];
  acc = acc + dir[2] * out[2];
  return acc;
}
