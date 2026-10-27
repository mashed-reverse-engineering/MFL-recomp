#include "mfl_tier2.h"
/*005bf2b0*/ extern int __cdecl FUN_005bf2b0(int a1);
/* gap_5caa20 */

/* no context */

int gap_5caa20(int param_1,char *unused_2,undefined4 *param_3)

{
  undefined4 uVar1;
  
  uVar1 = FUN_005bf2b0(*param_3);
  *(undefined4 *)(param_1 + 0xcc) = uVar1;
  *(uint *)(param_1 + 0x28) = *(uint *)(param_1 + 0x28) | 4;
  return param_1;
}


