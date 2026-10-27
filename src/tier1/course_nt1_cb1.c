#include "mfl_tier2.h"
/*008c5720*/ extern undefined4 g_course_nt1_8c5720;


/* course_nt1_cb1 */

void course_nt1_cb1(char *param_1,int param_2)
{
  undefined4 uVar1;
  
  uVar1 = func_0x00484b20(g_course_nt1_8c5720,param_2 * 2);
  func_0x00484960(*(undefined4 *)(param_1 + 0xc118),uVar1);
  func_0x004557f0(param_1);
  return;
}
