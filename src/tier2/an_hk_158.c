#include "mfl_tier2.h"
/* FUN_005c1e60 */

/* Calls the function pointer in call_args[1] with call_args[0] and call_args[2] as its two arguments.
   call_args is a 3-slot table: arg, callback, arg (layout inferred). Returns nothing. */
void FUN_005c1e60(undefined4 *call_args)

{
  (*(code *)call_args[1])(*call_args,call_args[2]);
  return;
}


