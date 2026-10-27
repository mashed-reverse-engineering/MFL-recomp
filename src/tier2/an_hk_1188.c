#include "mfl_tier2.h"
/* gap_5bb400 */

/* no context */

char *gap_5bb400(char *param_1,int param_2,undefined4 *param_3,uint param_4)

{
  uint uVar1;
  undefined4 *puVar2;
  
  puVar2 = (undefined4 *)(*(char **)(param_1 + 0x4c) + param_2);
  for (uVar1 = param_4 >> 2; uVar1 != 0; uVar1 = uVar1 - 1) {
    *puVar2 = *param_3;
    param_3 = param_3 + 1;
    puVar2 = puVar2 + 1;
  }
  for (param_4 = param_4 & 3; param_4 != 0; param_4 = param_4 - 1) {
    *(undefined1 *)puVar2 = *(undefined1 *)param_3;
    param_3 = (undefined4 *)((char *)param_3 + 1);
    puVar2 = (undefined4 *)((char *)puVar2 + 1);
  }
  return param_1;
}


