#include "mfl_tier2.h"
/*004219e0*/ extern undefined __cdecl course_call_cb4(void *p0);
/*00421aa0*/ extern int course_select();
/*00421b00*/ extern int course_has_cb4();
/*006095bc*/ extern undefined4 g_course_race_6095bc;
/*00664418*/ extern struct CourseCounts g_course_counts_data;
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_race_start */

/* "cb4 when present, sets 0x6095BC" */

/* Start the race for the current course and invoke callback 4 if available. */
void course_race_start(void)
{
  int has_cb4;
  
  course_select(g_course_counts_data.course_id);
  has_cb4 = course_has_cb4();
  /* Execute callback 4 on the course world if present; sets unknown global flag 0x6095bc */
  if (has_cb4 != 0) {
    course_call_cb4(&g_course_world);
    g_course_race_6095bc = 1;
  }
  return;
}
