#include "mfl_tier2.h"
/*005baf80*/ extern void FUN_005baf80(void *p0, undefined4 p1, undefined4 p2);
/* gap_5bfeb0 */

/* no context */

void gap_5bfeb0(char *param_1,char *param_2)

{
  **(undefined4 **)(param_1 + 0x30) = *(undefined4 *)(param_1 + 0x2c);
  *(undefined4 *)(*(char **)(param_1 + 0x2c) + 4) = *(undefined4 *)(param_1 + 0x30);
  FUN_005baf80(param_1,*(undefined4 *)(param_2 + 0x54),*(undefined4 *)(param_2 + 0x58));
  return;
}
