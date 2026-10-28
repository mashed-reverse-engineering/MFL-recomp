// cluster-entries: sound_ambience_course_16_17
/* Installed cluster sound_ambience_course_16_17 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_16_17.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*0044a4f0*/ extern void camera_get_position(float *out);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*005b6a40*/ extern void sound_voice_set_enabled(int voice, int enable);
/*005b7300*/ extern void sound_voice_set_float_param(int voice, int param, int enable, float value);
/*005b72a0*/ extern void sound_voice_call_entry(int voice, int param, int enable, float *position);
/*005e64a4*/ extern float k_f_0;
/*005dd330*/ extern float float_1_0;

/* Starts a voice that is not playing yet and sets its rate/pitch (22050.0) on the voice and its sound object. */
static void sound_voice_start_22050(int slot)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
  }
}

/*004c45c0*/ extern int rp_random_int_range(int low, int high);
/*00473aa0*/ extern float *sound_get_6af57c(void);
/*005dd33c*/ extern float k_f_4;
/*005df9a4*/ extern float k_f_184;
/*005ddb54*/ extern float k_f_0_0055555557;
/*005df9a0*/ extern float k_f_3_14;
extern double sin(double x);
#pragma intrinsic(sin)

/* Ambience of courses 16 and 17: a looping voice (group slot +0x590) whose volume is a half sine over the camera z range 4..184; on a 1 in 26 roll a second voice (random group slot, +0x470 + 0x90 * n) starts at the position of sound_get_6af57c. */
void sound_ambience_course_16_17(void)
{
  float camera[3];
  float volume;
  int slot;
  float *position;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  slot = SOUND_GROUP_FIRST_VOICE_OF(9);
  sound_voice_start_22050(slot);
  if (!(camera[2] > k_f_4) || !(camera[2] < k_f_184)) {
    volume = 0.0f;
  } else {
    volume = (float)sin((camera[2] - k_f_4) * k_f_0_0055555557 * k_f_3_14);
  }
  voice_set_volume(slot, volume);
  if (rp_random_int_range(0, 0x19) == 0x19) {
    slot = rp_random_int_range(0, 1);
    slot = *(int *)(slot * 0x90 + 0x470 + g_sound_groups);
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
    position = sound_get_6af57c();
    *(float *)(g_voices[slot].voice + 0x58) = position[0];
    *(float *)(g_voices[slot].voice + 0x5c) = position[1];
    *(float *)(g_voices[slot].voice + 0x60) = position[2];
    if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
      sound_voice_call_entry(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
    }
  }
}

