#include "mfl_tier2.h"
/* gap_570590 */

/* no context */

double gap_570590(char *param_1)

{
  if (*(float *)(param_1 + 0x40) <= *(float *)(param_1 + 0x44)) {
    return (double)*(float *)(param_1 + 0x40) + (double)*(float *)(param_1 + 0x40);
  }
  return (double)*(float *)(param_1 + 0x44) + (double)*(float *)(param_1 + 0x44);
}


