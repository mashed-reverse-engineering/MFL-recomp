#include "mfl_tier2.h"


/* particle_batch_clear */

/* "zeroes fields +8, +0xC, +0x54" */

void particle_batch_clear(char *batch)
{
  *(undefined4 *)(batch + 0x54) = 0;
  *(undefined4 *)(batch + 8) = 0;
  *(undefined4 *)(batch + 0xc) = 0;
  return;
}
