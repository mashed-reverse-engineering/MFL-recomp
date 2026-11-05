/* rwa_soft_voice_get_position_005d8530 (0x005d8530): software mixer voice: the play position the mixer has reached */
#include "mfl_tier2.h"

/* voice+0x8c = sample position, advanced by the mixer thread (the DXAudio voice copies it into its own +0x88). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_soft_voice_get_position_005d8530(char *voice)
{
  return *(int *)(voice + 0x8c);
}
