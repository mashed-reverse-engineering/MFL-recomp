#include "mfl_tier2.h"
/*00421a10*/ extern undefined __cdecl course_call_cb7(void *p0);
/*00421b30*/ extern int course_has_cb7();
/*00492e00*/ extern int rwp_objects_reset_all();
/*00668c18*/ extern struct CourseWorld g_course_world;


/* course_hook7 */

/* Invokes course callback 7 with the global course world if present, then resets all RWP objects. */

void course_hook7(void)
{
  int has_cb7;
  
  has_cb7 = course_has_cb7();
  if (has_cb7 != 0) {
    course_call_cb7(&g_course_world);
    rwp_objects_reset_all();
    return;
  }
  return;
}
