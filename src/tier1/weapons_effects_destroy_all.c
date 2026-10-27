#include "mfl_tier2.h"
extern char PTR_weapon_machinegun_free_assets_0060dc38[];
/*00405220*/ extern int crown_destroy();
/*004145b0*/ extern int trail_clumps_destroy();
/*00464580*/ extern int powerups_destroy();
/*00465200*/ extern int weapon_effects_destroy_6a9458();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*0050b5e0*/ extern int nop_stub();
/*0060de60*/ extern undefined4 weapon_descriptor_count;
/*006aaf08*/ extern undefined4 g_particles_6aaf08;
/* Pointer-sized slots per weapon in PTR_weapon_machinegun_free_assets (inferred) */
#define WEAPON_ASSET_STRIDE (0x10)
/* weapons_effects_destroy_all */
/* "trail_clumps_destroy, powerups_destroy, effects destroy, crown_destroy ..." */
/*
 * Tears down the effect systems (trail clumps, powerups, weapon effects, crown)
 * and calls each weapon's free-assets callback, then destroys the particle
 * texture dictionary. Takes no parameters and returns nothing.
 */
void weapons_effects_destroy_all(void)
{
  int weapon_index;
  undefined **free_assets_slot;

  trail_clumps_destroy();
  nop_stub();
  powerups_destroy();
  weapon_effects_destroy_6a9458();
  crown_destroy();
  /* Call the free-assets callback of every weapon descriptor that has one (inferred) */
  weapon_index = 0;
  if (0 < (int)weapon_descriptor_count) {
    free_assets_slot = &PTR_weapon_machinegun_free_assets_0060dc38;
    do {
      if ((code *)*free_assets_slot != (code *)0x0) {
        (*(code *)*free_assets_slot)();
      }
      weapon_index = weapon_index + 1;
      free_assets_slot = free_assets_slot + WEAPON_ASSET_STRIDE;
    } while (weapon_index < (int)weapon_descriptor_count);
  }
  RwTexDictionaryDestroy_004d5ac0(g_particles_6aaf08);
  return;
}
