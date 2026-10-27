#include "mfl_tier2.h"
/* course_warzone_cb1_sub_45da00 */

/* single caller course_warzone_cb1_sub_45da60 */

void course_warzone_cb1_sub_45da00(int *param_1,char *param_2)

{
  int *piVar1;
  int iVar2;
  
  iVar2 = 0;
  if (0 < *param_1) {
    piVar1 = (int *)(param_1[1] + 0x24);
    do {
      if (*piVar1 == 0) {
        piVar1[-3] = *(int *)(param_2 + 0x30);
        piVar1[-2] = *(int *)(param_2 + 0x34);
        piVar1[-1] = *(int *)(param_2 + 0x38);
        *piVar1 = 100;
        piVar1[2] = 0x3f800000;
      }
      piVar1 = piVar1 + 0xc;
      iVar2 = iVar2 + 1;
    } while (iVar2 < *param_1);
  }
  return;
}


