#include "mfl_tier2.h"
/* course_warzone_cb1_sub_45d720 */

/* single caller course_warzone_cb1 */

void course_warzone_cb1_sub_45d720(int param_1,int param_2,undefined4 param_3)

{
  if (0 < param_2) {
    do {
      course_warzone_cb1_sub_45d630(param_1,param_3);
      param_1 = param_1 + 0xc;
      param_2 = param_2 + -1;
    } while (param_2 != 0);
  }
  return;
}


