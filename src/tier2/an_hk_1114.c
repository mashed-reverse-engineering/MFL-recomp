#include "mfl_tier2.h"
/* gap_4c4200 */

/* no context */

undefined4 gap_4c4200(undefined4 param_1,int *param_2)

{
  int *piVar1;
  
  piVar1 = (int *)param_2[1];
  if (*param_2 != 0) {
    *(undefined4 *)(*param_2 + *piVar1 * 4) = param_1;
    *piVar1 = *piVar1 + 1;
    return param_1;
  }
  *piVar1 = *piVar1 + 1;
  return param_1;
}


