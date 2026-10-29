#include "mfl_tier2.h"
/* Initialises a six-word object: sets word 2 to `value`, updates the flags in word 0 (clears bit 3, sets bit 2), zeroes words 3 and 5, sets word 4 to float 1.0 and word 1 to 1 (inferred: ... */
/* FUN_005c1ad0
 * Initialises a six-word object: sets word 2 to `value`, updates the flags
 * in word 0 (clears bit 3, sets bit 2), zeroes words 3 and 5, sets word 4
 * to float 1.0 and word 1 to 1 (inferred: state/enabled marker).
 * Parameters: obj - pointer to the object's uint words, value - stored in obj[2].
 * Returns nothing. */
void FUN_005c1ad0(uint *obj,uint value)
{
  /* store the caller's value in word 2 (inferred) */
  obj[2] = value;
  /* clear flag bit 3, set flag bit 2 (inferred) */
  *obj = *obj & 0xfffffff7 | 4;
  obj[3] = 0;
  obj[4] = FLOAT_ONE_BITS;
  obj[1] = 1;
  obj[5] = 0;
  return;
}
