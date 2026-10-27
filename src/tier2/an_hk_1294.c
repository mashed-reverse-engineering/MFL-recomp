#include "mfl_tier2.h"
/*005cee90*/ extern int FUN_005cee90();
/* gap_5cee60 */

/* no context */

void gap_5cee60(undefined4 param_1,char *param_2)

{
  gap_5c1e30(param_1,0,param_2);
  *(code **)(*(char **)(param_2 + 4) + 4) = FUN_005cee90;
  *(undefined4 *)(*(char **)(param_2 + 4) + 8) = param_1;
  return;
}


