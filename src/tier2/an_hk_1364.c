#include "mfl_tier2.h"
/* gap_5cf5f0 */

/* no context */

undefined4 __stdcall gap_5cf5f0(char *param_1, undefined4 *param_2, undefined4 *param_3)

{
  if ((param_2 != (undefined4 *)0x0) && (param_3 != (undefined4 *)0x0)) {
    *param_2 = *(undefined4 *)(param_1 + 0x14);
    *param_3 = *(undefined4 *)(param_1 + 0xc);
    return 0;
  }
  return 0x80004003;
}


