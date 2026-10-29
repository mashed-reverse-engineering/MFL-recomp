#include "mfl_tier2.h"
/*004a5d70*/ extern int object_render();
/*006a07c4*/ extern undefined4 g_course_6a07c4;
/*006a07c8*/ extern undefined4 g_course_6a07c8;
/*006a07cc*/ extern undefined4 g_course_6a07cc;
/*006a07d0*/ extern undefined4 g_course_6a07d0;
/* course_dump_cb2_sub_453560 */

/* single caller course_dump_cb2 */

void course_dump_cb2_sub_453560(void)

{
  object_render(g_course_6a07c4);
  object_render(g_course_6a07c8);
  object_render(g_course_6a07cc);
  object_render(g_course_6a07d0);
  return;
}
