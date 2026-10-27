#include "mfl_tier2.h"
/* gap_5d85f0 */

/* no context */

char *gap_5d85f0(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x58) = *param_3;
  *(undefined4 *)(param_1 + 0x5c) = param_3[1];
  *(undefined4 *)(param_1 + 0x60) = param_3[2];
  return param_1;
}


