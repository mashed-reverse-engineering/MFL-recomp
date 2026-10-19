#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
/*006a2794*/ extern float g_course_superg_6a2794;
extern float func_0x004757e0();
/* course_superg_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Course Super G callback (inferred from callees): stores a float from func_0x004757e0(0)
 * in g_course_superg_6a2794, then passes that float's bit pattern as the second argument to
 * get_viewport_camera_matrix(0, ...) and hands the result to func_0x00449c40.
 * Parameters: none. Returns nothing. */
void course_superg_cb1(void)

{
  float10 value;

  value = (float10)func_0x004757e0(0);
  g_course_superg_6a2794 = (float)value;
  func_0x00449c40(get_viewport_camera_matrix(0),*(int *)&g_course_superg_6a2794);
  return;
}


