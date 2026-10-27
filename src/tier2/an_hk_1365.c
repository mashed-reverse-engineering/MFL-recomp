#include "mfl_tier2.h"
/* gap_5d0000 */

/* no context */

undefined4 __stdcall gap_5d0000(char *param_1, undefined4 *param_2)

{
  undefined4 *puVar1;
  
  puVar1 = *(undefined4 **)(param_1 + 0x10);
  if (puVar1 != (undefined4 *)0x0) {
    *param_2 = *puVar1;
    param_2[1] = puVar1[1];
    param_2[2] = puVar1[2];
    param_2[3] = puVar1[3];
    return 0;
  }
  return 0x80004005;
}


