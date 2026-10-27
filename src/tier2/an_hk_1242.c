#include "mfl_tier2.h"
/* gap_5cab50 */

/* no context */

void gap_5cab50(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0xf4) = -*param_3;
  *(float *)(param_1 + 0xf8) = param_3[1];
  *(float *)(param_1 + 0xfc) = param_3[2];
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  return;
}


