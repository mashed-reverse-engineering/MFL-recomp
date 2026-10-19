#include "mfl_tier2.h"
/* FUN_005cd810 */

/* Zeroes the two 32-bit words at pair[0] and pair[1] (inferred: resets a two-word record).
   pair: pointer to the first of the two words. No return value. */
void FUN_005cd810(undefined4 *pair)

{
  *pair = 0;
  pair[1] = 0;
  return;
}


