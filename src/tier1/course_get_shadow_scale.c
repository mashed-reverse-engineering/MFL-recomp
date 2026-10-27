#include "mfl_tier2.h"
/*00664418*/ extern struct CourseCounts g_course_counts_data;


/* course_get_shadow_scale */

/* "g_course_counts_data.shadow_scale" */

double course_get_shadow_scale(void)
{
  return (double)g_course_counts_data.shadow_scale;
}
