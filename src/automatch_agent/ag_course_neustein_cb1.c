#include "mfl_tier2.h"
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0048e180*/ extern int ptank_lock_all();
/*006a1d94*/ extern float g_course_neustein_6a1d94;


/* course_neustein_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void course_neustein_cb1(int param_1, int param_2)

{
  extern float func_0x004757e0(int);
  extern int get_viewport_camera_matrix(int);
  extern int func_0x00449c40(int, float);
  extern int get_viewport_camera(int);
  extern int ptank_lock_all(int);
  extern int func_0x00454970(undefined4);

  g_course_neustein_6a1d94 = func_0x004757e0(0);
  func_0x00449c40(get_viewport_camera_matrix(0), g_course_neustein_6a1d94);
  ptank_lock_all(get_viewport_camera(0));
  func_0x00454970(param_1);
  return;
}
