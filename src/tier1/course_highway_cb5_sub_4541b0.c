#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006a1860*/ extern undefined4 g_course_car_6a1860;
/* course_highway_cb5_sub_4541b0 */

/* single caller course_highway_cb5 */

void course_highway_cb5_sub_4541b0(void)

{
  particles_destroy(g_course_car_6a1860);
  g_course_car_6a1860 = 0;
  return;
}
