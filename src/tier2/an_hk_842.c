#include "mfl_tier2.h"
/* sphere_vs_box_test */

/* WARNING: Unknown calling convention */
/*
 * Coarse sphere-vs-box overlap test (inferred: a box-grown-by-radius test, not an
 * exact closest-point distance check).
 * box: float[6], max xyz at [0..2], min xyz at [3..5].
 * sphere: float[4], centre x y z at [0..2], radius at [3].
 * Returns 1 if the sphere centre lies inside the box grown by the radius on every
 * axis (the centre test is the special case of radius 0), else 0.
 * (was: "does the sphere (x, y, z, r) intersect the AABB (max xyz, min xyz)")
 */

undefined4 sphere_vs_box_test(float *box,float *sphere)

{
  /* centre inside the box on all three axes: already overlapping, skip the radius test */
  if (sphere[0] < box[3] || !(sphere[0] <= box[0]) ||
      sphere[1] < box[4] || !(sphere[1] <= box[1]) ||
      sphere[2] < box[5] || !(sphere[2] <= box[2])) {
    /* outside the box: reject unless the centre is within radius of it on every axis */
    if (sphere[3] + sphere[0] < box[3] || !(sphere[0] - sphere[3] <= box[0]) ||
        sphere[3] + sphere[1] < box[4] || !(sphere[1] - sphere[3] <= box[1]) ||
        sphere[3] + sphere[2] < box[5] || !(sphere[2] - sphere[3] <= box[2])) {
      return 0;
    }
  }
  return 1;
}




