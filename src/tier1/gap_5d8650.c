#include "mfl_tier2.h"
/* gap_5d8650 */

/* no context */

void gap_5d8650(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x34) = *param_3;
  *(uint *)(param_1 + 0xcc) = *(uint *)(param_1 + 0xcc) | 4;
  return;
}
