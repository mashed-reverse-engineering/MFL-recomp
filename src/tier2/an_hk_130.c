#include "mfl_tier2.h"
/* FUN_0056f500
 * Builds an axis-aligned bounding box around two corner points, grown by `radius` on every side.
 * bounds_out[0..2] receives the per-axis maximum + radius (high corner),
 * bounds_out[4..6] receives the per-axis minimum - radius (low corner).
 * Slots [3] and [7] are not written.
 */

void FUN_0056f500(float *bounds_out,float *corner_a,float *corner_b,float radius)

{
  float axis_max;

  /* x axis: low = smaller corner, high = larger corner */
  if (*corner_b <= *corner_a) {
    bounds_out[4] = *corner_b - radius;
    axis_max = *corner_a;
  }
  else {
    bounds_out[4] = *corner_a - radius;
    axis_max = *corner_b;
  }
  *bounds_out = radius + axis_max;
  /* y axis: same low/high split as x */
  if (corner_b[1] <= corner_a[1]) {
    bounds_out[5] = corner_b[1] - radius;
    axis_max = corner_a[1];
  }
  else {
    bounds_out[5] = corner_a[1] - radius;
    axis_max = corner_b[1];
  }
  bounds_out[1] = radius + axis_max;
  /* z axis: same low/high split as x, test written the other way round */
  if (corner_a[2] < corner_b[2]) {
    bounds_out[6] = corner_a[2] - radius;
    bounds_out[2] = radius + corner_b[2];
    return;
  }
  bounds_out[6] = corner_b[2] - radius;
  bounds_out[2] = radius + corner_a[2];
  return;
}


