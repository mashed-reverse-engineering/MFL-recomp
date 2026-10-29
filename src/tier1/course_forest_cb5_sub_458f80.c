#include "mfl_tier2.h"
/*004a5da0*/ extern int object_destroy_frames();
/*006a2668*/ extern undefined4 g_data_6a2668;
/*006a266c*/ extern undefined4 g_data_6a266c;
/*006a2670*/ extern undefined4 g_data_6a2670;
/*006a2674*/ extern undefined4 g_data_6a2674;
/*006a2678*/ extern undefined4 g_data_6a2678;
/* course_forest_cb5_sub_458f80 */

/* single caller course_forest_cb5 */

void course_forest_cb5_sub_458f80(void)

{
  object_destroy_frames(g_data_6a2668);
  object_destroy_frames(g_data_6a2674);
  object_destroy_frames(g_data_6a2678);
  object_destroy_frames(g_data_6a266c);
  object_destroy_frames(g_data_6a2670);
  g_data_6a2668 = 0;
  g_data_6a2674 = 0;
  g_data_6a2678 = 0;
  g_data_6a266c = 0;
  g_data_6a2670 = 0;
  return;
}
