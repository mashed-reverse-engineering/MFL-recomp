#include "mfl_tier2.h"

/* Size in bytes of one player's record in g_weapon_6a5ff8. */
#define ROCKET_PLAYER_RECORD_SIZE 0x2c

/*006a5ff8*/ extern undefined g_weapon_6a5ff8;


/* weapon_rocket_ammo_fraction */

/* Rocket ammo of the slot's player: first int of that player's record (no scaling). */
double weapon_rocket_ammo_fraction(WeaponSlot *slot)
{
  return (double)*(int *)(&g_weapon_6a5ff8 + slot->player * ROCKET_PLAYER_RECORD_SIZE);
}
