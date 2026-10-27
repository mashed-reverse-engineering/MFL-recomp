#include "mfl_tier2.h"
/*00675668*/ extern struct CourseLap g_course_lap_data;


/* course_get_safe_start_count */

/* "g_course_lap_data.safe_start_count" */

int course_get_safe_start_count(void)
{
  return g_course_lap_data.safe_start_count;
}
