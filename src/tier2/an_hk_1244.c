#include "mfl_tier2.h"
/* gap_5cabb0 */

/* no context */

void gap_5cabb0(char *param_1,char *unused_2,char *param_3)

{
  *(float *)(param_1 + 0x10c) = -*(float *)(param_3 + 0x10);
  *(undefined4 *)(param_1 + 0x110) = *(undefined4 *)(param_3 + 0x14);
  *(undefined4 *)(param_1 + 0x114) = *(undefined4 *)(param_3 + 0x18);
  *(float *)(param_1 + 0x100) = -*(float *)(param_3 + 0x20);
  *(undefined4 *)(param_1 + 0x104) = *(undefined4 *)(param_3 + 0x24);
  *(undefined4 *)(param_1 + 0x108) = *(undefined4 *)(param_3 + 0x28);
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 8;
  return;
}


