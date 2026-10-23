#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*006ae1d0*/ extern float g_sound_ambience_6ae1d0;      /* phase of the volume swell, 0..3.14 */
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*00481fd0*/ extern float random_range(float low, float high);
/*005b6a40*/ extern void FUN_005b6a40(uint voice, int enable);
/*005b7300*/ extern void FUN_005b7300(int sound_object, int param, int enable, float value);
/*005b72a0*/ extern void FUN_005b72a0(int sound_object, int param, int enable, RwV3d *position);
/*005dd61c*/ extern float k_f_0_001;
/*005df9a0*/ extern float k_f_3_14;
extern double sin(double x);

#define VOICE_FIELD(slot, type, off) (*(type *)(*(char **)&g_voices[slot].voice + (off)))

/* Ambience of courses 0x11 and 0x14: one looping voice (group slot +0x470) with a random pitch around 22050 Hz and a
   volume that dips slowly (1 - 0.1 sin(phase), the phase runs 0..3.14 in steps of 0.001), plus four consecutive voices of
   group slot +0x500 at fixed pitches 22050..22650 Hz, placed in a row at x 0.7, y 1.4, z 4.1 / 7.1 / 11.1 / 15.1. */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
void sound_ambience_course_11_14(void)
{
  RwV3d position;
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  g_sound_ambience_6ae1d0 += k_f_0_001;
  if (g_sound_ambience_6ae1d0 > k_f_3_14) {
    g_sound_ambience_6ae1d0 = 0.0f;
  }
  slot = *(int *)(g_sound_groups + 0x470);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  /* the voice and its sound object each get their own random pitch */
  VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 100.0f) + 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 100.0f) + 22050.0f);
  }
  voice_set_volume(slot, (float)(1.0 - sin(g_sound_ambience_6ae1d0) * 0.1f));

  slot = *(int *)(g_sound_groups + 0x500);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
  }
  voice_set_volume(slot, 1.0f);
  position.x = 0.7f;
  position.y = 1.4f;
  position.z = 4.1f;
  VOICE_FIELD(slot, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
  }
  if ((VOICE_FIELD(slot + 1, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot + 1].voice, 1);
  }
  VOICE_FIELD(slot + 1, float, 0x3c) = 22250.0f;
  if (VOICE_FIELD(slot + 1, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot + 1, int, 0x30), 4, 1, 22250.0f);
  }
  voice_set_volume(slot + 1, 1.0f);
  position.z = 7.1f;
  VOICE_FIELD(slot + 1, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot + 1, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot + 1, int, 0x30), 0, 1, &position);
  }
  if ((VOICE_FIELD(slot + 2, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot + 2].voice, 1);
  }
  VOICE_FIELD(slot + 2, float, 0x3c) = 22450.0f;
  if (VOICE_FIELD(slot + 2, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot + 2, int, 0x30), 4, 1, 22450.0f);
  }
  voice_set_volume(slot + 2, 1.0f);
  position.z = 11.1f;
  VOICE_FIELD(slot + 2, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot + 2, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot + 2, int, 0x30), 0, 1, &position);
  }
  if ((VOICE_FIELD(slot + 3, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot + 3].voice, 1);
  }
  VOICE_FIELD(slot + 3, float, 0x3c) = 22650.0f;
  if (VOICE_FIELD(slot + 3, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot + 3, int, 0x30), 4, 1, 22650.0f);
  }
  voice_set_volume(slot + 3, 1.0f);
  position.z = 15.1f;
  VOICE_FIELD(slot + 3, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot + 3, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot + 3, int, 0x30), 0, 1, &position);
  }
}
