/* rwa_virtual_voice_set_play_005b6ce0 (0x005b6ce0): virtual voice parameter callback (play state): nonzero starts the voice (time stamp), zero stops it */
#include "mfl_tier2.h"

/*005b7eb0*/ extern void rwa_voicemanager_005b7eb0(void *voice, int op);      /* voice control: 0 play, 1 stop */
/*007dab90*/ extern RwInt32 g_data_7dab90;      /* the time stamp (ms) */

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
char *rwa_virtual_voice_set_play_005b6ce0(char *obj, RwUInt32 param, const RwUInt32 *value)
{
  if (*value) {
    rwa_voicemanager_005b7eb0(obj + 0x28, 0);
    *(RwInt32 *)(obj + 0x54) = g_data_7dab90;
    return obj;
  }
  rwa_voicemanager_005b7eb0(obj + 0x28, 1);
  return obj;
}
