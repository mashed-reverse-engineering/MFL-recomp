#include "mfl_tier2.h"
/*004c5860*/ extern int __cdecl FUN_004c5860(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
/* FUN_004c65d0 */
/* Thin wrapper: forwards its five arguments unchanged to FUN_004c5860 and
 * passes 0 for the last two (inferred: unused/default slots).
 * arg_1..arg_5 meaning unknown (inferred: not determinable from this function alone). */
void FUN_004c65d0(undefined4 arg_1,undefined4 arg_2,undefined4 arg_3,undefined4 arg_4, undefined4 arg_5)
{
  FUN_004c5860(arg_1,arg_2,arg_3,arg_4,arg_5,0,0);
  return;
}
