#include "mfl_tier2.h"

#define WEAPON_MINES_UNKNOWN_TABLE_1_STRIDE 6
#define WEAPON_MINES_UNKNOWN_TABLE_1_END    ((int)&g_weapon_6a8b6c)
#define WEAPON_MINES_UNKNOWN_TABLE_2_STRIDE 0x1b
#define WEAPON_MINES_UNKNOWN_TABLE_2_END    ((int)&g_weapon_mines_6a8b10)

/*00486710*/ extern int particles_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a7d90*/ extern undefined4 g_weapon_6a7d90;
/*006a8ab8*/ extern undefined g_weapon_6a8ab8;
/*006a8b0c*/ extern undefined4 g_weapon_6a8b0c;
/*006a8b58*/ extern undefined4 g_weapon_6a8b58;
/*006a8b5c*/ extern undefined4 g_weapon_6a8b5c;
/*006a8b10*/ extern char g_weapon_mines_6a8b10[];
/*006a8b6c*/ extern char g_weapon_6a8b6c[];


/* Frees 3D clump models and particle assets for the mine weapon. */

void weapon_mines_free_assets(void)
{
  undefined4 *clump_ptr;
  
  /* Destroy clumps in unknown mine asset table */
  clump_ptr = &g_weapon_6a8b0c;
  do {
    RpClumpDestroy((RpClump *)*clump_ptr);
    clump_ptr = clump_ptr + WEAPON_MINES_UNKNOWN_TABLE_1_STRIDE;
  } while ((int)clump_ptr < WEAPON_MINES_UNKNOWN_TABLE_1_END);
  /* Destroy clumps in unknown mine entity/asset table */
  clump_ptr = &g_weapon_6a7d90;
  do {
    RpClumpDestroy((RpClump *)*clump_ptr);
    clump_ptr = clump_ptr + WEAPON_MINES_UNKNOWN_TABLE_2_STRIDE;
  } while ((int)clump_ptr < WEAPON_MINES_UNKNOWN_TABLE_2_END);
  /* Destroy remaining unknown mine clumps and particle system */
  RpClumpDestroy(g_weapon_6a8b5c);
  RpClumpDestroy(g_weapon_6a8b58);
  particles_destroy(&g_weapon_6a8ab8);
  return;
}
