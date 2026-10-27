#include "mfl_tier2.h"
/*006c2680*/ extern undefined4 g_course_object_count;
/*006c2684*/ extern undefined4 g_course_object_pair_count;


/* course_object_pair_counts_clear */

/* "g_car_6c2680 = g_car_6c2684 = 0" */

void course_object_pair_counts_clear(void)
{
  g_course_object_count = 0;
  g_course_object_pair_count = 0;
  return;
}
