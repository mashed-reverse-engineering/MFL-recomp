#include "mfl_tier2.h"
#define FLAMETHROWER_CLUMP_INDEX 4
#define FLAMETHROWER_SLOT_STRIDE 0x1a
#define WEAPON_FLAMETHROWER_ARRAY_END ((int)&g_weapon_flamethrower_6a9738)

/*0049bd90*/ extern int fx_system_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a9598*/ extern undefined4 g_weapon_flamethrower_6a9598;
/*006a9738*/ extern char g_weapon_flamethrower_6a9738[];


/* Frees clump models and FX systems for all flamethrower weapon slots. */

void weapon_flamethrower_free_assets(void)
{
  undefined4 *flamethrower_slot;
  
  flamethrower_slot = &g_weapon_flamethrower_6a9598;
  do {
    /* Destroy 3D model and particle FX system for this slot */
    RpClumpDestroy((RpClump *)flamethrower_slot[FLAMETHROWER_CLUMP_INDEX]);
    fx_system_destroy(*flamethrower_slot);
    /* Advance to next slot; remaining fields in the 0x1a-dword slot are unknown */
    flamethrower_slot = flamethrower_slot + FLAMETHROWER_SLOT_STRIDE;
  } while ((int)flamethrower_slot < WEAPON_FLAMETHROWER_ARRAY_END);
  return;
}
