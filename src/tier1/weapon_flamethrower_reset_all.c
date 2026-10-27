#include "mfl_tier2.h"
#define WEAPON_FX_GRID_SIZE_DWORDS 0x1a

/*006a9588*/ extern undefined g_weapon_6a9588;
/*006a9728*/ extern undefined4 g_weapon_6a9728;


/* weapon_flamethrower_reset_all */
/* Resets visual effect grids for all flamethrowers. */

void weapon_flamethrower_reset_all(void)
{
  undefined4 *fx_grid;
  
  fx_grid = &g_weapon_6a9728;
  /* Iterate backwards through each effect grid in the array and clear it */
  do {
    fx_grid = fx_grid + -WEAPON_FX_GRID_SIZE_DWORDS;
    weapon_fx_grid_clear(fx_grid);
  } while (fx_grid != (undefined4 *)&g_weapon_6a9588);
  return;
}
