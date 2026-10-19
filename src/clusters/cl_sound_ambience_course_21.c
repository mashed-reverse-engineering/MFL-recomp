// cluster-entries: sound_ambience_course_21
/* Installed cluster sound_ambience_course_21 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_21.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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

/* Same as sound_voice_start_22050 but the rate is only set when the voice had to be started. */
static void sound_voice_restart_22050(int slot)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
    *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
    if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
      FUN_005b7300(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
    }
  }
}

/* Same, always (re)starts the voice. */
static void sound_voice_force_22050(int slot)
{
  FUN_005b6a40(g_voices[slot].voice, 1);
  *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
  }
}
/*006ae1bc*/ extern int g_sound_ambience_6ae1bc;
/*006ae1c0*/ extern int g_sound_ambience_6ae1c0;
/*006ae1c4*/ extern float g_sound_ambience_6ae1c4;
/*005dde40*/ extern float k_f_70;
/*005dfd00*/ extern float k_f_0_0125;
/*005dd370*/ extern float k_f_0_3;
/*005dd378*/ extern float k_f_0_1;
/*005dd340*/ extern float k_f_0_5;
/*005dec9c*/ extern float k_f_88;
/*005dfcfc*/ extern float k_f_104;
/*005dd62c*/ extern float k_f_100;
/*005de238*/ extern float k_f_2000;
/*005df99c*/ extern float k_f_22050;
/*005dfcf8*/ extern float k_f_n14;
/*005dec20*/ extern float k_f_14;
/*005dfcf4*/ extern float k_f_21;
/*00481fd0*/ extern double random_range(float low, float high);
/*004c45c0*/ extern int rp_random_int_range(int low, int high);
/*0045d620*/ extern float *course_get_matrix_pos_6a291c(void);
/*00472520*/ extern void sfx_play_group_voice_at(float *position, int group);
/*00472430*/ extern void sound_voices_set_position_start(float *position);

/* Rolls rp_random_int_range(0, sides) and on a hit plays group voice `group` at the fixed position. */
static void sound_ambience_roll_fixed(int sides, float x, float y, float z, int group)
{
  float position[3];

  if (rp_random_int_range(0, sides) == 0x19) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
    sfx_play_group_voice_at(position, group);
  }
}

/* Ambience of course 21: a speed/camera dependent bed (group +0x470), a periodic one-shot (+0x620 every 500 frames), a voice that follows a moving object (+0x500), three voices when it passes x = 116.41855 (+0x590), the object's engine voice (+0x8f0), and random one-shots around the track. */
void sound_ambience_course_21(void)
{
  float camera[3];
  float position[3];
  volatile float level;  /* stored float local in the original */
  double volume;
  double clamped;
  float *object;
  int slot;
  int i;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  if (camera[0] <= k_f_70) {
    level = 0.3f;
  } else {
    level = (camera[0] - k_f_70) * k_f_0_0125 + k_f_0_3;
    if (float_1_0 < level) {
      level = 1.0f;
    }
  }
  slot = *(int *)(g_sound_groups + 0x470);
  sound_voice_start_22050(slot);
  volume = (camera[1] + float_1_0) * k_f_0_1 + k_f_0_5;
  clamped = k_f_0;
  if (volume >= k_f_0) {
    clamped = volume;
    if (volume > float_1_0) {
      clamped = float_1_0;
    }
  }
  voice_set_volume(slot, clamped * level);
  if (g_sound_ambience_6ae1bc == 500) {
    slot = *(int *)(g_sound_groups + 0x620);
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
    sound_voice_place_bits(slot, 0x42500000, 0x40a00000, 0x41f80000);
    g_sound_ambience_6ae1bc = 0;
  }
  slot = *(int *)(g_sound_groups + 0x500);
  g_sound_ambience_6ae1bc = g_sound_ambience_6ae1bc + 1;
  sound_voice_start_rate(slot, 32050.0f);
  voice_set_volume(slot, 1.0f);
  object = course_get_matrix_pos_6a291c();
  position[0] = object[0];
  position[1] = 1.5f;
  position[2] = object[2];
  *(float *)(g_voices[slot].voice + 0x58) = position[0];
  *(unsigned int *)(g_voices[slot].voice + 0x5c) = 0x3fc00000;
  *(float *)(g_voices[slot].voice + 0x60) = position[2];
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    FUN_005b72a0(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
  }
  if (object[2] <= k_f_88 || (g_sound_ambience_6ae1c4 = 1.0f, g_sound_ambience_6ae1c0 != 0)) {
    g_sound_ambience_6ae1c4 = 0.0f;
  }
  if (k_f_104 < object[2] && g_sound_ambience_6ae1c0 == 0 && *(unsigned int *)&object[0] == 0x42e8d64c) {
    for (i = 0; i < 3; i++) {
      float rate;

      slot = *(int *)(g_sound_groups + 0x590) + i;
      if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
        FUN_005b6a40(g_voices[slot].voice, 1);
      }
      rate = (float)i * k_f_2000 + k_f_22050;
      *(float *)(g_voices[slot].voice + 0x3c) = rate;
      if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
        FUN_005b7300(*(int *)(g_voices[slot].voice + 0x30), 4, 1, rate);
      }
      voice_set_volume(slot, 1.0f);
      sound_voice_place_bits(slot, 0x42e60000, 0x40400000, 0x42c60000);
    }
    g_sound_ambience_6ae1c0 = 1;
  }
  if (object[2] < k_f_100) {
    g_sound_ambience_6ae1c0 = 0;
  }
  if (rp_random_int_range(0, 0x32) == 0x19) {
    position[0] = (float)random_range(0.0f, 84.0f);
    position[1] = 0.0f;
    position[2] = (float)(k_f_n14 - random_range(0.0f, 16.0f));
    sfx_play_group_voice_at(position, 0);
  }
  if (rp_random_int_range(0, 0xfa) == 0x19) {
    position[0] = (float)(random_range(0.0f, 30.0f) + k_f_14);
    position[1] = 0.0f;
    position[2] = (float)(random_range(0.0f, 34.0f) + k_f_21);
    sound_voices_set_position_start(position);
  }
  if (rp_random_int_range(0, 0xfa) == 0x19) {
    position[0] = (float)(random_range(0.0f, 30.0f) + k_f_14);
    position[1] = 0.0f;
    position[2] = (float)(random_range(0.0f, 34.0f) + k_f_21);
    sfx_play_group_voice_at(position, 1);
  }
  sound_ambience_roll_fixed(200, 152.0f, 0.0f, -7.0f, 1);
  sound_ambience_roll_fixed(200, 81.0f, 0.0f, 60.0f, 1);
  sound_ambience_roll_fixed(200, 151.0f, 1.0f, 35.0f, 1);
  sound_ambience_roll_fixed(200, 159.0f, 0.0f, 5.0f, 1);
  sound_ambience_roll_fixed(200, 168.0f, 0.0f, 22.0f, 1);
  sound_ambience_roll_fixed(200, 61.0f, 0.0f, 60.0f, 1);
  if (rp_random_int_range(0, 0xfa) == 0x19) {
    position[0] = (float)(random_range(0.0f, 30.0f) + k_f_14);
    position[1] = 0.0f;
    position[2] = (float)(random_range(0.0f, 34.0f) + k_f_21);
    sfx_play_group_voice_at(position, rp_random_int_range(0, 1) + 2);
  }
  if (rp_random_int_range(0, 0xfa) == 0x19) {
    position[0] = 52.0f;
    position[1] = 0.0f;
    position[2] = 23.0f;
    sfx_play_group_voice_at(position, rp_random_int_range(0, 1) + 2);
  }
  slot = *(int *)(g_sound_groups + 0x8f0);
  if (g_sound_ambience_6ae1c4 != k_f_0) {
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
    position[0] = object[0];
    position[1] = 1.5f;
    position[2] = object[2];
    *(float *)(g_voices[slot].voice + 0x58) = position[0];
    *(unsigned int *)(g_voices[slot].voice + 0x5c) = 0x3fc00000;
    *(float *)(g_voices[slot].voice + 0x60) = position[2];
    if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
      FUN_005b72a0(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
    }
  }
}

