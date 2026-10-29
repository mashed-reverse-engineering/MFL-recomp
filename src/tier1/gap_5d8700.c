#include "mfl_tier2.h"
/* gap_5d8700 */

/* no context */

char *gap_5d8700(char *param_1,char *unused_2,undefined4 *param_3)

{
  undefined4 uVar1;
  
  uVar1 = *param_3;
  *(undefined4 *)(param_1 + 0x88) = 0;
  *(undefined4 *)(param_1 + 0x8c) = uVar1;
  return param_1;
}
