#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*006a1d88*/ extern undefined4 g_course_neustein_6a1d88;
/*006a1d8c*/ extern undefined4 g_course_neustein_6a1d8c;
/*006a1d90*/ extern undefined4 g_course_neustein_6a1d90;
/* course_neustein_cb2 */
void course_neustein_cb2(char *course)
{
  if (COURSE_CLUMP((CourseWorld *)course, 8) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 8));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 6) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 6));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 5) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 5));
  }
  if (COURSE_CLUMP((CourseWorld *)course, 9) != (RpClump *)0x0) {
    RpClumpRender(COURSE_CLUMP((CourseWorld *)course, 9));
  }
  func_0x00455060();
  object_render(g_course_neustein_6a1d88);
  object_render(g_course_neustein_6a1d8c);
  object_render(g_course_neustein_6a1d90);      /* tail call (the decompiler showed the inlined body of object_render) */
  return;
}
