#include "mfl_tier2.h"
/*00675668*/ extern struct CourseLap g_course_lap_data;


/* course_get_ai_polygon_count */

/* "g_course_lap_data.ai_polygon_count" */

int course_get_ai_polygon_count(void)
{
  return g_course_lap_data.ai_polygon_count;
}
