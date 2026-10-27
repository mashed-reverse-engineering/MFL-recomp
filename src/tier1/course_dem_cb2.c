#include "mfl_tier2.h"
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a07bc*/ extern undefined4 g_course_dem_6a07bc;
/*007ef804*/ extern int g_tick_number;
/* course_dem_cb2 */
/* the original ends in tail jumps to RpClumpRender (both arms); the decompiler inlined the target as a list walk */
RpClump * course_dem_cb2(char *course)
{
  RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 4));
  if (g_course_dem_6a07bc != 0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 5));
  }
  RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 2));
  RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 1));
  RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 0));
  if (g_tick_number % 0x32 < 0x19) {
    return RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 7));
  }
  return RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 8));
}
