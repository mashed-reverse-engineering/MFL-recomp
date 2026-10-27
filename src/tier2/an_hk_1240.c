#include "mfl_tier2.h"
/* gap_5caaf0 */

/* no context */

void gap_5caaf0(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0xe8) = -*param_3;
  *(float *)(param_1 + 0xec) = param_3[1];
  *(float *)(param_1 + 0xf0) = param_3[2];
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  return;
}


