#include "mfl_tier2.h"

/* Offset of the ammo counter in the per-player weapon state (field ammo). */
#define SHOTGUN_STATE_AMMO_OFFSET 0x10

/*005ddc1c*/ extern float k_f_0_33333334;


/* weapon_shotgun_ammo_fraction */

/* Shotgun ammo as a HUD fraction: ammo * 1/3. */
double weapon_shotgun_ammo_fraction(WeaponSlot *slot)
{
  return (double)*(int *)(slot->state + SHOTGUN_STATE_AMMO_OFFSET) * (double)k_f_0_33333334;
}
