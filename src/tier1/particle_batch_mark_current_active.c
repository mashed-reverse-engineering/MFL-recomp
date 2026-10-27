#include "mfl_tier2.h"


/* particle_batch_mark_current_active */

/* "increments the count and sets 1 in the current 0x30 byte entry" */

void particle_batch_mark_current_active(int *param_1)
{
  param_1[3] = param_1[3] + 1;
  *(undefined4 *)(param_1[2] * 0x30 + -0x14 + *param_1) = 1;
  return;
}
