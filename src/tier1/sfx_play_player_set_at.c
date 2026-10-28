#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int sound_voice_set_enabled(void *p0, undefined4 p1);
/*005b72a0*/ extern int sound_voice_call_entry(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int sound_voice_set_float_param(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005de238*/ extern float k_f_2000;
/*005df99c*/ extern float k_f_22050;
/*00618c98*/ extern undefined4 g_sound_618c98;
#define VOICE_FLAG_IN_USE (2) /* (inferred) */
#define VOICE_PITCH_OFF (0x3c)
#define VOICE_FULL_VOLUME (1.0)
#define SFX_SET_VOICE_COUNT (4)
/* sfx_play_player_set_at */
/* "(?, position*): starts four voices of the set at 0x618C98 + 4 with a pitch ramp (inferred)" */
/* Starts up to four sound voices of the set at g_sound_618c98 + 4 at a world position (inferred).
 * position: float[3] (x, y, z), copied into each voice record that gets started.
 * Each started voice also gets the position on its emitter (+0x30), a pitch from the
 * remaining count, and full volume. Returns nothing. */
void __cdecl sfx_play_player_set_at(undefined4 *position)
{
  char *voice;
  int emitter;
  float pitch;
  int voices_left;
  undefined4 rate_result;
  int slot_index;

  /* Only runs when the app state is 3 (inferred gate). */
  voices_left = get_app_state();
  if (voices_left == APP_STATE_RACE) {
    slot_index = g_sound_618c98 + 4;
    voices_left = SFX_SET_VOICE_COUNT;
    /* Walk the set's slots. voices_left counts the slots still to try; it is
     * set to 0 after the first free voice, so the loop stops after one start. */
    do {
      voice = g_voices[slot_index].voice;
      /* Free voice (IN_USE flag clear): start it. voices_left is reused here to
       * hold the emitter handle until the set-rate call. */
      if ((*(byte *)(voice + VOICE_OFF_FLAGS) & VOICE_FLAG_IN_USE) == 0) {
        *(RwV3d *)(voice + VOICE_OFF_POS_X) = *(RwV3d *)position;
        emitter = *(int *)(g_voices[slot_index].voice + VOICE_OFF_HANDLE);
        if (emitter != 0) {
          sound_voice_call_entry(emitter,0,1,position);
        }
        sound_voice_set_enabled(g_voices[slot_index].voice,1);
        pitch = (float)voices_left * k_f_2000 + k_f_22050;
        *(float *)(g_voices[slot_index].voice + VOICE_PITCH_OFF) = pitch;
        voices_left = *(int *)(g_voices[slot_index].voice + VOICE_OFF_HANDLE);
        rate_result = 0;
        if (voices_left != 0) {
          rate_result = sound_voice_set_float_param(voices_left,4,1,pitch);
        }
        voice_set_volume(slot_index,VOICE_FULL_VOLUME);
        voices_left = 0;
      }
      slot_index = slot_index + 1;
      voices_left = voices_left + -1;
    } while (0 < voices_left);
  }
  return;
}
