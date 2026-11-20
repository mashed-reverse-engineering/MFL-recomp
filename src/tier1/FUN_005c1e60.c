#include "mfl_tier2.h"
/* FUN_005c1e60 */
/* Calls the function pointer in call_args[1] with call_args[0] and call_args[2] as its two arguments.
   call_args is a 3-slot table: arg, callback, arg (layout inferred). Returns the callback's result
   (RwaStreamConnectionGetPacket: 0x5c813e stores it, 0x5ceb03 pushes it; VOID_w16: was void). */
typedef undefined4 (*FUN_005c1e60_cb)(undefined4, undefined4);
undefined4 FUN_005c1e60(undefined4 *call_args)
{
  return ((FUN_005c1e60_cb)call_args[1])(*call_args,call_args[2]);
}
