// cluster-entries: sound_ambience_course_25
/* Installed cluster sound_ambience_course_25 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_25.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
  if ((*(unsigned char *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + VOICE_OFF_FREQ) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 4, 1, 22050.0f);
  }
}


/* Same, without the "already playing" test and with an explicit rate. */
static void sound_voice_start_rate(int slot, float rate)
{
  if ((*(unsigned char *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  *(float *)(g_voices[slot].voice + VOICE_OFF_FREQ) = rate;
  if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 4, 1, rate);
  }
}

/* Places a voice at the position given as raw float bits (x, y, z) and tells its sound object. */
static void sound_voice_place_bits(int slot, unsigned int x, unsigned int y, unsigned int z)
{
  unsigned int position[3];

  position[0] = x;
  position[1] = y;
  position[2] = z;
  *(unsigned int *)(g_voices[slot].voice + VOICE_OFF_POS_X) = x;
  *(unsigned int *)(g_voices[slot].voice + VOICE_OFF_POS_Y) = y;
  *(unsigned int *)(g_voices[slot].voice + VOICE_OFF_POS_Z) = z;
  if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
    FUN_005b72a0(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 0, 1, (float *)position);
  }
}

/* Places a voice at *position (3 floats) and tells its sound object. */
static void sound_voice_place_ptr(int slot, float *position)
{
  *(float *)(g_voices[slot].voice + VOICE_OFF_POS_X) = position[0];
  *(float *)(g_voices[slot].voice + VOICE_OFF_POS_Y) = position[1];
  *(float *)(g_voices[slot].voice + VOICE_OFF_POS_Z) = position[2];
  if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
    FUN_005b72a0(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 0, 1, position);
  }
}

/* Same as sound_voice_start_22050 but the rate is only set when the voice had to be started. */
static void sound_voice_restart_22050(int slot)
{
  if ((*(unsigned char *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
    *(float *)(g_voices[slot].voice + VOICE_OFF_FREQ) = 22050.0f;
    if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
      FUN_005b7300(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 4, 1, 22050.0f);
    }
  }
}

/* Same, always (re)starts the voice. */
static void sound_voice_force_22050(int slot)
{
  FUN_005b6a40(g_voices[slot].voice, 1);
  *(float *)(g_voices[slot].voice + VOICE_OFF_FREQ) = 22050.0f;
  if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
    FUN_005b7300(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 4, 1, 22050.0f);
  }
}
/*005dd7f0*/ extern float k_f_30;
/*005dd334*/ extern float k_f_0_033333335;
/*005dd374*/ extern float k_f_0_2;
/*005deb40*/ extern float k_f_34;
/*005dd620*/ extern float float_10_0;
/*005dd364*/ extern float k_f_n1;
/*005dde40*/ extern float k_f_70;
/*00409f30*/ extern double player_progress_b(int car);
/*00481fd0*/ extern double random_range(float low, float high);
/*004c45c0*/ extern int rp_random_int_range(int low, int high);

/* Start the voice of a group slot, set its volume and place it at a fixed position (raw float bits). */
static void sound_voice_fixed_a(int offset, float rate, float volume, unsigned int x, unsigned int y, unsigned int z)
{
  int slot = *(int *)(g_sound_groups + offset);

  sound_voice_start_rate(slot, rate);
  voice_set_volume(slot, volume);
  sound_voice_place_bits(slot, x, y, z);
}

/* Ambience of course 25: a bed whose volume follows the lead car's progress (group +0x470: 0.2, rising below 30 / above 70), then 12 voices at fixed positions, and with a 1 in 101 roll a voice at a random position. */
void sound_ambience_course_25(void)
{
  float camera[3];
  float position[3];
  float progress;
  int slot;
  int base;
  double random_x;
  double random_z;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  progress = (float)player_progress_b(0);
  camera_get_position(camera);
  slot = SOUND_GROUP_FIRST_VOICE_OF(7);
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 0.2f);
  if (progress < k_f_30) {
    voice_set_volume(slot, (progress - k_f_30) * k_f_0_033333335 * k_f_n1 * k_f_0_2 + k_f_0_2);
  }
  if (progress > k_f_70) {
    voice_set_volume(slot, (progress - k_f_70) * k_f_0_033333335 * k_f_0_2 + k_f_0_2);
  }
  sound_voice_fixed_a(0x500, 22050.0f, 1.0f, 0x42860000, 0xc0a00000, 0x42fc0000);
  slot = SOUND_GROUP_FIRST_VOICE_OF(9);
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x42740000, 0xc0a00000, 0x42e7cccd);
  slot = slot + 1;
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x423e0000, 0x40000000, 0x4286999a);
  slot = SOUND_GROUP_FIRST_VOICE_OF(10);
  sound_voice_start_22050(slot);
  sound_voice_place_bits(slot, 0x422f3333, 0x40000000, 0x427c0000);
  voice_set_volume(slot, 1.0f);
  sound_voice_fixed_a(0x6b0, 27050.0f, 0.9f, 0x4214cccd, 0xc04ccccd, 0x42e60000);
  slot = SOUND_GROUP_FIRST_VOICE_OF(12);
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x422e6666, 0xc0566666, 0x42b96666);
  slot = slot + 1;
  sound_voice_start_rate(slot, 25050.0f);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x42300000, 0xc0870a3d, 0x42d2cccd);
  sound_voice_fixed_a(0xa10, 22050.0f, 1.0f, 0x3fd9999a, 0xbf19999a, 0x42ef3333);
  sound_voice_fixed_a(0x980, 22050.0f, 1.0f, 0x42886666, 0xc0866666, 0x42d5999a);
  base = SOUND_GROUP_FIRST_VOICE_OF(17);
  slot = base + 1;
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x420ecccd, 0x3ecccccd, 0x42700000);
  slot = base + 2;
  sound_voice_start_22050(slot);
  voice_set_volume(slot, 1.0f);
  sound_voice_place_bits(slot, 0x42896666, 0x3f2e147b, 0x425a6666);
  if (rp_random_int_range(0, 100) == 0x19) {
    slot = rp_random_int_range(0, 1);
    slot = *(int *)(slot * SOUND_GROUP_STRIDE + 2000 + g_sound_groups);
    sound_voice_start_22050(slot);
    voice_set_volume(slot, 1.0f);
    random_x = random_range(0.0f, 20.0f);
    random_z = random_range(0.0f, 10.0f);
    position[2] = (float)(random_z + k_f_34);
    position[1] = 0.0f;
    *(float *)(g_voices[slot].voice + VOICE_OFF_POS_X) = (float)(random_x - float_10_0);
    *(unsigned int *)(g_voices[slot].voice + VOICE_OFF_POS_Y) = 0;
    *(float *)(g_voices[slot].voice + VOICE_OFF_POS_Z) = position[2];
    if (*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE) != 0) {
      position[0] = (float)(random_x - float_10_0);
      FUN_005b72a0(*(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE), 0, 1, position);
    }
  }
}

