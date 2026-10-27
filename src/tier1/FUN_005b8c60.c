#include "mfl_tier2.h"
/* FUN_005b8c60 */
/* Returns the 32-bit word at offset 0xd74 of `obj` (inferred: a game object,
   the struct type has no definition in mfl_tier2.h yet). The other two parameters
   are never read. */
undefined4 __cdecl FUN_005b8c60(char *obj,char *unused_param_2,char *unused_param_3)
{
  return *(undefined4 *)(obj + 0xd74);
}
