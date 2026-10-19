#include "mfl_tier2.h"
/* FUN_005cf210
 * Zeroes a block of five 32-bit words (20 bytes) starting at block.
 * block: pointer to the first word of the block to clear.
 * Returns nothing. (inferred: a 5-word state/record reset)
 */

void FUN_005cf210(undefined4 *block)

{
  *block = 0;
  block[1] = 0;
  block[2] = 0;
  block[3] = 0;
  block[4] = 0;
  return;
}


