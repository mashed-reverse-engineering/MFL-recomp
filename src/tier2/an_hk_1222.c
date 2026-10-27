#include "mfl_tier2.h"
/* gap_5c8ca0 */

/* no context */

void gap_5c8ca0(char *param_1,char *unused_2,int *param_3)

{
  if (*param_3 != 0) {
    *(uint *)(param_1 + 0x1e4) = *(uint *)(param_1 + 0x1e4) | 0x4000;
    return;
  }
  *(uint *)(param_1 + 0x1e4) = *(uint *)(param_1 + 0x1e4) & 0xffffbfff;
  return;
}


