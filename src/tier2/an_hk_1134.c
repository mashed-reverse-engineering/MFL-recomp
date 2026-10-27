#include "mfl_tier2.h"
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/* gap_5b6b20 */

/* no context */

int gap_5b6b20(int param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x58) = *param_3;
  *(undefined4 *)(param_1 + 0x5c) = param_3[1];
  *(undefined4 *)(param_1 + 0x60) = param_3[2];
  if (*(int *)(param_1 + 0x30) != 0) {
    FUN_005b72a0(*(int *)(param_1 + 0x30),0,1,param_3);
  }
  return param_1;
}


