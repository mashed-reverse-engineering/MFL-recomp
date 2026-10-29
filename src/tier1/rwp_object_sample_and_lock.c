#include "mfl_tier2.h"
/*00493960*/ extern void *__cdecl rwp_cars_sphere_points_generate(int object, float scale, int lock_object);
/*00493ca0*/ extern int __cdecl rwp_collision_mesh_to_geometry(void *samples, int flag);
/*00575200*/ extern void __cdecl FUN_00575200(void *samples);
/*005e64a4*/ extern float k_f_0;
/* rwp_object_sample_and_lock */

/* Builds the sphere sample points of an object (locking it when lock_flag is set), turns them into collision geometry and frees them; returns the geometry result. */
int __cdecl rwp_object_sample_and_lock(int object, int lock_flag)
{
  void *samples;
  int result;
  /* two calls, not a ternary: the compiler merges the common tail (push object / push 0 then jmp) like the original */
  if (lock_flag) samples = rwp_cars_sphere_points_generate(object, 1.0f, object); else samples = rwp_cars_sphere_points_generate(object, 1.0f, 0);
  result = rwp_collision_mesh_to_geometry(samples, *(float *)(object + 0x4c) > k_f_0);
  FUN_00575200(samples);
  return result;
}
