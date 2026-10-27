#include "mfl_tier2.h"
/* gap_4aec80 */

/* no context */

undefined4 __stdcall gap_4aec80(char *param_1, undefined4 *param_2)

{
  if (param_2 == (undefined4 *)0x0) {
    return 0x80004003;
  }
  *param_2 = *(undefined4 *)(param_1 + 0x1c);
  param_2[1] = *(undefined4 *)(param_1 + 0x20);
  param_2[2] = *(undefined4 *)(param_1 + 0x24);
  param_2[3] = *(undefined4 *)(param_1 + 0x28);
  return 0;
}


