#include "mfl_tier2.h"
/* gap_5c1b10 */

/* no context */

undefined4 gap_5c1b10(uint *param_1,uint param_2,byte *param_3)

{
  if ((*param_3 & 1) != 0) {
    return 0;
  }
  *param_1 = *param_1 | 0x20;
  param_1[4] = param_2;
  return 1;
}


