#include "mfl_tier2.h"
extern undefined4 g_course_sands_6a2780;
/*0045b730*/ extern int course_clumps_destroy();
/*00484ad0*/ extern int pool_list_destroy();
/*004a5da0*/ extern int object_destroy_frames();
/*008bf088*/ extern undefined4 g_course_sands_8bf088;


/* course_sands_cb5 */

void course_sands_cb5(undefined4 param_1)

{
  pool_list_destroy(g_course_sands_8bf088);
  course_clumps_destroy(param_1);
  object_destroy_frames(g_course_sands_6a2780);
  return;
}
