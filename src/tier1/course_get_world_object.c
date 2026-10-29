#include "mfl_tier2.h"
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00668c18*/ extern struct CourseWorld g_course_world;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/* course_get_world_object */

/* "returns &g_course_world" */

CourseWorld * course_get_world_object(void)
{
  return &g_course_world;
}
