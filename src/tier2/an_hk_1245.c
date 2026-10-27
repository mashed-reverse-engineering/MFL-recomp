#include "mfl_tier2.h"
/* gap_5cac00 */

/* no context */

char *gap_5cac00(char *param_1,char *unused_2,char *param_3)

{
  *(float *)(param_3 + 0x10) = -*(float *)(param_1 + 0x10c);
  *(undefined4 *)(param_3 + 0x14) = *(undefined4 *)(param_1 + 0x110);
  *(undefined4 *)(param_3 + 0x18) = *(undefined4 *)(param_1 + 0x114);
  *(float *)(param_3 + 0x20) = -*(float *)(param_1 + 0x100);
  *(undefined4 *)(param_3 + 0x24) = *(undefined4 *)(param_1 + 0x104);
  *(undefined4 *)(param_3 + 0x28) = *(undefined4 *)(param_1 + 0x108);
  return param_1;
}


