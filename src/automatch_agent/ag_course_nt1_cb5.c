#include "mfl_tier2.h"
extern undefined4 g_course_nt1_6a1f2c;
/*004558e0*/ extern int course_object_destroy_frames_6a1f28();
/*00484ad0*/ extern int pool_list_destroy();
/*004a5da0*/ extern int object_destroy_frames();
/*0050b5e0*/ extern int nop_stub();
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;


/* course_nt1_cb5 */

void course_nt1_cb5(undefined4 param_1)

{
  nop_stub();
  pool_list_destroy(g_course_nt1_8c5720);
  course_object_destroy_frames_6a1f28(param_1);
  object_destroy_frames(g_course_nt1_6a1f2c);
  return;
}
