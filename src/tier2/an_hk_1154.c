#include "mfl_tier2.h"
/* gap_5b8840 */

/* no context */

void gap_5b8840(char *param_1,int param_2)

{
  int *piVar1;
  int *piVar2;
  
  piVar1 = (int *)(param_1 + 0x14);
  if (*(int *)(param_1 + 0x14) != 0) {
    **(int **)(param_1 + 0x18) = *(int *)(param_1 + 0x14);
    *(undefined4 *)(*piVar1 + 4) = *(undefined4 *)(param_1 + 0x18);
  }
  piVar2 = (int *)(param_2 + 0xc);
  *piVar1 = *(int *)(param_2 + 0xc);
  *(int **)(param_1 + 0x18) = piVar2;
  *(int **)(*piVar2 + 4) = piVar1;
  *piVar2 = (int)piVar1;
  *(int *)(param_1 + 0x24) = param_2;
  return;
}


