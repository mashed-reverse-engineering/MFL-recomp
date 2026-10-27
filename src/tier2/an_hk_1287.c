#include "mfl_tier2.h"
/* gap_5ccb30 */

/* no context */

char *gap_5ccb30(char *param_1,char *unused_2,undefined4 *param_3)

{
  int iVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)(param_1 + 0x44);
  for (iVar1 = 0xc; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  *(uint *)(param_1 + 0x78) = *(uint *)(param_1 + 0x78) | 0x80;
  return param_1;
}


