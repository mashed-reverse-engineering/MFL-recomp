#include "mfl_tier2.h"
/*00421a20*/ extern int __cdecl course_call_cb8(char *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00421b40*/ extern int course_has_cb8();
/* course_hook8 */

/* "cb8 when present" */

/* Course hook: if course_has_cb8() reports a cb8 callback is present, calls it with the
 * course world (g_course_world). Does nothing otherwise. Returns nothing.
 * param_1 is never read (inferred: unused by this hook). */
void __cdecl course_hook8(char *param_1)

{
  int has_cb8;

  has_cb8 = course_has_cb8();
  if (has_cb8 != 0) {
    course_call_cb8((char *)&g_course_world);
    return;
  }
  return;
}


