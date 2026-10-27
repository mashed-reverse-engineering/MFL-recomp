#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_is_outdoor */

/* "g_course_world.flag_outdoor" */

int course_is_outdoor(void)
{
  return g_course_world.flag_outdoor;
}
