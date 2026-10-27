#include "mfl_tier2.h"

/* Offset of the ammo counter in the drum per-player state (inferred from the function name). */
#define DRUM_STATE_AMMO_OFFSET 8

/*005dd340*/ extern float k_f_0_5;


/* weapon_drum_ammo_fraction */

/* Drum ammo as a HUD fraction: ammo * 0.5. */
double weapon_drum_ammo_fraction(WeaponSlot *slot)
{
  return (double)*(int *)(slot->state + DRUM_STATE_AMMO_OFFSET) * (double)k_f_0_5;
}
