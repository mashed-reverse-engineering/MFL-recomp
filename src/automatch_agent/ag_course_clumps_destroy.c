#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a26d0*/ extern undefined4 g_course_clumps_6a26d0;
/*006a26d4*/ extern undefined4 g_course_clumps_6a26d4;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2730*/ extern undefined4 g_course_clumps_6a2730;
/*006a2734*/ extern undefined4 g_course_clumps_6a2734;


/* course_clumps_destroy */

/* "destroys the 5 clumps g_course_6a26cc.." */

void course_clumps_destroy(undefined4 param_1)

{
  RpClumpDestroy(g_course_clumps_6a26cc);
  RpClumpDestroy(g_course_clumps_6a26d0);
  RpClumpDestroy(g_course_clumps_6a26d4);
  RpClumpDestroy(g_course_clumps_6a272c);
  RpClumpDestroy(g_course_clumps_6a2730);
  RpClumpDestroy(g_course_clumps_6a2734);
  return;
}
