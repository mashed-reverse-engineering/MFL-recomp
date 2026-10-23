#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005de238*/ extern float k_f_2000;
/*005df99c*/ extern float k_f_22050;
/* layout of *voice in sound_voices_set_position_wide, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct sound_voices_set_position_wide_voice_layout {
    char pad_0x0[52];
    byte f_0x34;
    char pad_0x35[35];
    undefined4 f_0x58;
    undefined4 f_0x5c;
    undefined4 f_0x60;
} sound_voices_set_position_wide_voice_layout;

/*00618c98*/ extern undefined4 g_sound_618c98;
#define WIDE_VOICE_COUNT (4)
#define WIDE_VOICE_BASE_OFFSET (4)
/* sound_voices_set_position_wide */

/* "for app_state 3 sets position vector on 4 voices from g_sound_618c98+4" */

/*
 * Copies a 3-float position vector into the voice records of the wide-spread
 * sound voices (WIDE_VOICE_COUNT slots starting at g_sound_618c98 + WIDE_VOICE_BASE_OFFSET),
 * but only when app_state is 3 and the voice's flag bit 0x2 (at +0x34) is clear.
 * Each updated voice also gets a playback frequency = (app_state * k_f_2000) + frequency_offset + k_f_22050
 * and a volume of 1.0. Returns nothing.
 *   position        - pointer to 3 floats (x, y, z), copied to +0x58..+0x60 of each voice
 *   frequency_offset - extra frequency added (inferred: spreads the pitch of the voices)
 */
void __cdecl sound_voices_set_position_wide(undefined4 *position,float frequency_offset)

{
  sound_voices_set_position_wide_voice_layout *voice;
  int source;
  float frequency;
  int app_state;
  undefined4 call_result;
  int slot_index;

  app_state = get_app_state();
  /* only the wide-spread state (app_state 3) updates the voices */
  if (app_state == APP_STATE_RACE) {
    /* app_state doubles as a countdown: starts at WIDE_VOICE_COUNT, and the
       value is also the frequency multiplier (inferred: voice rank) */
    slot_index = g_sound_618c98 + WIDE_VOICE_BASE_OFFSET;
    app_state = WIDE_VOICE_COUNT;
    do {
      voice = (sound_voices_set_position_wide_voice_layout *)(g_voices[slot_index].voice);
      /* skip voices with flag bit 0x2 set; an updated voice sets app_state to 0,
         so the loop ends after the first voice that is updated (inferred: check intent) */
      if ((voice->f_0x34 & 2) == 0) {
        /* copy the position vector into the voice record */
        voice->f_0x58 = *position;
        voice->f_0x5c = position[1];
        voice->f_0x60 = position[2];
        /* if the voice has a source at +0x30, pass it the position too */
        source = *(int *)(g_voices[slot_index].voice + 0x30);
        if (source != 0) {
          FUN_005b72a0(source,0,1,position);
        }
        FUN_005b6a40(g_voices[slot_index].voice,1);
        /* frequency stored at +0x3c and passed to the source */
        frequency = (float)app_state * k_f_2000 + frequency_offset + k_f_22050;
        *(float *)(g_voices[slot_index].voice + 0x3c) = frequency;
        app_state = *(int *)(g_voices[slot_index].voice + 0x30);
        call_result = 0;
        if (app_state != 0) {
          call_result = FUN_005b7300(app_state,4,1,frequency);
        }
        voice_set_volume(slot_index,1.0);
        app_state = 0;
      }
      /* next slot; countdown step */
      slot_index = slot_index + 1;
      app_state = app_state + -1;
    } while (0 < app_state);
  }
  return;
}


