#include "mfl_tier2.h"
/*00421a30*/ extern int __cdecl course_call_cb9(char *p0);
/*00668c18*/ extern struct CourseWorld g_course_world;
/*00421b50*/ extern int course_has_cb9();
/* course_hook9: runs the cb9 callback with the course world when course_has_cb9()
   reports it is present. param_1 is unused (inferred). Returns nothing. */

/* "cb9 when present" */

void __cdecl course_hook9(char *param_1)

{
  int has_cb9;

  has_cb9 = course_has_cb9();
  /* cb9 is only called when the course provides one */
  if (has_cb9 != 0) {
    course_call_cb9((char *)&g_course_world);
    return;
  }
  return;
}


