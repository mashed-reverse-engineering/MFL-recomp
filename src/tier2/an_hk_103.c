#include "mfl_tier2.h"
/*005dd330*/ extern float float_1_0;
#define VEC3_FLOATS (3)
#define UNROLL_POINTS (4)
/* FUN_004c4620 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Computes the centroid (average position) of an array of 3D points.
   out_center: receives the average x, y, z (float[3]).
   points: array of float[3] points, point_count entries.
   point_count: number of points; the sum is divided by it, so it must be > 0 (inferred). */
void FUN_004c4620(float *out_center,float *points,int point_count)

{
  float inv_count;
  float *next_z;
  int next_point;
  int loop_count;

  /* Start the sum with point 0. */
  *out_center = *points;
  out_center[1] = points[1];
  out_center[2] = points[2];
  next_point = 1;
  if (3 < point_count + -1) {
    /* Unrolled: add 4 points per pass while the next full group fits. */
    loop_count = UNROLL_POINTS;
    next_z = points + 5;
    do {
      loop_count = loop_count + UNROLL_POINTS;
      next_point = next_point + UNROLL_POINTS;
      *out_center = next_z[-2] + *out_center;
      out_center[1] = next_z[-1] + out_center[1];
      out_center[2] = out_center[2] + *next_z;
      *out_center = next_z[1] + *out_center;
      out_center[1] = next_z[2] + out_center[1];
      out_center[2] = next_z[3] + out_center[2];
      *out_center = next_z[4] + *out_center;
      out_center[1] = next_z[5] + out_center[1];
      out_center[2] = next_z[6] + out_center[2];
      *out_center = next_z[7] + *out_center;
      out_center[1] = next_z[8] + out_center[1];
      out_center[2] = next_z[9] + out_center[2];
      next_z = next_z + (VEC3_FLOATS * UNROLL_POINTS);
    } while (loop_count < point_count);
  }
  /* Tail: add the remaining points one at a time. */
  if (next_point < point_count) {
    loop_count = point_count - next_point;
    next_z = points + next_point * VEC3_FLOATS + 2;
    do {
      loop_count = loop_count + -1;
      *out_center = next_z[-2] + *out_center;
      out_center[1] = next_z[-1] + out_center[1];
      out_center[2] = out_center[2] + *next_z;
      next_z = next_z + VEC3_FLOATS;
    } while (loop_count != 0);
  }
  /* Divide the sum by the point count. */
  inv_count = float_1_0 / (float)point_count;
  *out_center = inv_count * *out_center;
  out_center[1] = inv_count * out_center[1];
  out_center[2] = inv_count * out_center[2];
  return;
}


