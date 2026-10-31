#include "mfl_tier2.h"

typedef struct MflBodyRef {
  int id0, id1;
  void *shape;   /* 0x8 */
  int id3;
  float *xf;   /* 0x10 */
} MflBodyRef;

/* A direction with the interval [lo, hi] that the first body covers along it. */
typedef struct MflAxis {
  float n[3];
  float lo, hi;
} MflAxis;

extern void rwp_sphere_0056c8a0(void *shape, float *xf, MflAxis *axis, float *far_end, float *near_end);

/* Separation along one axis of a pair (pair[0], pair[1]): the interval of the second body along axis (from rwp_sphere_0056c8a0)
   is compared with the first body's interval shifted by the position of the first body.  The axis (or its negation) and the
   interval end with the smaller overlap go to out_n / *out_t.  Returns the overlap. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_aggregate_00589270(MflBodyRef **pair, MflAxis *axis, float *out_n, float *out_t)
{
  float fv[2];
  float *xf = pair[0]->xf;
  MflBodyRef *b2 = pair[1];
  float centre, near_end, far_end, overlap;

  centre = xf[13] * axis->n[1] + xf[12] * axis->n[0] + xf[14] * axis->n[2];
  fv[0] = centre + axis->lo;
  fv[1] = centre + axis->hi;
  rwp_sphere_0056c8a0(b2->shape, b2->xf, axis, &far_end, &near_end);
  overlap = fv[0] - near_end;
  fv[0] = far_end - fv[1];
  if (overlap > fv[0]) {
    out_t[0] = near_end;
    *(int *)&out_n[0] = *(int *)&axis->n[0];
    *(int *)&out_n[1] = *(int *)&axis->n[1];
    *(int *)&out_n[2] = *(int *)&axis->n[2];
    return overlap;
  }
  out_n[0] = axis->n[0] * -1.0f;
  out_n[1] = axis->n[1] * -1.0f;
  out_n[2] = axis->n[2] * -1.0f;
  out_t[0] = -far_end;
  return fv[0];
}
