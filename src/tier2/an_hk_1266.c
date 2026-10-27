#include "mfl_tier2.h"
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddc28*/ extern float k_f_0;
/* gap_5caf10 */

/* no context */

void gap_5caf10(char *param_1,char *unused_2,float *param_3)

{
  if (*param_3 <= k_f_0) {
    *(float *)(param_1 + 0x98) = k_f_1_0en5;
    *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
    return;
  }
  *(float *)(param_1 + 0x98) = *param_3;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  return;
}


