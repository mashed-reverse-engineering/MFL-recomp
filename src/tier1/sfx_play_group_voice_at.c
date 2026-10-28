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
/*008bd7b0*/ extern char *g_sound_groups;
#define GROUP_RECORD_SIZE (0x90)
#define GROUP_VOICE_SLOT_OFS (0x6b0)
#define GROUP_VOICE_TRIES (2)
#define VOICE_FLAG_IN_USE (2)
#define VOICE_PITCH_OFS (0x3c)
/* sfx_play_group_voice_at */
/* "(?, position*, group): starts two voices of a group with a pitch ramp (inferred)" */
/*
 * Starts one voice of sound group `group` at world `position` (x, y, z floats), with a pitch
 * ramp. Only runs while get_app_state() == 3. Walks the group's voice slots from the first one,
 * takes the first slot whose voice is not in use, copies the position into the voice record,
 * and sets its pitch from the number of tries left. Returns nothing.
 */
void __cdecl sfx_play_group_voice_at(undefined4 *position,int group)
{
  char *voice;
  int sample_handle;
  float pitch;
  /* Holds the app state first, then the index of the current voice slot (reused). */
  int app_state;
  undefined4 pitch_call_result;
  /* Candidate slots left to try (starts at GROUP_VOICE_TRIES); also the pitch ramp step.
     Reused as the sample handle once a free voice is found (inferred). */
  int tries_left;

  app_state = get_app_state();
  if (app_state == APP_STATE_RACE) {
    app_state = *(int *)(group * (GROUP_RECORD_SIZE) + GROUP_VOICE_SLOT_OFS + g_sound_groups);
    tries_left = GROUP_VOICE_TRIES;
    /* Try the slots in turn; the loop ends once a free voice has been started. */
    do {
      voice = g_voices[app_state].voice;
      /* Only a voice that is not in use is taken. */
      if ((*(byte *)(voice + VOICE_OFF_FLAGS) & VOICE_FLAG_IN_USE) == 0) {
        /* Copy the position into the voice record. */
        *(RwV3d *)(voice + VOICE_OFF_POS_X) = *(RwV3d *)position;
        sample_handle = *(int *)(g_voices[app_state].voice + VOICE_OFF_HANDLE);
        if (sample_handle != 0) {
          sound_voice_call_entry(sample_handle,0,1,position);
        }
        sound_voice_set_enabled(g_voices[app_state].voice,1);
        /* Pitch ramp: earlier tries (larger tries_left) start higher (inferred). */
        pitch = (float)tries_left * k_f_2000 + k_f_22050;
        *(float *)(g_voices[app_state].voice + VOICE_PITCH_OFS) = pitch;
        /* From here tries_left holds the sample handle, not a count (reused). */
        tries_left = *(int *)(g_voices[app_state].voice + VOICE_OFF_HANDLE);
        pitch_call_result = 0;
        if (tries_left != 0) {
          pitch_call_result = sound_voice_set_float_param(tries_left,4,1,pitch);
        }
        voice_set_volume(app_state,1.0);
        tries_left = 0;
      }
      /* Next slot; the counter is cleared above when a voice starts, so the loop ends then. */
      app_state = app_state + 1;
      tries_left = tries_left + -1;
    } while (0 < tries_left);
  }
  return;
}
