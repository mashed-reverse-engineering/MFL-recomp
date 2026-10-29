#include "mfl_tier2.h"
/* gap_5bab10 */

/* no context */

undefined4 gap_5bab10(int *param_1,int *param_2)

{
  if ((*param_1 == *param_2) && ((param_1[7] & 4U) != 0)) {
    param_2[1] = param_1[7];
    return 0;
  }
  return 1;
}
