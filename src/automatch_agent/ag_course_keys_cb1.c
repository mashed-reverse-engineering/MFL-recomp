#include "mfl_tier2.h"
/*00449c40*/ extern void __cdecl func_0x00449c40(int p0, float p1);
/*004757a0*/ extern int __cdecl get_viewport_camera_matrix(int p0);
/*004757e0*/ extern float __cdecl func_0x004757e0(int p0);
/*008c5724*/ extern undefined4 g_course_keys_8c5724;


/* course_keys_cb1 */

void course_keys_cb1(char *param_1,int param_2)

{
  undefined4 camera_matrix;
  
  camera_matrix = func_0x00484b20(g_course_keys_8c5724,param_2 * 2);
  func_0x00484960(*(undefined4 *)(param_1 + 0xc114),camera_matrix);
  func_0x00449c40(get_viewport_camera_matrix(0),func_0x004757e0(0));
  return;
}
