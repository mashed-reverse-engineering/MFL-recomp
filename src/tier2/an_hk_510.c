#include "mfl_tier2.h"
/*004a5da0*/ extern int object_destroy_frames();
/*006a2150*/ extern undefined4 g_course_city_6a2150;
/* course_city_cb5 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

/* Course-specific teardown for the City course (inferred from the name: course_city, callback 5).
 * Takes no parameters and returns nothing.
 * 1. Calls func_0x00455aa0() (unnamed, purpose unknown).
 * 2. Passes g_course_city_6a2150 to object_destroy_frames(), which destroys the frame objects
 *    held in that global (inferred from the callee name). */
void course_city_cb5(void)

{
  func_0x00455aa0();
  object_destroy_frames(g_course_city_6a2150);
  return;
}


