#include "mfl_tier2.h"
/* FUN_005d2ba0 */
/* Returns true when none of the flag bits 0x44 are set in the uint at
   offset 0x20 of obj (flag meaning inferred: obj's status flags). */
bool __stdcall FUN_005d2ba0(char *obj)
{
  return (*(uint *)(obj + 0x20) & 0x44) == 0;
}
