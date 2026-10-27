#include "mfl_tier2.h"
/* gap_5cae50 */

/* no context */

void gap_5cae50(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0xa0) = -*param_3;
  *(float *)(param_1 + 0xa4) = param_3[1];
  *(float *)(param_1 + 0xa8) = param_3[2];
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  return;
}


