#include "mfl_tier2.h"
/* gap_5d8740 */

/* no context */

void gap_5d8740(char *param_1,char *unused_2,int *param_3)

{
  if (*param_3 != 0) {
    *(uint *)(param_1 + 0xcc) = *(uint *)(param_1 + 0xcc) | 8;
    return;
  }
  *(uint *)(param_1 + 0xcc) = *(uint *)(param_1 + 0xcc) & 0xfffffff7;
  return;
}


