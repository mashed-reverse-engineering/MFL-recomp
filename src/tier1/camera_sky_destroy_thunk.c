#include "mfl_tier2.h"
/*0044a020*/ extern int course_sky_atomic_destroy();
/* camera_sky_destroy_thunk */
/* "calls camera_fn_44a020 (course_sky_atomic_destroy)" */
/* Thunk for the sky camera's destroy step: forwards straight to course_sky_atomic_destroy().
 * Takes no parameters and returns nothing (inferred: only a tail call, no state of its own). */
void camera_sky_destroy_thunk(void)
{
  course_sky_atomic_destroy();
  return;
}
