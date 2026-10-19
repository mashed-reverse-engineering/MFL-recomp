#include "mfl_tier2.h"
/*004601d0*/ extern int weapon_effects_clear_6a5c60();
/* weapon_drum_race_start */

/* Start of the drum race: clears any active weapon effects (inferred from the
   callee name). Takes no parameters and returns nothing. */
void weapon_drum_race_start(void)

{
  weapon_effects_clear_6a5c60();
  return;
}


