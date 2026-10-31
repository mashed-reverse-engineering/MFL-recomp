#include "mfl_tier2.h"

typedef struct MflBodyRef {
  int id0, id1;
  void *shape;   /* 0x8 */
  int id3;
  float *xf;   /* 0x10 */
} MflBodyRef;

/* A direction with the interval [lo, hi] that the second body covers along it. */
typedef struct MflAxis {
  float n[3];
  float lo, hi;
} MflAxis;

extern void rwp_sphere_0056c8a0(void *shape, float *xf, MflAxis *axis, float *far_end, float *near_end);

/* Mirror of rwp_aggregate_00589270: the interval of the first body along axis is compared with the second body's interval
   shifted by the position of the second body. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
float rwp_aggregate_00589350(MflBodyRef **pair, MflAxis *axis, float *out_n, float *out_t)
{
  float *xf = pair[1]->xf;
  float centre, lo, hi, near_end, far_end, overlap;

  rwp_sphere_0056c8a0(pair[0]->shape, pair[0]->xf, axis, &far_end, &near_end);
  centre = xf[13] * axis->n[1] + xf[12] * axis->n[0] + xf[14] * axis->n[2];
  lo = centre + axis->lo;
  hi = centre + axis->hi;
  overlap = far_end - hi;
  far_end = lo - near_end;
  if (overlap > far_end) {
    out_t[0] = hi;
    *(int *)&out_n[0] = *(int *)&axis->n[0];
    *(int *)&out_n[1] = *(int *)&axis->n[1];
    *(int *)&out_n[2] = *(int *)&axis->n[2];
    return overlap;
  }
  out_n[0] = axis->n[0] * -1.0f;
  out_n[1] = axis->n[1] * -1.0f;
  out_n[2] = axis->n[2] * -1.0f;
  out_t[0] = -lo;
  return far_end;
}
