#include "mfl_tier2.h"
/*005cfc80*/ extern int __cdecl FUN_005cfc80(int a1, int a2, int a3);
/* gap_5d01c0 */

/* no context */

int __stdcall gap_5d01c0(undefined4 param_1, int *param_2)

{
  int iVar1;
  
  iVar1 = FUN_005cfc80(param_1,0,0);
  *param_2 = iVar1;
  return (-(uint)(iVar1 != 0) & 0x7ff8fff2) + 0x8007000e;
}


