#include "mfl_tier2.h"
/* gap_5d2c00 */

/* no context */

undefined4 __stdcall gap_5d2c00(char *param_1, undefined4 *param_2, undefined4 *param_3)

{
  if ((*(byte *)(param_1 + 0x14) & 2) != 0) {
    *param_2 = *(undefined4 *)(param_1 + 0x48);
    param_2[1] = *(undefined4 *)(param_1 + 0x4c);
    *param_3 = *(undefined4 *)(param_1 + 0x50);
    param_3[1] = *(undefined4 *)(param_1 + 0x54);
    return 0;
  }
  return 0x80040251;
}


