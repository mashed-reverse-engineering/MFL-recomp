#include "mfl_tier2.h"
/* gap_5c8bf0 */

/* no context */

void gap_5c8bf0(char *param_1,uint *param_2)

{
  int *piVar1;
  uint uVar2;
  
  piVar1 = (int *)(*(char **)(param_1 + 0x1a4) + *(int *)(param_1 + 0x1ac) * 0x14);
  if (param_2[1] == 1) {
    uVar2 = *(uint *)(*piVar1 + 4);
  }
  else {
    if (param_2[1] != 2) {
      param_2[1] = 3;
      *param_2 = (uint)(*piVar1 - *(int *)(*(char **)(param_1 + 0x174) + 0x10)) >> 5;
      goto LAB_005c8c43;
    }
    uVar2 = *(uint *)*piVar1;
  }
  *param_2 = uVar2;
LAB_005c8c43:
  param_2[3] = 2;
  uVar2 = gap_5c3020(param_1 + 0x5c,piVar1,0);
  param_2[2] = uVar2;
  return;
}


