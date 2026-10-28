// cluster-entries: sound_ambience_course_2_6_b
/* Installed cluster sound_ambience_course_2_6_b (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_2_6_b.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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


/* Same, without the "already playing" test and with an explicit rate. */
static void sound_voice_start_rate(int slot, float rate)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + 0x3c) = rate;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, rate);
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
    sound_voice_call_entry(*(int *)(g_voices[slot].voice + 0x30), 0, 1, (float *)position);
  }
}

/* Places a voice at *position (3 floats) and tells its sound object. */
static void sound_voice_place_ptr(int slot, float *position)
{
  *(float *)(g_voices[slot].voice + 0x58) = position[0];
  *(float *)(g_voices[slot].voice + 0x5c) = position[1];
  *(float *)(g_voices[slot].voice + 0x60) = position[2];
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_call_entry(*(int *)(g_voices[slot].voice + 0x30), 0, 1, position);
  }
}

/* Same as sound_voice_start_22050 but the rate is only set when the voice had to be started. */
static void sound_voice_restart_22050(int slot)
{
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
    *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
    if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
      sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
    }
  }
}

/* Same, always (re)starts the voice. */
static void sound_voice_force_22050(int slot)
{
  sound_voice_set_enabled(g_voices[slot].voice, 1);
  *(float *)(g_voices[slot].voice + 0x3c) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, 22050.0f);
  }
}
/*005dd378*/ extern float k_f_0_1;
/*005df99c*/ extern float k_f_22050;
/*00481fd0*/ extern double random_range(float low, float high);

/* Starts the voice of group slot `offset`, full volume, at a fixed position (raw float bits). */
static void sound_voice_fixed(int offset, unsigned int x, unsigned int y, unsigned int z)
{
  int slot = *(int *)(g_sound_groups + offset);

  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, x, y, z);
}

/* Ambience of courses 2 and 6 (variant b): a camera-height bed (group +0x590), one voice with a random rate 23050..24050 (+0x500, volume 0.3) and five fixed point sources. */
void sound_ambience_course_2_6_b(void)
{
  float camera[3];
  float volume;
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  slot = SOUND_GROUP_FIRST_VOICE_OF(9);
  sound_voice_start_22050(slot);
  volume = (camera[1] + float_1_0) * k_f_0_1;
  if (volume < k_f_0) {
    volume = 0.0f;
  } else if (volume > float_1_0) {
    volume = 1.0f;
  }
  voice_set_volume(slot, volume);
  slot = SOUND_GROUP_FIRST_VOICE_OF(8);
  if ((*(unsigned char *)(g_voices[slot].voice + 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + 0x3c) = (float)(random_range(1000.0f, 2000.0f) + k_f_22050);
  if (*(int *)(g_voices[slot].voice + 0x30) != 0) {
    sound_voice_set_float_param(*(int *)(g_voices[slot].voice + 0x30), 4, 1, (float)(random_range(1000.0f, 2000.0f) + k_f_22050));
  }
  voice_set_volume(slot, 0.3f);
  sound_voice_place_bits(slot, 0x40c28f5c, 0x3eb33333, 0x41c26666);
  sound_voice_fixed(0x6b0, 0xc1800000, 0x3fcccccd, 0x421c0000);
  slot = SOUND_GROUP_FIRST_VOICE_OF(11) + 1;
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x41500000, 0x3f800000, 0x42580000);
  sound_voice_fixed(0x740, 0x41c73333, 0x40733333, 0x416ccccd);
  sound_voice_fixed(2000, 0xc1a00000, 0x3e4ccccd, 0xc1f80000);
  sound_voice_fixed(0x860, 0xc1a80000, 0x40e66666, 0xc0a00000);
}

