#include "mfl_tier2.h"
/*00462980*/ extern int weapon_effects_clear_6a7c78();
/* Resets the oil weapon state by clearing all weapon effects (inferred from the name:
   the body only calls weapon_effects_clear_6a7c78 and ignores its return value).
   Takes no parameters and returns nothing. */
void weapon_oil_reset_all(void)
{
  weapon_effects_clear_6a7c78();
}
