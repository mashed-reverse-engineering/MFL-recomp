#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*006a1860*/ extern undefined4 g_course_car_6a1860;
/* course_highway_cb2_sub_4541a0 */

/* single caller course_highway_cb2 */

void course_highway_cb2_sub_4541a0(void)

{
  particles_render(g_course_car_6a1860);
  return;
}


