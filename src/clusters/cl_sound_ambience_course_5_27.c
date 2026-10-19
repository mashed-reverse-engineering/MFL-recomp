// cluster-entries: sound_ambience_course_5_27
/* Installed cluster sound_ambience_course_5_27 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_5_27.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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


/* Same, without the "already playing" test and with an explicit rate. */
static void sound_voice_start_rate(int slot, float rate)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + 0x3c) = rate;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + 0x30), 4, 1, rate);
  }
}

/* Places a voice at the position given as raw float bits (x, y, z) and tells its sound object. */
static void sound_voice_place_bits(int slot, unsigned int x, unsigned int y, unsigned int z)
{
  unsigned int position[3];

  position[0] = x;
  position[1] = y;
  position[2] = z;
  *(unsigned int *)(g_voices[slot].voice + 0x58) = x;
  *(unsigned int *)(g_voices[slot].voice + 0x5c) = y;
  *(unsigned int *)(g_voices[slot].voice + 0x60) = z;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b72a0(*(int *)(g_voices[slot].voice + 0x30), 0, 1, (float *)position);
  }
}

/* Places a voice at *position (3 floats) and tells its sound object. */
static void sound_voice_place_ptr(int slot, float *position)
{
  *(float *)(g_voices[slot].voice + 0x58) = position[0];
  *(float *)(g_voices[slot].voice + 0x5c) = position[1];
  *(float *)(g_voices[slot].voice + 0x60) = position[2];
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b72a0(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
  }
}
/*006ae1dc*/ extern float g_sound_ambience_6ae1dc;
/*005de20c*/ extern float k_f_2_5;
/*005de1ac*/ extern float k_f_0_125;
/*00455750*/ extern float *course_get_car_body_position(int car);

/* Ambience of courses 5 and 27: a camera-height scaled bed (group +0x470), three fixed point sources (+0x500), two sources following the cars' bodies (+0x590), and two more fixed ones (+0x620, +0x6b0 with rate 21050). */
void sound_ambience_course_5_27(void)
{
  float camera[3];
  int slot;
  int i;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  slot = *(int *)(g_sound_groups + 0x470);
  sound_voice_start_22050(slot);
  g_sound_ambience_6ae1dc = (camera[1] + k_f_2_5) * k_f_0_125;
  if (g_sound_ambience_6ae1dc < k_f_0) {
    g_sound_ambience_6ae1dc = 0.0f;
  } else if (g_sound_ambience_6ae1dc > float_1_0) {
    g_sound_ambience_6ae1dc = 1.0f;
  }
  voice_set_volume(slot, g_sound_ambience_6ae1dc);
  for (i = 0; i < 3; i++) {
    slot = *(int *)(g_sound_groups + 0x500) + i;
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
  }
  slot = *(int *)(g_sound_groups + 0x500);
  sound_voice_place_bits(slot, 0x413ccccd, 0x4123851f, 0x4197ae14);
  sound_voice_place_bits(slot + 1, 0x41f4cccd, 0x412e6666, 0x42020000);
  sound_voice_place_bits(slot + 2, 0xc0accccd, 0x40b66666, 0x41766666);
  for (i = 0; i < 2; i++) {
    slot = *(int *)(g_sound_groups + 0x590) + i;
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
  }
  slot = *(int *)(g_sound_groups + 0x590);
  sound_voice_place_ptr(slot, course_get_car_body_position(0));
  sound_voice_place_ptr(slot + 1, course_get_car_body_position(1));
  slot = *(int *)(g_sound_groups + 0x620);
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0xc14b3333, 0x408ccccd, 0x4142e148);
  slot = *(int *)(g_sound_groups + 0x6b0);
  sound_voice_start_rate(slot, 21050.0f);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0xc20acccd, 0x40000000, 0x41a33333);
}

