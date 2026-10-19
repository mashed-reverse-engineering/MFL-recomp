#include "mfl_tier2.h"
/*004574a0*/ extern int course_spline_objects_update();
/*004577c0*/ extern int course_tierra_n_rotating_object_update();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0050b5e0*/ extern int nop_stub(char *p0, undefined4 p1);
/* course_tierra_n_cb1 */

void course_tierra_n_cb1(char *param_1,undefined4 param_2)

{
  course_spline_objects_update(param_2);
  course_tierra_n_rotating_object_update(param_2);
  get_viewport_camera_matrix(0);
  nop_stub(param_1,param_2);
  return;
}


