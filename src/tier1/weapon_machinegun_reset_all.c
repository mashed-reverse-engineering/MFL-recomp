#include "mfl_tier2.h"
/*004628c0*/ extern int machinegun_states_clear();
/* Resets all machinegun weapon state by calling machinegun_states_clear(). */
/* Resets all machinegun weapon state by calling machinegun_states_clear().
   Takes no parameters and returns nothing. (inferred from the callee name) */
void weapon_machinegun_reset_all(void)
{
  machinegun_states_clear();
}
