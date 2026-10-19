#include "mfl_tier2.h"

/* IEEE-754 bit pattern of 1.0f */
#define FLOAT_ONE_BITS (0x3f800000)

/* FUN_005c19b0 */

/* Resets a 6-dword state block to its default values (inferred):
 *   [0] = 0, [1] = 1, [2] = 0, [3] = 0, [4] = 1.0f, [5] = 0.
 * state: pointer to the 6-dword block (inferred). No return value. */
void FUN_005c19b0(undefined4 *state)

{
  *state = 0;
  state[1] = 1;
  state[2] = 0;
  state[3] = 0;
  state[4] = FLOAT_ONE_BITS;
  state[5] = 0;
  return;
}


