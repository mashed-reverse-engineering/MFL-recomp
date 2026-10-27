#include "mfl_tier2.h"
/*00421a00*/ extern int __cdecl course_call_cb6(char *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00421b20*/ extern int course_has_cb6();
/* course_hook6 */
/* "cb6 when present" */
/*
 * Calls course callback 6 with the course world as its argument, but only when
 * course_has_cb6() reports that the callback is present. Returns nothing.
 * param_1 is not used by the body (inferred: hook signature only).
 */
void __cdecl course_hook6(char *param_1)
{
  int has_cb6;

  /* Only run the callback if the course provides one. */
  has_cb6 = course_has_cb6();
  if (has_cb6 != 0) {
    course_call_cb6((char *)&g_course_world);
    return;
  }
  return;
}
