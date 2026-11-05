/* rwa_soft_wave_def_create_005da590 (0x005da590): software mixer: create the wave definition of the software mixer */
#include "mfl_tier2.h"

/*005ba5c0*/ extern void *FUN_005ba5c0(int a, const void *uuid, int b);      /* RwaObjDefFindByUUID */
/*005bd230*/ extern char *FUN_005bd230(void *objdef, void *wave_def);      /* RwaWaveDefCreate */
/*005bc030*/ extern void FUN_005bc030(void *wave_def, const void *uuid, int flags);      /* RwaWaveDefAssignID */
/*005bcf60*/ extern void FUN_005bcf60(void *wave_def, const void *formats, int num_formats);      /* RwaWaveDefAssignSupportedFormats */
/*005da4d0*/ extern void *rwa_soft_wave_alloc_data_005da4d0(char *wave, int owner);
/*005da500*/ extern void gap_5da500(void);      /* free the sample block */
/*005da520*/ extern void gap_5da520(void);      /* copy sample bytes into the block (+2 guard samples) */
/*005bb430*/ extern void gap_5bb430(void);

/*005f8f6c*/ extern undefined g_rwa_soft_mixer_uuid;      /* the software mixer object definition */
/*005f8f8c*/ extern undefined g_rwa_soft_wave_uuid;
/*007dc450*/ extern undefined g_rwa_soft_wave_def;
/*0064c710*/ extern undefined g_rwa_soft_wave_formats;      /* 2 supported formats */

/* Returns the definition, or NULL when it could not be created.  +0x0c..+0x18 = the wave methods. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_soft_wave_def_create_005da590(void)
{
  char *def = FUN_005bd230(FUN_005ba5c0(0, &g_rwa_soft_mixer_uuid, 0), &g_rwa_soft_wave_def);

  if (!def)
    return 0;
  FUN_005bc030(def, &g_rwa_soft_wave_uuid, 0);
  *(void **)(def + 0x0c) = (void *)rwa_soft_wave_alloc_data_005da4d0;
  *(void **)(def + 0x10) = (void *)gap_5da500;
  *(void **)(def + 0x14) = (void *)gap_5da520;
  *(void **)(def + 0x18) = (void *)gap_5bb430;
  FUN_005bcf60(def, &g_rwa_soft_wave_formats, 2);
  return def;
}
