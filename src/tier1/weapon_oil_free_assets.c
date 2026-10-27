#include "mfl_tier2.h"

#define WEAPON_OIL_CLUMP_STRIDE 4
#define WEAPON_OIL_CLUMPS_END ((int)&g_weapon_6a7cc4)

/*00486710*/ extern int particles_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a7c38*/ extern undefined g_weapon_6a7c38;
/*006a7c84*/ extern undefined4 g_weapon_6a7c84;
/*006a7ce8*/ extern undefined g_weapon_6a7ce8;
/*006a7cc4*/ extern char g_weapon_6a7cc4[];


/* weapon_oil_free_assets */
/* Releases clump models and particle systems allocated for the oil weapon. */

void weapon_oil_free_assets(void)
{
  undefined4 *clump_ptr;
  
  /* Destroy clump assets (unknown 16-byte record structure, 4 entries) */
  clump_ptr = &g_weapon_6a7c84;
  do {
    RpClumpDestroy((RpClump *)*clump_ptr);
    clump_ptr = clump_ptr + WEAPON_OIL_CLUMP_STRIDE;
  } while ((int)clump_ptr < WEAPON_OIL_CLUMPS_END);
  particles_destroy(&g_weapon_6a7ce8);
  particles_destroy(&g_weapon_6a7c38);
  return;
}
