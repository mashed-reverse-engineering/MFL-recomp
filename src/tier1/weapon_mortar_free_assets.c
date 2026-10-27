#include "mfl_tier2.h"
/*0049bd90*/ extern int fx_system_destroy();
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a29cc*/ extern undefined4 g_weapon_6a29cc;
/*006a2a80*/ extern undefined4 g_weapon_6a2a80;
/*006a4c80*/ extern char g_explosion_queue_2__frames_left[];
/*006a2a3c*/ extern char g_weapon_6a2a3c[];


/* weapon_mortar_free_assets */

/* Frees the mortar weapon clumps and fx systems. */

void weapon_mortar_free_assets(void)
{
  undefined4 *slot;

  /* first table, stride 7 words from g_weapon_6a29cc: clump pointer at slot[0] */
  slot = &g_weapon_6a29cc;
  do {
    RpClumpDestroy((RpClump *)*slot);
    slot = slot + 7;
  } while ((int)slot < ((int)&g_weapon_6a2a3c));
  /* second table, stride 0x44 words from g_weapon_6a2a80: clump at slot[-4], fx system at slot[0] */
  slot = &g_weapon_6a2a80;
  do {
    RpClumpDestroy((RpClump *)slot[-4]);
    fx_system_destroy(*slot);
    slot = slot + 0x44;
  } while ((int)slot < ((int)&g_explosion_queue_2__frames_left));
  return;
}
