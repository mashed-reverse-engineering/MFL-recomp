#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_get_collision_bsp */

/* "g_course_world.collision_bsps[0]" */

uint course_get_collision_bsp(void)
{
  return g_course_world.collision_bsps[0];
}
