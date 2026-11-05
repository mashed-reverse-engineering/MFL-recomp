/* rwa_dxaudio_wave_free_data_005ccdf0 (0x005ccdf0): free the sample storage of a DXAudio wave (software or DirectSound) */
#include "mfl_tier2.h"

/*005da4a0*/ extern int thunk_RwaWaveDestroy_5da4a0(void *soft_data);      /* software mixer: free the sample block */
/*005cd2f0*/ extern int FUN_005cd2f0(void **rec);      /* _rwaDSBufferDestroy */

/* wave+0x58 -> wave definition, +0x20 -> its format; format byte +0x50 bit 3 = the wave lives in the software mixer
   (block pointer at wave+0x4c), otherwise it owns a DirectSound buffer record at wave+0x5c. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_dxaudio_wave_free_data_005ccdf0(char *wave)
{
  char *format = *(char **)(*(char **)(wave + 0x58) + 0x20);

  if (format[0x50] & 8)
    return thunk_RwaWaveDestroy_5da4a0(*(void **)(wave + 0x4c));
  return FUN_005cd2f0((void **)(wave + 0x5c));
}
