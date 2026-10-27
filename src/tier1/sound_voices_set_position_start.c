#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005de238*/ extern float k_f_2000;
/*005df99c*/ extern float k_f_22050;
/*00618c98*/ extern undefined4 g_sound_618c98;
#define VOICE_FIRST_SLOT_OFFSET (4) /* candidates start at g_sound_618c98 + 4 */
#define VOICE_CANDIDATES (4) /* number of slots tried */
#define VOICE_FLAGS_OFFSET (0x34) /* byte */
#define VOICE_FLAG_SKIP_POSITION (2) /* (inferred) set = voice is skipped */
#define VOICE_HANDLE_OFFSET (0x30)
#define VOICE_POSITION_X_OFFSET (0x58)
#define VOICE_FREQUENCY_OFFSET (0x3c)
#define VOICE_START_VOLUME (0.5)
/* sound_voices_set_position_start: only in app state APP_STATE_RACE. Looks at the
   VOICE_CANDIDATES voice slots starting at g_sound_618c98 + VOICE_FIRST_SLOT_OFFSET and
   positions the first one whose VOICE_FLAG_SKIP_POSITION bit is clear: copies the 3 floats
   at position to the voice record, hands the pointer to FUN_005b72a0 (inferred: 3D position),
   starts the voice (FUN_005b6a40 with 1, inferred), sets its frequency and volume.
   position: 3 floats (x, y, z). unused_arg_2..5: never read.
   Returns nothing. (was: "same, then starts the voices") */
void __cdecl sound_voices_set_position_start(undefined4 *position,char *unused_arg_2,char *unused_arg_3,char *unused_arg_4, char *unused_arg_5)
{
  uint voice;
  int voice_handle;
  float frequency;
  int slot_index;                  /* app state first, then the voice slot index */
  undefined4 frequency_result;     /* return of FUN_005b7300, never read */
  int remaining_slots;             /* candidates left, this one included (4 down to 1) */

  /* only in app state APP_STATE_RACE */
  slot_index = get_app_state();
  if (slot_index == APP_STATE_RACE) {
    slot_index = g_sound_618c98 + VOICE_FIRST_SLOT_OFFSET;
    remaining_slots = VOICE_CANDIDATES;
    /* try each candidate slot in turn; stop after the first one that gets positioned */
    do {
      voice = g_voices[slot_index].voice;
      /* skip voices that have VOICE_FLAG_SKIP_POSITION set */
      if ((*(byte *)(voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_SKIP_POSITION) == 0) {
        /* copy the position into the voice record, then hand it to the handle (inferred: 3D position) */
        *(RwV3d *)(voice + VOICE_POSITION_X_OFFSET) = *(RwV3d *)position;
        voice_handle = *(int *)(g_voices[slot_index].voice + VOICE_HANDLE_OFFSET);
        if (voice_handle != 0) {
          FUN_005b72a0(voice_handle,0,1,position);
        }
        /* start the voice (inferred) */
        FUN_005b6a40(g_voices[slot_index].voice,1);
        /* frequency = k_f_22050 - remaining_slots * k_f_2000: the first candidate (remaining 4) gets the lowest */
        frequency = k_f_22050 - (float)remaining_slots * k_f_2000;
        *(float *)(g_voices[slot_index].voice + VOICE_FREQUENCY_OFFSET) = frequency;
        /* push the frequency to the handle, if there is one */
        voice_handle = *(int *)(g_voices[slot_index].voice + VOICE_HANDLE_OFFSET);
        frequency_result = 0;
        if (voice_handle != 0) {
          frequency_result = FUN_005b7300(voice_handle,4,1,frequency);
        }
        voice_set_volume(slot_index,VOICE_START_VOLUME);
        /* 0 ends the loop after the decrement below */
        remaining_slots = 0;
      }
      slot_index = slot_index + 1;
      remaining_slots = remaining_slots + -1;
    } while (0 < remaining_slots);
  }
  return;
}
