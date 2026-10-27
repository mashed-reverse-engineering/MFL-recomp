#include "mfl_tier2.h"

#define WEAPON_EFFECT_RECORD_CLUMP_INDEX 7
#define WEAPON_EFFECT_RECORD_STRIDE 0x16
#define WEAPON_EFFECT_RECORDS_END ((int)&g_load_6a95b8)

/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*004d4e40*/ extern RwBool RwMatrixDestroy(RwMatrix *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*006a93e0*/ extern undefined g_particles_6a93e0;
/*006a9458*/ extern undefined4 g_weapon_6a9458;
/*006a9580*/ extern undefined4 g_particles_6a9580;
/*006a95b8*/ extern char g_load_6a95b8[];


/* weapon_effects_destroy_6a9458 */

/* Destroys per-slot weapon effect clumps and matrices, then tears down particle system and textures. */

void weapon_effects_destroy_6a9458(void)
{
  int *effect_record;
  
  /* Destroy clump and matrix for each 0x58-byte weapon effect slot record */
  effect_record = &g_weapon_6a9458;
  do {
    RpClumpDestroy((RpClump *)effect_record[WEAPON_EFFECT_RECORD_CLUMP_INDEX]);
    RwMatrixDestroy((RwMatrix *)*effect_record);
    effect_record = effect_record + WEAPON_EFFECT_RECORD_STRIDE;
  } while ((int)effect_record < WEAPON_EFFECT_RECORDS_END);
  particles_destroy(&g_particles_6a93e0);
  /* Destroy particle texture dictionary if loaded */
  if (g_particles_6a9580 != (RwTexDictionary *)0x0) {
    RwTexDictionaryDestroy_004d5ac0(g_particles_6a9580);
  }
  return;
}
