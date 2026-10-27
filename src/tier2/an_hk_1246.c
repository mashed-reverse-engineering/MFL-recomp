#include "mfl_tier2.h"
/* gap_5cac50 */

/* no context */

void gap_5cac50(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0x100) = -*param_3;
  *(float *)(param_1 + 0x104) = param_3[1];
  *(float *)(param_1 + 0x108) = param_3[2];
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  return;
}


