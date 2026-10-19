#include "mfl_tier2.h"
/*004757a0*/ extern int get_viewport_camera_matrix();
extern float func_0x004757e0();
/* course_angels_cb1
 * Camera setup for the Angels course (inferred from the name: course callback 1).
 * Takes no parameters. Gets a float from func_0x004757e0(0) (inferred: a viewport scale),
 * passes its raw bits to get_viewport_camera_matrix(0, ...) and hands the returned
 * camera matrix to func_0x00449c40. Returns nothing.
 */

extern void func_0x00449c40(int, float);

void course_angels_cb1(void)

{
  func_0x00449c40(get_viewport_camera_matrix(0), func_0x004757e0(0));
  return;
}


