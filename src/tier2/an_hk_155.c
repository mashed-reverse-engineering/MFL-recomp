#include "mfl_tier2.h"
/* FUN_005c1b00 */

/* Stores value in word 3 of state and sets bit 3 (mask 8) in word 0 (flags) of state.
   state: pointer to a block of uint words (inferred), value: new word 3 (inferred). */
void FUN_005c1b00(uint *state,uint value)

{
  state[3] = value;
  *state = *state | 8;
  return;
}


