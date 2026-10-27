#include "mfl_tier2.h"
/* gap_5d85b0 */

/* no context */

char *gap_5d85b0(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x4c) = *param_3;
  *(undefined4 *)(param_1 + 0x50) = param_3[1];
  *(undefined4 *)(param_1 + 0x54) = param_3[2];
  return param_1;
}


