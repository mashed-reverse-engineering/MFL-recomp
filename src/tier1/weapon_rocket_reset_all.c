#include "mfl_tier2.h"
/*00460970*/ extern int weapon_effects_clear_6a5dd0();
/* Resets rocket weapon state by clearing all weapon effects via weapon_effects_clear_6a5dd0 (inferred from the callee name). Takes no parameters, returns nothing. */
/* weapon_rocket_reset_all: resets rocket weapon state by clearing all weapon effects via
 * weapon_effects_clear_6a5dd0 (inferred from the callee name). Takes no parameters, returns nothing.
 */
void weapon_rocket_reset_all(void)
{
  weapon_effects_clear_6a5dd0();
  return;
}
