#include "mfl_tier2.h"
/* course_warzone_cb0_sub_4c4830 */

/* single caller course_warzone_cb0_sub_4c4860 */

uint * course_warzone_cb0_sub_4c4830(undefined4 param_1)

{
  uint *puVar1;
  
  puVar1 = (uint *)course_warzone_cb0_sub_4c4800(param_1);
  *puVar1 = *puVar1 | 0x3000;
  func_0x00544380(puVar1);
  return puVar1;
}
