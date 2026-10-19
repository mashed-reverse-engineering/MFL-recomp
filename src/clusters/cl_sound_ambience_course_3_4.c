// cluster-entries: sound_ambience_course_3_4
/* Installed cluster sound_ambience_course_3_4 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_3_4.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*0044a4f0*/ extern void camera_get_position(float *out);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*005b6a40*/ extern void FUN_005b6a40(int voice, int enable);
/*005b7300*/ extern void FUN_005b7300(int voice, int param, int enable, float value);
/*005b72a0*/ extern void FUN_005b72a0(int voice, int param, int enable, float *position);
/*005e64a4*/ extern float k_f_0;
/*005dd330*/ extern float float_1_0;

/* Starts a voice that is not playing yet and sets its rate/pitch (22050.0) on the voice and its sound object. */
static void sound_voice_start_22050(int slot)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
  }
}

/*006ae1e0*/ extern float g_sound_ambience_6ae1e0;
/*005de20c*/ extern float k_f_2_5;
/*005de1ac*/ extern float k_f_0_125;

/* Ambience of courses 3 and 4: one looping voice (group slot at +0x470) whose volume follows the camera height ((y + 2.5) / 8, clamped to 0..1). Skipped for split screen when muting is on. */
void sound_ambience_course_3_4(void)
{
  float camera[3];
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  slot = *(int *)(g_sound_groups + 0x470);
  sound_voice_start_22050(slot);
  g_sound_ambience_6ae1e0 = (camera[1] + k_f_2_5) * k_f_0_125;
  if (g_sound_ambience_6ae1e0 < k_f_0) {
    g_sound_ambience_6ae1e0 = 0.0f;
  } else if (g_sound_ambience_6ae1e0 > float_1_0) {
    g_sound_ambience_6ae1e0 = 1.0f;
  }
  voice_set_volume(slot, g_sound_ambience_6ae1e0);
}

