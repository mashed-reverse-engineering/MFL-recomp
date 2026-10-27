#include "mfl_tier2.h"
#define FLASHBANG_ENTRY_STRIDE 5
#define FLASHBANG_ENTRIES_END ((int)&g_weapon_flashbang_6a5dd8)

/*00486710*/ extern int particles_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a5d88*/ extern undefined4 g_weapon_6a5d88;
/*006a5dc8*/ extern undefined4 g_weapon_6a5dc8;
/*008bf000*/ extern undefined g_weapon_8bf000;
/*006a5dd8*/ extern char g_weapon_flashbang_6a5dd8[];


/* Free clump models and particle assets for the flashbang weapon. */

void weapon_flashbang_free_assets(void)
{
  undefined4 *clump_ptr;
  
  clump_ptr = &g_weapon_6a5d88;
  /* Destroy clump models for each flashbang entry (unknown layout) */
  do {
    RpClumpDestroy((RpClump *)*clump_ptr);
    clump_ptr = clump_ptr + FLASHBANG_ENTRY_STRIDE;
  } while ((int)clump_ptr < FLASHBANG_ENTRIES_END);
  /* Destroy additional flashbang clump model (unknown) */
  RpClumpDestroy(g_weapon_6a5dc8);
  particles_destroy(&g_weapon_8bf000);
  return;
}
