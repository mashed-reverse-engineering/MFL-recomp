#include "mfl_tier2.h"

/* Size in bytes of one player's record in g_weapon_mortar_6a29c4. */
#define MORTAR_PLAYER_RECORD_SIZE 0x1c

/*005ddc1c*/ extern float k_f_0_33333334;
/*006a29c4*/ extern undefined g_weapon_mortar_6a29c4;


/* weapon_mortar_ammo_fraction */

/* Mortar ammo as a HUD fraction: first int of the slot's player record * 1/3. */
double weapon_mortar_ammo_fraction(WeaponSlot *slot)
{
  return (double)*(int *)(&g_weapon_mortar_6a29c4 + slot->player * MORTAR_PLAYER_RECORD_SIZE) *
         (double)k_f_0_33333334;
}
