// cluster-entries: sound_ambience_course_0
/* Installed cluster sound_ambience_course_0 (tools/cluster_install.py from output/cluster_pilot_zy/sound_ambience_course_0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
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
/*006ae1a0*/ extern float g_sound_ambience_6ae1a0;
/*006ae0a0*/ extern float g_sound_ambience_6ae0a0[3];
/*006ad1e4*/ extern struct RpSpline *g_sound_6ad1e4;
/*006adcf0*/ extern int g_sound_6adcf0;
/*006ad8ec*/ extern int g_sound_6ad8ec;
/*006ae1a4*/ extern int g_sound_ambience_6ae1a4;
/*005de20c*/ extern float k_f_2_5;
/*005de1ac*/ extern float k_f_0_125;
/*005dd628*/ extern float k_f_0_25;
/*005dd338*/ extern float k_f_0_01;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb18*/ extern float float_0_8;
/*005df368*/ extern float s_333_Lap_frames_005df368;
/*00481fd0*/ extern double random_range(float low, float high);
/*0047a830*/ extern int car_is_active(int car);
/*00409f30*/ extern double player_progress_b(int car);
/*00555ff0*/ extern struct RwV3d *RpSplineFindPosition(struct RpSpline *spline, int segments, float t, struct RwV3d *position, struct RwV3d *tangent);

/* Ambience of course 0: camera-height beds (group slots +0x6b0 / +0x470), a random-volume voice (+0x500) that moves along the track spline at the cars' mean progress,
   a random one-shot (+0x590 + 0..1) every 1000..3000 frames and a fixed one-shot (+0x740) every 500 frames. */
void sound_ambience_course_0(void)
{
  float camera[3];
  float volume;
  float level;
  float progress;
  int slot;
  int spline_slot;
  int count;
  int i;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count >= 2) {
    return;
  }
  camera_get_position(camera);
  slot = *(int *)(g_sound_groups + 0x6b0);
  sound_voice_start_22050(slot);
  volume = (camera[1] + k_f_2_5) * k_f_0_125;
  if (volume < k_f_0) {
    volume = 0.0f;
  } else if (volume > float_1_0) {
    volume = 1.0f;
  }
  voice_set_volume(slot, volume);
  slot = *(int *)(g_sound_groups + 0x470);
  sound_voice_restart_22050(slot);
  level = (camera[1] + s_333_Lap_frames_005df368) * k_f_0_125 - k_f_0_25;
  progress = k_f_0;
  if (level >= k_f_0) {
    progress = level;
    if (level > float_1_0) {
      progress = float_1_0;
    }
  }
  voice_set_volume(slot, float_1_0 - progress);
  slot = *(int *)(g_sound_groups + 0x500);
  sound_voice_restart_22050(slot);
  voice_set_volume(slot, (float)(float_0_8 - random_range(0.0f, 0.1f)));
  spline_slot = slot;
  if (g_sound_6ad1e4 != 0) {
    count = 0;
    g_sound_ambience_6ae1a0 = 0.0f;
    for (i = 0; i < 4; i++) {
      if (car_is_active(i) != 0) {
        count++;
        g_sound_ambience_6ae1a0 = (float)(player_progress_b(i) * k_f_0_01 + g_sound_ambience_6ae1a0);
      }
    }
    if (count != 0) {
      g_sound_ambience_6ae1a0 = g_sound_ambience_6ae1a0 / (float)count;
    }
    if (g_sound_ambience_6ae1a0 <= float_1_0) {
      if (g_sound_ambience_6ae1a0 < k_f_0) {
        g_sound_ambience_6ae1a0 = 0.0f;
      }
    } else {
      g_sound_ambience_6ae1a0 = 1.0f;
    }
    g_sound_ambience_6ae1a0 = g_sound_ambience_6ae1a0 + k_f_0_05;
    if (float_1_0 < g_sound_ambience_6ae1a0) {
      g_sound_ambience_6ae1a0 = g_sound_ambience_6ae1a0 - float_1_0;
    }
    RpSplineFindPosition(g_sound_6ad1e4, 10, g_sound_ambience_6ae1a0, (struct RwV3d *)g_sound_ambience_6ae0a0, 0);
    sound_voice_place_ptr(spline_slot, g_sound_ambience_6ae0a0);
    voice_set_volume(spline_slot, 1.0f);
  }
  slot = *(int *)(g_sound_groups + 0x590);
  g_sound_6adcf0 = g_sound_6adcf0 + 1;
  if (g_sound_6ad8ec <= g_sound_6adcf0) {
    slot = slot + (int)random_range(0.0f, 2.0f);
    sound_voice_force_22050(slot);
    voice_set_volume(slot, (float)(float_1_0 - random_range(0.0f, 0.2f)));
    g_sound_6adcf0 = 0;
    g_sound_6ad8ec = (int)random_range(1000.0f, 3000.0f);
  }
  if (g_sound_ambience_6ae1a4 == 500) {
    slot = *(int *)(g_sound_groups + 0x740);
    sound_voice_force_22050(slot);
    voice_set_volume(slot, 1.0f);
    sound_voice_place_bits(slot, 0xc1b80000, 0, 0xc2540000);
    g_sound_ambience_6ae1a4 = 0;
  }
  g_sound_ambience_6ae1a4 = g_sound_ambience_6ae1a4 + 1;
}

