#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*00481fd0*/ extern float random_range(float low, float high);
/*004c45c0*/ extern int FUN_004c45c0(int low, int high);      /* random integer low..high */
/*005b6a40*/ extern void sound_voice_set_enabled(uint voice, int enable);
/*005b7300*/ extern void sound_voice_set_float_param(int sound_object, int param, int enable, float value);
/*005b72a0*/ extern void sound_voice_call_entry(int sound_object, int param, int enable, RwV3d *position);
/*005dd61c*/ extern float k_f_0_001;
/*005df9a0*/ extern float k_f_3_14;
extern double sin(double x);

#define VOICE_FIELD(slot, type, off) (*(type *)(*(char **)&g_voices[slot].voice + (off)))
#define GROUP_VOICE(off) (*(int *)(g_sound_groups + (off)))
/*006ae1d8*/ extern float g_sound_ambience_6ae1d8;      /* volume swell phase of voice group +0x470 */
/*00629290*/ extern float g_sound_ambience_629290;      /* volume swell phase of voice group +0x500 */

/* Ambience of course "b": two looping 22050 Hz voices (group slots +0x470, +0x500) whose volumes swell independently (0.7 - 0.7 sin(phase)), a fixed voice (+0x590) at (-11.9, 1, -19.13), ... */
/* Ambience of course "b": two looping 22050 Hz voices (group slots +0x470, +0x500) whose volumes swell independently
   (0.7 - 0.7 sin(phase)), a fixed voice (+0x590) at (-11.9, 1, -19.13), random one-shots (+0x6b0 on a 1 in 251 roll at the
   same place, +0x620 on a 1 in 501 roll at (-41.9, 2.8, 41.38)) with a random pitch 22050..25050 Hz, and a fixed
   voice (+0x740) at (-6.96, 1.9, 17.77). */
void sound_ambience_course_b(void)
{
  RwV3d position;
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  g_sound_ambience_6ae1d8 += k_f_0_001;
  if (g_sound_ambience_6ae1d8 > k_f_3_14) {
    g_sound_ambience_6ae1d8 = 0.0f;
  }
  slot = GROUP_VOICE(0x470);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
  }
  voice_set_volume(slot, (float)(0.7f - sin(g_sound_ambience_6ae1d8) * 0.7f));

  g_sound_ambience_629290 += k_f_0_001;
  if (g_sound_ambience_629290 > k_f_3_14) {
    g_sound_ambience_629290 = 0.0f;
  }
  slot = GROUP_VOICE(0x500);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
  }
  voice_set_volume(slot, (float)(0.7f - sin(g_sound_ambience_629290) * 0.7f));

  slot = GROUP_VOICE(0x590);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
  }
  voice_set_volume(slot, 1.0f);
  position.x = -11.9f;
  position.y = 1.0f;
  position.z = -19.13f;
  VOICE_FIELD(slot, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_call_entry(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
  }

  if (FUN_004c45c0(0, 250) == 25) {
    slot = GROUP_VOICE(0x6b0);
    if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
      sound_voice_set_enabled(g_voices[slot].voice, 1);
    }
    /* the voice and its sound object each get their own random pitch */
    VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 3000.0f) + 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 3000.0f) + 22050.0f);
    }
    voice_set_volume(slot, 1.0f);
    position.x = -11.9f;
    position.y = 1.0f;
    position.z = -19.13f;
    VOICE_FIELD(slot, RwV3d, 0x58) = position;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      sound_voice_call_entry(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
    }
  }
  if (FUN_004c45c0(0, 500) == 35) {
    slot = GROUP_VOICE(0x620);
    if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
      sound_voice_set_enabled(g_voices[slot].voice, 1);
    }
    /* the voice and its sound object each get their own random pitch */
    VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 3000.0f) + 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 3000.0f) + 22050.0f);
    }
    voice_set_volume(slot, 1.0f);
    position.x = -41.9f;
    position.y = 2.8f;
    position.z = 41.38f;
    VOICE_FIELD(slot, RwV3d, 0x58) = position;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      sound_voice_call_entry(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
    }
  }
  slot = GROUP_VOICE(0x740);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
  }
  voice_set_volume(slot, 1.0f);
  position.x = -6.96f;
  position.y = 1.9f;
  position.z = 17.77f;
  VOICE_FIELD(slot, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    sound_voice_call_entry(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
  }
}
