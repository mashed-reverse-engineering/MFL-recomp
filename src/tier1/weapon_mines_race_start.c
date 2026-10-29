#include "mfl_tier2.h"
/*00464060*/ extern int weapon_effects_clear_6a8af8();
/* Clears all active weapon effects before a mines race starts (inferred from the callee name). Takes no parameters and returns nothing. */
/*
 * Clears all active weapon effects before a mines race starts (inferred from the
 * callee name). Takes no parameters and returns nothing.
 */
void weapon_mines_race_start(void)
{
  weapon_effects_clear_6a8af8();
  return;
}
