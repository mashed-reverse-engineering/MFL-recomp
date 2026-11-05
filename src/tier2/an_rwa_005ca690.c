/* rwa_dxaudio_voice_get_playing_005ca690 (0x005ca690): DirectSound voices (_rwaDXAudioVoiceGetPlaying): is the voice playing */
#include "mfl_tier2.h"

/*005cd3e0*/ extern int _rwaDSBufferIsPlaying(void *buffer);

#define VOICE_STATE_KNOWN 0x100      /* voice+0x28: bit 9 holds the answer */
#define VOICE_PLAYING     0x200
#define OUTPUT_SOFTWARE   0x08       /* output+0x50 */
#define SOFT_STARTING 2              /* soft voice+0x28 */
#define SOFT_PLAYING  4

/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwa_dxaudio_voice_get_playing_005ca690(char *voice)
{
  RwUInt32 flags = *(RwUInt32 *)(voice + 0x28);

  if (flags & VOICE_STATE_KNOWN)
    return (flags >> 9) & 1;
  if (*(unsigned char *)(*(char **)(voice + 0x94) + 0x50) & OUTPUT_SOFTWARE) {
    int state;

    if (!*(void **)(voice + 0x84))
      return 0;
    state = *(int *)(*(char **)(voice + 0x11c) + 0x28);      /* the software mixer's voice */
    return state == SOFT_STARTING || state == SOFT_PLAYING;
  }
  if (!*(void **)(voice + 0x84))
    return 0;
  return _rwaDSBufferIsPlaying(voice + 0xdc) != 0;
}
