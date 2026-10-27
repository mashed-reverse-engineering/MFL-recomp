#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_get_ambient_light */

/* "g_course_world.ambient_light" */

uint course_get_ambient_light(void)
{
  return g_course_world.ambient_light;
}
