#include "mfl_tier2.h"
/*00421a50*/ extern int __cdecl course_call_cb11(char *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00421b70*/ extern int course_has_cb11();
/* course_hook11 */

/* "cb11 when present" */

/*
 * Course hook 11: if course_has_cb11() reports the callback is present,
 * call course_call_cb11() with the address of g_course_world.
 * param_1 is unused (inferred).
 */
void __cdecl course_hook11(char *param_1)

{
  int has_cb11;

  /* Check whether the course provides the cb11 callback. */
  has_cb11 = course_has_cb11();
  if (has_cb11 != 0) {
    course_call_cb11((char *)&g_course_world);
    return;
  }
  return;
}


