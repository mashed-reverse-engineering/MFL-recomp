#include "mfl_tier2.h"
/*00484660*/ extern undefined __cdecl clump_set_flag_on_atomics(struct RpClump *p0, undefined4 p1);
/*0049bdd0*/ extern int fx_system_render();
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*005ddc28*/ extern float k_f_0;
/*006a29d0*/ extern undefined4 g_weapon_6a29d0[0x1000];
/*006a2a70*/ extern undefined4 g_weapon_6a2a70[0x1000];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*006a4c70*/ extern char g_explosion_queue_2__damage[];
/*006a2a40*/ extern char g_weapon_mortar_6a2a40[];
/* Offsets in the g_weapon_6a29d0 entries (word index from the cursor; entries are 7 words apart). */
#define EFFECT_ACTIVE_WORD (-4)
#define EFFECT_THRESHOLD_WORD (-2)
/* Byte offset in the effect object of the update function pointer (inferred). */
#define EFFECT_UPDATE_FN_OFFSET (0x48)
/* Word offsets in the g_weapon_6a2a70 entries (entries are 0x44 words apart). */
#define CLUMP_FX_ACTIVE_WORD (-0x11)
#define CLUMP_FX_PARAM_WORD (4)
/* weapon_mortar_clear_effects
 * Per-frame pass over the two weapon effect tables (inferred from the table names).
 * Takes no parameters and returns nothing.
 * (was: "weapon_mortar_clear_effects" - title only) */
void weapon_mortar_clear_effects(void)
{
  int *effect_entry;
  undefined4 *clump_entry;

  /* Table 1 (g_weapon_6a29d0, 7-word entries, up to g_weapon_mortar_6a2a40):
     for each active entry whose threshold float is above k_f_0 (inferred: age or timer vs. limit),
     call the update function pointer at object + EFFECT_UPDATE_FN_OFFSET with the object. */
  effect_entry = g_weapon_6a29d0;
  do {
    if ((effect_entry[EFFECT_ACTIVE_WORD] != 0) && (*(float *)&effect_entry[EFFECT_THRESHOLD_WORD] > k_f_0)) {
      (**(code **)(*effect_entry + EFFECT_UPDATE_FN_OFFSET))(*effect_entry);
    }
    effect_entry = effect_entry + 7;
  } while ((int)effect_entry < ((int)&g_weapon_mortar_6a2a40));
  /* Table 2 (g_weapon_6a2a70, 0x44-word entries, up to g_explosion_queue_2__damage):
     for each active entry, draw its clump with z-write off, then fx_system_render(entry[CLUMP_FX_PARAM_WORD]),
     then restore z-write on. */
  clump_entry = g_weapon_6a2a70;
  do {
    if (clump_entry[CLUMP_FX_ACTIVE_WORD] != 0) {
      clump_set_flag_on_atomics((RpClump *)*clump_entry, 0x1);
      RpClumpRender((RpClump *)*clump_entry);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)false);
      fx_system_render(clump_entry[CLUMP_FX_PARAM_WORD]);
      RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)true);
    }
    clump_entry = clump_entry + 0x44;
  } while ((int)clump_entry < ((int)&g_explosion_queue_2__damage));
  return;
}
