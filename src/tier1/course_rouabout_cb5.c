#include "mfl_tier2.h"
/*0049c880*/ extern int SUB_0049c880();
/*004a5da0*/ extern int object_destroy_frames();
/*004575a0*/ extern int func_0x004575a0();
/*006a262c*/ extern undefined4 g_course_rouabout_6a262c;
/*006a2630*/ extern undefined4 g_course_rouabout_6a2630;

/* Tears down the Rouabout course: destroys two object frames, then runs the shared teardown (tail call). */
int course_rouabout_cb5(void)
{
  object_destroy_frames(g_course_rouabout_6a2630);
  object_destroy_frames(g_course_rouabout_6a262c);
  SUB_0049c880(0);
  return func_0x004575a0();
}
