#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*00456ef0*/ extern RwV3d *course_get_matrix_pos_8c53e0(void);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*005b6a40*/ extern void FUN_005b6a40(uint voice, int enable);
/*005b7300*/ extern void FUN_005b7300(int sound_object, int param, int enable, float value);
/*005b72a0*/ extern void FUN_005b72a0(int sound_object, int param, int enable, RwV3d *position);
extern double sin(double x);
/*005dfce0*/ extern double k_f_n3_689349e19;
/*005dfcd8*/ extern double k_f_0_5dfcd8;
/*005df9b0*/ extern double k_f_0_5df9b0;

#define VOICE_FIELD(slot, type, off) (*(type *)(*(char **)&g_voices[slot].voice + (off)))
#define VOICE_POSITION(slot) VOICE_FIELD(slot, RwV3d, 0x58)

/* Ambience of courses 2 and 6 (first part): two looping voices (group slots +0x470 and +0x620) at full volume,
   placed at the object position of g_course_8c53e0; the second one plays slightly faster. */
/* TIER2: validated 2026-10-22 authority=diffemu */
void sound_ambience_course_2_6_a(void)
{
  RwV3d *position;
  int slot;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  position = course_get_matrix_pos_8c53e0();
  slot = *(int *)(g_sound_groups + 0x470);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {      /* not playing yet: start at 22050 Hz */
    FUN_005b6a40(g_voices[slot].voice, 1);
    VOICE_FIELD(slot, float, 0x3c) = 22050.0f;
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, 22050.0f);
    }
  }
  voice_set_volume(slot, 1.0f);
  VOICE_POSITION(slot) = *position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, position);
  }

  slot = *(int *)(g_sound_groups + 0x620);
  if ((VOICE_FIELD(slot, uchar, 0x34) & 2) == 0) {      /* not playing yet: start slightly above 22050 Hz */
    FUN_005b6a40(g_voices[slot].voice, 1);
    VOICE_FIELD(slot, float, 0x3c) = (float)(sin(k_f_n3_689349e19) * k_f_0_5dfcd8 + k_f_0_5df9b0);
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      FUN_005b7300(VOICE_FIELD(slot, int, 0x30), 4, 1, (float)(sin(k_f_n3_689349e19) * k_f_0_5dfcd8 + k_f_0_5df9b0));
    }
  }
  voice_set_volume(slot, 1.0f);
  VOICE_POSITION(slot) = *position;
  if (VOICE_FIELD(slot, int, 0x30) != 0) {
    FUN_005b72a0(VOICE_FIELD(slot, int, 0x30), 0, 1, position);
  }
}
