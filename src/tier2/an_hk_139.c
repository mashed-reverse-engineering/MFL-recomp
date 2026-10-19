#include "mfl_tier2.h"
/* FUN_005b9fa0
 * Calls the first `count` function pointers in the table `callbacks` (inferred), in order.
 * Returns 0 as soon as one of them returns 0, otherwise 1 (also 1 when `count` is 0, inferred).
 */

undefined4 __cdecl FUN_005b9fa0(char *callbacks,uint count)

{
  int result;
  uint index;

  index = 0;
  if (count != 0) {
    /* walk the table: call each entry and bail out on the first failure (inferred) */
    do {
      result = (**(int (**)())(callbacks + index * 4))();
      if (result == 0) {
        return 0;
      }
      index = index + 1;
    } while (index < count);
  }
  return 1;
}


