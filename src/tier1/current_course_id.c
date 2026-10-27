#include "mfl_tier2.h"
/*00664418*/ extern struct CourseCounts g_course_counts_data;


/* current_course_id */

/* course id (add 1000 in mirrored mode: see current_course_id_with_mirror) */

int current_course_id(void)
{
  return g_course_counts_data.course_id;
}
