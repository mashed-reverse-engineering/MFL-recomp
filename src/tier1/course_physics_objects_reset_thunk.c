#include "mfl_tier2.h"
/*00455370*/ extern int course_physics_objects_reset();
/* course_physics_objects_reset_thunk */
/* "calls course_physics_objects_reset" */
/* Thunk: forwards to course_physics_objects_reset() (0x00455370) and discards its int result.
 * Takes no parameters and returns nothing. */
void course_physics_objects_reset_thunk(void)
{
  course_physics_objects_reset();
  return;
}
