#include "mfl_tier2.h"
#include <math.h>

/* A vector perpendicular to v: the component of v with the smallest magnitude is dropped (out[k] = 0, k from the ordering of the three
   magnitudes through the packed table 0x21312300) and the two others are swapped with a sign change.  Returns the index of the
   component that received v's third component. */
/* TIER2: unvalidated 2026-10-31 authority=diffemu */
int rwp_cylinder_0056fd90(float *out, float *v)
{
  float az;
  int a_gt_b, c_lt_a, c_lt_b, k, k1, k2;

  out[0] = fabs(v[0]);
  out[1] = fabs(v[1]);
  az = fabs(v[2]);
  out[2] = az;
  a_gt_b = out[0] > out[1];
  c_lt_a = az < out[0];
  c_lt_b = az < out[1];
  k = (0x21312300 >> ((((a_gt_b << 1) | c_lt_a) << 1 | c_lt_b) << 2)) & 3;
  k1 = (1 << k) & 3;
  k2 = (1 << k1) & 3;
  *(int *)&out[k] = 0;
  out[k1] = v[k2];
  out[k2] = -v[k1];
  return k1;
}
