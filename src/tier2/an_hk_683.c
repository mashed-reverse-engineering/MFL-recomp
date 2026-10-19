#include "mfl_tier2.h"
/*00464d50*/ extern undefined __cdecl powerup_set_enabled(undefined4 p0, undefined4 p1);
#define POWERUP_COUNT (0x19)
/* powerups_set_all_enabled */

/* "powerup_set_enabled(i, flag) for all 25 powerups" */

/* Turns every powerup on or off: calls powerup_set_enabled(i, enabled) for each
   index i in [0, POWERUP_COUNT). Returns nothing. */
void powerups_set_all_enabled(undefined4 enabled)

{
  int current_index;
  int powerup_index;

  /* Loop over powerup indices 0..POWERUP_COUNT-1 (current_index is a copy of the index for this pass). */
  powerup_index = 0;
  do {
    current_index = (int)(powerup_index);
    powerup_set_enabled(powerup_index,enabled);
    powerup_index = current_index + 1;
  } while (powerup_index < POWERUP_COUNT);
  return;
}


