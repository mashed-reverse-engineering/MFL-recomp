#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dde40*/ extern float k_f_70;
/*005ddea0*/ extern float k_f_20;
/*005df99c*/ extern float k_f_22050;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*008bd7b0*/ extern undefined4 g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* Byte offsets into a voice record (g_voices[i].voice); the meanings are inferred from how the code uses them. */
#define VOICE_FLAGS_OFFSET (0x34)
#define VOICE_FLAG_ACTIVE (2) /* inferred: voice is already started / playing */
#define VOICE_CHILD_OFFSET (0x30) /* inferred: pointer to a sub-voice, 0 if none */
#define VOICE_FREQ_OFFSET (0x3c) /* inferred: float playback frequency, 22050 +/- jitter */
#define VOICE_POS_X_OFFSET (0x58) /* inferred: position vector, x, y, z */
#define VOICE_POS_Y_OFFSET (0x5c)
#define VOICE_POS_Z_OFFSET (0x60)

/* Byte offsets into g_sound_groups: each entry is a voice slot index, entries are SOUND_GROUP_STRIDE bytes apart (inferred). */
#define SOUND_GROUP_STRIDE (0x90)
#define SOUND_GROUP_BANK_A_OFFSET (0x470) /* 4 random entries */
#define SOUND_GROUP_BANK_B_OFFSET (0x6b0) /* 3 random entries */
#define SOUND_GROUP_FIXED_0 (0x860)
#define SOUND_GROUP_FIXED_1 (0x8f0)
#define SOUND_GROUP_FIXED_2 (0x980)
#define SOUND_GROUP_FIXED_3 (0xa10)
#define SOUND_GROUP_FIXED_4 (0xaa0)

#define COURSE_SUPERG_N (8) /* inferred: course_id is a CourseId */

/* sound_ambience_course_8_19 */

/* "course ids 0x08 and 0x19" */

/*
 * Plays the random ambience one-shots for the current course. Each call rolls a few chances and,
 * on a hit, picks a voice slot from the sound groups, sets its position (x, y, z) and volume, and
 * starts it (plus its sub-voice, if any). Does nothing when multiplayer mute is on and 2+ players
 * are present.
 * course_id: current course id (inferred; the extra block below is skipped for COURSE_SUPERG_N).
 * Returns nothing.
 */
void __cdecl sound_ambience_course_8_19(int course_id)

{
  float pos_x;
  int slot;
  char *voice;
  undefined4 child_status;
  int child;
  float10 random_value;
  float position [3];
  
  /* skip everything when multiplayer mute is on and there are 2+ players */
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    /* roll 1: pick a random slot from bank A and start it at a random position */
    slot = rp_random_int_range(0,0x32);
    if (slot == 0x19) {
      slot = rp_random_int_range(0,3);
      slot = *(int *)(slot * SOUND_GROUP_STRIDE + SOUND_GROUP_BANK_A_OFFSET + g_sound_groups);
      if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
        FUN_005b6a40(g_voices[slot].voice,1);
      }
      random_value = (float10)random_range(0,0x461c4000);
      *(float *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = (float)(random_value + (float10)k_f_22050);
      voice = g_voices[slot].voice;
      if (*(int *)(voice + VOICE_CHILD_OFFSET) != 0) {
        random_value = (float10)random_range(0,0x461c4000);
        voice = FUN_005b7300(*(undefined4 *)(g_voices[slot].voice + VOICE_CHILD_OFFSET),4,1,
                             (float)(random_value + (float10)k_f_22050));
      }
      voice_set_volume(slot,1.0);
      random_value = (float10)random_range(0,0x42200000);
      pos_x = (float)(random_value - (float10)k_f_20);
      random_value = (float10)random_range(0,0x430c0000);
      voice = g_voices[slot].voice;
      position[2] = (float)(random_value - (float10)k_f_70);
      position[1] = -1.0;
      *(float *)(voice + VOICE_POS_X_OFFSET) = pos_x;
      *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0xbf800000;
      *(float *)(voice + VOICE_POS_Z_OFFSET) = position[2];
      slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
      position[0] = pos_x;
      if (slot != 0) {
        FUN_005b72a0(slot,0,1,position);
      }
    }
    /* roll 2: pick a random slot from bank B, only when not on COURSE_SUPERG_N */
    if (course_id != COURSE_SUPERG_N) {
      slot = rp_random_int_range(0,0x19);
      if (slot == 0x19) {
        slot = rp_random_int_range(0,2);
        slot = *(int *)(slot * SOUND_GROUP_STRIDE + SOUND_GROUP_BANK_B_OFFSET + g_sound_groups);
        if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
          FUN_005b6a40(g_voices[slot].voice,1);
        }
        *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
        child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
        child_status = 0;
        if (child != 0) {
          child_status = FUN_005b7300(child,4,1,22050.0f);
        }
        voice_set_volume(slot,1.0);
        random_value = (float10)random_range(0,0x42200000);
        voice = g_voices[slot].voice;
        position[1] = 2.0;
        position[2] = -70.0;
        position[0] = (float)(random_value - (float10)k_f_20);
        *(float *)(voice + VOICE_POS_X_OFFSET) = position[0];
        *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x40000000;
        *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0xc28c0000;
        slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
        if (slot != 0) {
          FUN_005b72a0(slot,0,1,position);
        }
      }
    }
    /* fixed one-shots: sound group entries FIXED_0 to FIXED_4, each started with its own position */
    slot = *(int *)(g_sound_groups + SOUND_GROUP_FIXED_0);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x463c4800;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (child != 0) {
      child_status = FUN_005b7300(child,4,1,12050.0f);
    }
    voice_set_volume(slot,1.0);
    voice = g_voices[slot].voice;
    position[0] = -13.5;
    position[1] = 0.6;
    position[2] = -3.22;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0xc1580000;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x3f19999a;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0xc04e147b;
    slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,position);
    }
    slot = *(int *)(g_sound_groups + SOUND_GROUP_FIXED_1);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (child != 0) {
      child_status = FUN_005b7300(child,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice = g_voices[slot].voice;
    position[0] = 8.9;
    position[1] = 1.3;
    position[2] = 69.6;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0x410e6666;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x3fa66666;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0x428b3333;
    slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,position);
    }
    slot = *(int *)(g_sound_groups + SOUND_GROUP_FIXED_2);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (child != 0) {
      child_status = FUN_005b7300(child,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice = g_voices[slot].voice;
    position[0] = 0.0;
    position[1] = 2.0;
    position[2] = -74.0;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x40000000;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0xc2940000;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    if (child != 0) {
      FUN_005b72a0(child,0,1,position);
    }
    child = slot + 1;
    if ((*(byte *)(g_voices[slot + 1].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot + 1].voice,1);
    }
    *(undefined4 *)(g_voices[child].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
    slot = *(int *)(g_voices[child].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (slot != 0) {
      child_status = FUN_005b7300(slot,4,1,22050.0f);
    }
    voice_set_volume(child,1.0);
    voice = g_voices[child].voice;
    position[0] = 8.9;
    position[1] = 1.3;
    position[2] = 69.6;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0x410e6666;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x3fa66666;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0x428b3333;
    slot = *(int *)(g_voices[child].voice + VOICE_CHILD_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,position);
    }
    slot = *(int *)(g_sound_groups + SOUND_GROUP_FIXED_3);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (child != 0) {
      child_status = FUN_005b7300(child,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice = g_voices[slot].voice;
    position[0] = -3.86;
    position[1] = 6.716;
    position[2] = 24.4;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0xc0770a3d;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x40d6e979;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0x41c33333;
    slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,position);
    }
    slot = *(int *)(g_sound_groups + SOUND_GROUP_FIXED_4);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_FREQ_OFFSET) = 0x46ac4400;
    child = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    child_status = 0;
    if (child != 0) {
      child_status = FUN_005b7300(child,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice = g_voices[slot].voice;
    position[0] = 4.7;
    position[1] = 12.6;
    position[2] = -71.5;
    *(undefined4 *)(voice + VOICE_POS_X_OFFSET) = 0x40966666;
    *(undefined4 *)(voice + VOICE_POS_Y_OFFSET) = 0x4149999a;
    *(undefined4 *)(voice + VOICE_POS_Z_OFFSET) = 0xc28f0000;
    slot = *(int *)(g_voices[slot].voice + VOICE_CHILD_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,position);
    }
  }
  return;
}


