#include "mfl_tier2.h"
/*0049c120*/ extern int fx_buffers_free();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a5e38*/ extern undefined4 g_weapon_rocket_6a5e38;
/*006a6008*/ extern undefined4 g_weapon_6a6008;
/*006a6054*/ extern char g_weapon_rocket_6a6054[];


/* weapon_rocket_free_assets */

/* Frees the rocket weapon clumps and fx buffers. */

void weapon_rocket_free_assets(void)
{
  undefined4 *fx_slot;      /* stride 0x1b words from g_weapon_rocket_6a5e38; fx buffer at slot[0], clump at slot[-3] */
  undefined4 *clump_slot;   /* stride 0xb words from g_weapon_6a6008; clump pointer at slot[0] */

  clump_slot = &g_weapon_6a6008;
  fx_slot = &g_weapon_rocket_6a5e38;
  do {
    RpClumpDestroy((RpClump *)*clump_slot);
    RpClumpDestroy((RpClump *)fx_slot[-3]);
    fx_buffers_free(*fx_slot);
    fx_slot = fx_slot + 0x1b;
    clump_slot = clump_slot + 0xb;
  } while ((int)fx_slot < ((int)&g_weapon_rocket_6a6054));   /* end of the table */
  return;
}
