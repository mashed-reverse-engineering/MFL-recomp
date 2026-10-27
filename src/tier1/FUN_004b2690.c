#include "mfl_tier2.h"
/* FUN_004b2690 */
/* Returns the 32-bit field at offset 8 of `object` (inferred), or 0 if `object` is NULL.
   Parameter: object - pointer to a game object whose field at +8 is read (inferred). */
undefined4 __stdcall FUN_004b2690(char *object)
{
  if (object == 0) {
    return 0;
  }
  return *(undefined4 *)(object + 8);
}
