#include "mfl_tier2.h"


/* particle_batch_set_size */

/* "stores a value into the current entry" */

void particle_batch_set_size(int *param_1,float param_2)

{
  *(float *)(param_1[2] * 0x30 + -0x18 + *param_1) = param_2;
  return;
}
