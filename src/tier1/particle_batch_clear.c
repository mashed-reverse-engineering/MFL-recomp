#include "mfl_tier2.h"


/* particle_batch_clear */

/* "zeroes fields +8, +0xC, +0x54" */

void particle_batch_clear(char *param_1)
{
  *(undefined4 *)(param_1 + 0x54) = 0;
  *(undefined4 *)(param_1 + 8) = 0;
  *(undefined4 *)(param_1 + 0xc) = 0;
  return;
}
