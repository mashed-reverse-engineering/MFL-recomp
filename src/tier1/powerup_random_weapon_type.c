#include "mfl_tier2.h"
typedef int WeaponId;
/*00463310*/ extern int weapon_effects_any_active();
/*00482010*/ extern int random_int_range();
/* powerup_random_weapon_type */
/* "picks a random weapon id from {0xC, 0x13, 0x12, 0x11, 0x10, 0xB, 0xA, 9, 7}" */
/* Purpose: pick the weapon a power-up box gives out. Returns a random WeaponId from weapon_pool.
   If any weapon effect is already active (weapon_effects_any_active() != 0), the pool is
   reduced to 8 entries without WEAPON_OIL (inferred: stops a second oil/slick from stacking). */
WeaponId powerup_random_weapon_type(void)
{
  int pool_index;
  int effects_active;
  WeaponId weapon_pool [9];

  /* full pool of 9 weapons, index picked uniformly by random_int_range(0, 8) */
  weapon_pool[0] = WEAPON_MINE;
  weapon_pool[1] = WEAPON_OIL;
  weapon_pool[2] = WEAPON_FLARE;
  weapon_pool[3] = WEAPON_SHOTGUN;
  weapon_pool[4] = WEAPON_FLAMETHROWER;
  weapon_pool[5] = WEAPON_MISSILE;
  weapon_pool[6] = WEAPON_DEPTHCHARGE;
  weapon_pool[7] = WEAPON_MACHINEGUN;
  weapon_pool[8] = WEAPON_MORTAR;
  pool_index = random_int_range(0,8);
  effects_active = weapon_effects_any_active();
  if (effects_active != 0) {
    /* effects already running: rebuild the pool without WEAPON_OIL, index 0..7 */
    weapon_pool[0] = WEAPON_MINE;
    weapon_pool[1] = WEAPON_FLARE;
    weapon_pool[2] = WEAPON_SHOTGUN;
    weapon_pool[3] = WEAPON_FLAMETHROWER;
    weapon_pool[4] = WEAPON_MISSILE;
    weapon_pool[5] = WEAPON_DEPTHCHARGE;
    weapon_pool[6] = WEAPON_MACHINEGUN;
    weapon_pool[7] = WEAPON_MORTAR;
    pool_index = random_int_range(0,7);
    return weapon_pool[pool_index];
  }
  return weapon_pool[pool_index];
}
