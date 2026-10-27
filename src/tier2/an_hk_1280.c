#include "mfl_tier2.h"
/* gap_5cb160 */

/* no context */

char *gap_5cb160(char *param_1,char *unused_2,uint *param_3)

{
  if (*(int *)(param_1 + 0x84) != 0) {
    *param_3 = ~*(uint *)(*(char **)(param_1 + 0x84) + 0x54) >> 1 & 1;
    return param_1;
  }
  *param_3 = 0;
  return param_1;
}


