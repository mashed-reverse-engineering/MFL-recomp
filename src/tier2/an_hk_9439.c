#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*00481fd0*/ extern float random_range(float low, float high);
/*004c45c0*/ extern int FUN_004c45c0(int low, int high);      /* random integer low..high */
/*005b6a40*/ extern void FUN_005b6a40(uint voice, int enable);
/*005b7300*/ extern void FUN_005b7300(int sound_object, int param, int enable, float value);
/*005b72a0*/ extern void FUN_005b72a0(int sound_object, int param, int enable, RwV3d *position);
/*005dd61c*/ extern float k_f_0_001;
/*005df9a0*/ extern float k_f_3_14;
extern double sin(double x);

#define VOICE_FIELD(slot, type, off) (*(type *)(*(char **)&g_voices[slot].voice + (off)))
#define GROUP_VOICE(off) (*(int *)(g_sound_groups + (off)))
/*006ae1c8*/ extern float g_sound_ambience_6ae1c8;      /* volume swell phase of voice group +0x470 */
/*006ae1cc*/ extern float g_sound_ambience_6ae1cc;      /* pitch / volume swell phase of voice group +0x620 */
/*00629278*/ extern float g_sound_ambience_629278[3];   /* phases of the three voices of group +0x590 (course 0x1e) */

/* Ambience of courses 0x01 and 0x1e: a looping voice (+0x470) with a random pitch around 22050 Hz and a slowly dipping
   volume, a voice (+0x620) at (7.7, 2.9, 10.5) whose pitch (22050 - 500 sin) and volume (0.9 - 0.1 sin) follow one phase,
   on course 0x1e three more fixed voices (+0x590 .. +2), and random one-shots (+0x500 on a 1 in 1001 roll at
   (15, 11, 5), +0x6b0 on a 1 in 701 roll at (4.25, 7, 1.6)) with a random pitch 22050..23050 Hz. */
/* TIER2: validated 2026-10-21 authority=diffemu */
void sound_ambience_course_1_1e(int course_id)
{
  RwV3d position;
  RwV3d position_1e;
  RwV3d position_shot;
  double rate;
  int slot;
  int i;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  g_sound_ambience_6ae1c8 += k_f_0_001;
  if (g_sound_ambience_6ae1c8 > k_f_3_14) {
    g_sound_ambience_6ae1c8 = 0.0f;
  }
  slot = GROUP_VOICE(0x470);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  /* the voice and its sound object each get their own random pitch */
  VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 100.0f) + 22050.0f;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 100.0f) + 22050.0f);
  }
  voice_set_volume(slot, (float)(1.0 - sin(g_sound_ambience_6ae1c8) * 0.1f));

  g_sound_ambience_6ae1cc += k_f_0_001;
  if (g_sound_ambience_6ae1cc > k_f_3_14) {
    g_sound_ambience_6ae1cc = 0.0f;
  }
  slot = GROUP_VOICE(0x620);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
    FUN_005b6a40(g_voices[slot].voice, 1);
  }
  rate = 22050.0 - sin(g_sound_ambience_6ae1cc) * 500.0;
  VOICE_FIELD(slot, float, 0x3c) = (float)rate;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, (float)rate);
  }
  voice_set_volume(slot, (float)(0.9f - sin(g_sound_ambience_6ae1cc) * 0.1f));
  position.x = 7.7f;
  position.y = 2.9f;
  position.z = 10.5f;
  VOICE_FIELD(slot, RwV3d, 0x58) = position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, &position);
  }

  if (course_id == 0x1e) {
    for (i = 0; i < 3; i++) {
      g_sound_ambience_629278[i] += k_f_0_001;
      if (g_sound_ambience_629278[i] > k_f_3_14) {
        g_sound_ambience_629278[i] = 0.0f;
      }
      if ((VOICE_FIELD(GROUP_VOICE(0x590) + i, uchar, 0x34) & 2) == 0) {
        FUN_005b6a40(g_voices[GROUP_VOICE(0x590) + i].voice, 1);
      }
    }
    slot = GROUP_VOICE(0x590);
    position_1e.x = -4.0f;
    position_1e.y = 2.6f;
    position_1e.z = -6.0f;
    VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
    }
    voice_set_volume(slot, 0.1f);
    VOICE_FIELD(slot, RwV3d, 0x58) = position_1e;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, &position_1e);
    }
    position_1e.x = 18.0f;
    position_1e.y = -1.6f;
    position_1e.z = -3.2f;
    VOICE_FIELD(slot + 1, float, 0x3c) = 22050.0f;
    if (VOICE_FIELD(slot + 1, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot + 1, int, 0x30), 4, 1, 22050.0f);
    }
    voice_set_volume(slot + 1, 1.0f);
    VOICE_FIELD(slot + 1, RwV3d, 0x58) = position_1e;
    if (VOICE_FIELD(slot + 1, int, 0x30) != 0) {
      FUN_005b72a0(VOICE_FIELD(slot + 1, int, 0x30), 0, 1, &position_1e);
    }
    position_1e.x = 11.9f;
    position_1e.y = -2.136f;
    position_1e.z = -30.4f;
    VOICE_FIELD(slot + 2, float, 0x3c) = 22050.0f;
    if (VOICE_FIELD(slot + 2, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot + 2, int, 0x30), 4, 1, 22050.0f);
    }
    voice_set_volume(slot + 2, 1.0f);
    VOICE_FIELD(slot + 2, RwV3d, 0x58) = position_1e;
    if (VOICE_FIELD(slot + 2, int, 0x30) != 0) {
      FUN_005b72a0(VOICE_FIELD(slot + 2, int, 0x30), 0, 1, &position_1e);
    }
  }
  if (FUN_004c45c0(0, 1000) == 25) {
    slot = GROUP_VOICE(0x500);
    if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
      FUN_005b6a40(g_voices[slot].voice, 1);
    }
    /* the voice and its sound object each get their own random pitch */
    VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 1000.0f) + 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 1000.0f) + 22050.0f);
    }
    voice_set_volume(slot, 1.0f);
    position_shot.x = 15.0f;
    position_shot.y = 11.0f;
    position_shot.z = 5.0f;
    VOICE_FIELD(slot, RwV3d, 0x58) = position_shot;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, &position_shot);
    }
  }
  if (FUN_004c45c0(0, 700) == 35) {
    slot = GROUP_VOICE(0x6b0);
    if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {
      FUN_005b6a40(g_voices[slot].voice, 1);
    }
    /* the voice and its sound object each get their own random pitch */
    VOICE_FIELD(slot, float, 0x3c) = random_range(0.0f, 1000.0f) + 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, random_range(0.0f, 1000.0f) + 22050.0f);
    }
    voice_set_volume(slot, 1.0f);
    position_shot.x = 4.25f;
    position_shot.y = 7.0f;
    position_shot.z = 1.6f;
    VOICE_FIELD(slot, RwV3d, 0x58) = position_shot;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, &position_shot);
    }
  }
}
