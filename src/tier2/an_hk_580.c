#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*008bd7b0*/ extern undefined4 g_sound_groups;
#define SOUND_GROUP_STRIDE (0x90)
#define SOUND_GROUP_FIRST_VOICE (0x80)
#define SOUND_GROUP_VOICE_COUNT (0x84)
/* sfx_play_group_at */

/* "(group, position* or null): starts the voices of a g_sound_groups record (+0x80 first voice,
   +0x84 count)" */

void sfx_play_group_at(int group_index,undefined4 *position)

{
  char *voice;
  int voice_index;
  int voices_left;
  undefined4 fx_result;

  /* voice_index holds the get_app_state() result for the check below, then the index of the
     first voice of the group (inferred: the same local is reused, not split) */
  voice_index = get_app_state();
  if (voice_index == APP_STATE_RACE) {
    /* group record: first voice index at +0x80, voice count at +0x84 */
    voices_left = group_index * SOUND_GROUP_STRIDE + g_sound_groups;
    voice_index = *(int *)(voices_left + SOUND_GROUP_FIRST_VOICE);
    /* loop over the group's voices (voices_left counts down) */
    for (voices_left = *(int *)(voices_left + SOUND_GROUP_VOICE_COUNT); 0 < voices_left; voices_left = voices_left + -1) {
      voice = g_voices[voice_index].voice;
      /* skip voices whose flags have bit 0x2 set (inferred: busy) */
      if ((*(byte *)(voice + VOICE_OFF_FLAGS) & 2) == 0) {
        /* optional position: copy the three floats into the voice */
        if (position != (undefined4 *)0x0) {
          *(undefined4 *)(voice + VOICE_OFF_POS_X) = *position;
          *(undefined4 *)(voice + VOICE_OFF_POS_Y) = position[1];
          *(undefined4 *)(voice + VOICE_OFF_POS_Z) = position[2];
          voices_left = *(int *)(g_voices[voice_index].voice + VOICE_OFF_HANDLE);
          if (voices_left != 0) {
            FUN_005b72a0(voices_left,0,1,position);
          }
        }
        FUN_005b6a40(g_voices[voice_index].voice,1);
        *(undefined4 *)(g_voices[voice_index].voice + 0x3c) = VOICE_DEFAULT_FREQ_BITS;
        voices_left = *(int *)(g_voices[voice_index].voice + VOICE_OFF_HANDLE);
        fx_result = 0;
        if (voices_left != 0) {
          fx_result = FUN_005b7300(voices_left,4,1,22050.0f);
        }
        voice_set_volume(voice_index,1.0);
        voices_left = 0;
      }
      voice_index = voice_index + 1;
    }
  }
  return;
}


