#include "mfl_tier2.h"
/*0045bad0*/ extern void __cdecl func_0x0045bad0(float p0);
/*008bf088*/ extern undefined4 g_course_sands_8bf088;


/* course_sands_cb1 */

void course_sands_cb1(char *param_1,uint param_2)

{
  undefined4 uVar2;
  
  uVar2 = func_0x00484b20(g_course_sands_8bf088,param_2 * 2);
  func_0x00484960(*(undefined4 *)(param_1 + 0xc114),uVar2);
  func_0x0045bad0((float)param_2);
  return;
}
