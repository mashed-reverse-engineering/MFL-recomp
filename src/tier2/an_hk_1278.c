#include "mfl_tier2.h"
/* gap_5cb130 */

/* no context */

void gap_5cb130(char *param_1,char *unused_2,undefined4 *param_3)

{
  *(undefined4 *)(param_1 + 0x34) = *param_3;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 0x80;
  return;
}


