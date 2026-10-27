#include "mfl_tier2.h"


/* particle_batch_mark_current_active */

/* "increments the count and sets 1 in the current 0x30 byte entry" */

void particle_batch_mark_current_active(int *batch)
{
  batch[3] = batch[3] + 1;
  *(undefined4 *)(batch[2] * 0x30 + -0x14 + *batch) = 1;
  return;
}
