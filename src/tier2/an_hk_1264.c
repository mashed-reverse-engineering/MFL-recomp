#include "mfl_tier2.h"
/* gap_5caeb0 */

/* no context */

void gap_5caeb0(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0xac) = -*param_3;
  *(float *)(param_1 + 0xb0) = param_3[1];
  *(float *)(param_1 + 0xb4) = param_3[2];
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  return;
}


