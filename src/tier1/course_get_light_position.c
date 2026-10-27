#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_get_light_position */

/* "address of the directional light frame position" */

int course_get_light_position(void)
{
  return *(int *)(g_course_world.directional_light + 4) + 0x30;
}
