#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*006187f8*/ extern undefined4 g_sfx_table;

#define SFX_TABLE_STRIDE (0x25) /* g_sfx_table entries per effect (inferred) */
#define VOICE_FLAG_NO_PLAY (2) /* flags bit that skips playback (inferred) */
#define VOICE_OFF_RATE (0x3c) /* voice record: playback rate (inferred) */
#define VOICE_DEFAULT_RATE_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */

/* sfx_play_voice_at */

/* "(effect, position*, voice index): same for a single voice of an effect"
 * Plays one voice of a sound effect at a position (inferred).
 * effect_id: effect index; g_sfx_table gives the first voice slot of that effect.
 * position: float[3] position, copied into the voice record.
 * voice_index: voice number within the effect.
 * Does nothing unless get_app_state() == 3 and the voice is not flagged no-play. */

void sfx_play_voice_at(int effect_id,undefined4 *position,int voice_index)

{
  char *voice_rec;
  int state_or_handle;
  undefined4 set_param_result;

  state_or_handle = get_app_state();
  if (state_or_handle == APP_STATE_RACE) {
    /* voice_index becomes the absolute slot in g_voices */
    voice_index = (&g_sfx_table)[effect_id * SFX_TABLE_STRIDE] + voice_index;
    voice_rec = g_voices[voice_index].voice;
    if ((*(byte *)(voice_rec + VOICE_OFF_FLAGS) & VOICE_FLAG_NO_PLAY) == 0) {
      /* store the position in the voice record */
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_X) = *position;
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = position[1];
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = position[2];
      /* notify the handle if there is one, then start playback */
      state_or_handle = *(int *)(g_voices[voice_index].voice + VOICE_OFF_HANDLE);
      if (state_or_handle != 0) {
        FUN_005b72a0(state_or_handle,0,1,position);
      }
      FUN_005b6a40(g_voices[voice_index].voice,1);
      /* set the default playback rate, then the handle's parameter */
      *(undefined4 *)(g_voices[voice_index].voice + VOICE_OFF_RATE) = VOICE_DEFAULT_FREQ_BITS;
      state_or_handle = *(int *)(g_voices[voice_index].voice + VOICE_OFF_HANDLE);
      set_param_result = 0;
      if (state_or_handle != 0) {
        set_param_result = FUN_005b7300(state_or_handle,4,1,VOICE_DEFAULT_RATE_F);
      }
      voice_set_volume(voice_index,1.0);
    }
  }
  return;
}


