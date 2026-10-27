#include "mfl_tier2.h"
/*004c4230*/ extern int __cdecl FUN_004c4230(int a1, int a2);
/* FUN_004c4270
 * Thin wrapper: forwards its argument to FUN_004c4230 with a second argument of 0.
 * value: the single parameter, passed through unchanged as FUN_004c4230's first argument
 * (meaning unknown; the callee's parameters are unnamed in the dossier).
 */
void FUN_004c4270(undefined4 value)
{
  FUN_004c4230(value,0);
  return;
}
