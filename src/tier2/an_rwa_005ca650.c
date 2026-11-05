/* rwa_dxaudio_voice_set_trigger_005ca650 (0x005ca650): _rwaDXAudioVoiceSetTrigger, start / stop a DXAudio voice */
#include "mfl_tier2.h"

/*005d84f0*/ extern void FUN_005d84f0(void *soft_voice, int trigger);      /* software mixer voice: set trigger */

#define DXVOICE_TRIGGER 0x200      /* voice+0x28 flags */
#define DXVOICE_DIRTY   0x100

/* voice+0x94 -> wave format; byte +0x50 bit 3 = software mixer voice (its mixer voice at +0x11c gets the trigger too). */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
void rwa_dxaudio_voice_set_trigger_005ca650(char *voice, int trigger)
{
  unsigned flags = *(unsigned *)(voice + 0x28);

  if (trigger)
    flags |= DXVOICE_TRIGGER;
  else
    flags &= ~DXVOICE_TRIGGER;
  *(unsigned *)(voice + 0x28) = flags | DXVOICE_DIRTY;
  if ((*(char **)(voice + 0x94))[0x50] & 8)
    FUN_005d84f0(*(void **)(voice + 0x11c), trigger);
}
