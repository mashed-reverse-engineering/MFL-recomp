/* rwa_wave_def_assign_supported_formats_005bcf60 (0x005bcf60): wave definitions (RwaWaveDefAssignSupportedFormats): replace the definition's supported format table (freeing an owned old one) */
#include "mfl_tier2.h"

/*005bf120*/ extern void FUN_005bf120(void *block);      /* RWA free */

#define WAVEDEF_NUM_FORMATS(d) (*(int *)((d) + 0x24))
#define WAVEDEF_FORMATS(d) (*(const void **)((d) + 0x28))
#define WAVEDEF_W2C(d) (*(RwUInt32 *)((d) + 0x2c))
#define WAVEDEF_FLAGS(d) (*(RwUInt32 *)((d) + 0x48))      /* 1: the format table is owned (allocated) */

/* TIER2: unvalidated 2026-11-06 authority=diffemu */
const void *rwa_wave_def_assign_supported_formats_005bcf60(char *waveDef, const void *formats, int numFormats)
{
  if (WAVEDEF_FLAGS(waveDef) & 1) {
    if (WAVEDEF_FORMATS(waveDef))
      FUN_005bf120((void *)WAVEDEF_FORMATS(waveDef));
    WAVEDEF_FLAGS(waveDef) &= ~1u;
  }
  WAVEDEF_FORMATS(waveDef) = formats;
  WAVEDEF_NUM_FORMATS(waveDef) = numFormats;
  WAVEDEF_W2C(waveDef) = 0;
  return formats;      /* the original leaves FORMATS in eax */
}
