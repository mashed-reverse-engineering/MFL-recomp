#include "mfl_tier2.h"
/*005de19c*/ extern float k_f_1_0en4;
/* gap_5caf80 */

/* no context */

void gap_5caf80(char *param_1,char *unused_2,float *param_3)

{
  *(float *)(param_1 + 0xd0) = *param_3 - *param_3 * k_f_1_0en4;
  *(float *)(param_1 + 0xd4) = *param_3;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  return;
}


