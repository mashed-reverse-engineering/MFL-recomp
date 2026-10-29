#include "mfl_tier2.h"
/*00493960*/ extern int rwp_cars_sphere_points_generate(int p1, float p2, undefined4 p3);
/*00493ca0*/ extern int rwp_collision_mesh_to_geometry();
/*00575200*/ extern void FUN_00575200(undefined4 p0);
/*005e64a4*/ extern float k_f_0;
/* rwp_object_sample_and_lock */

/* "builds the sphere samples and locks the object (when lock_flag non zero)" */

/* Builds the sphere sample points for `object` and turns them into collision geometry.
 * object: handle of the object (struct not known yet).
 * lock_flag: non zero = lock the object (passed to the sample generator), zero = do not lock.
 * Returns the result of rwp_collision_mesh_to_geometry (inferred: a status / geometry handle).
 * The sample handle is released with FUN_00575200 before returning. */

undefined4 rwp_object_sample_and_lock(int object,int lock_flag)

{
  undefined4 sphere_samples;
  undefined4 result;
  int lock_object;

  /* lock_object: the object when locking, 0 when not (inferred: 0 = no lock target) */
  lock_object = object;
  if (lock_flag == 0) {
    lock_object = 0;
  }
  sphere_samples = rwp_cars_sphere_points_generate(object,1.0,lock_object);
  result = rwp_collision_mesh_to_geometry(sphere_samples,k_f_0 < *(float *)(object + 0x4c));
  FUN_00575200(sphere_samples);
  return result;
}


