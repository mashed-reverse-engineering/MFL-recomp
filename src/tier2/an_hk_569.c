#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00473aa0*/ extern char *sound_get_6af57c();
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005de290*/ extern float float_50_0;
/*005deb58*/ extern float k_f_74;
/*005df99c*/ extern float k_f_22050;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*008bd7b0*/ extern char *g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* Voice record offsets (inferred from use) */
#define VOICE_FLAG_ACTIVE (2)
#define VOICE_RATE (0x3c)
/* Sound group table in g_sound_groups (inferred) */
#define SOUND_GROUP_BASE (0x500)
#define SOUND_GROUP_STRIDE (0x90)
/* Float bit patterns and roll ranges (inferred from the calls they appear in) */
#define VOICE_RATE_DEFAULT_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */
#define RATE_JITTER_MAX (0x461c4000) /* 10000.0f */
#define OFFSET_X_RANGE (0x42c80000) /* 100.0f, minus float_50_0 */
#define OFFSET_Z_RANGE (0x42940000) /* 74.0f, minus k_f_74 */
#define ROLL_A_MAX (0x1e)
#define ROLL_A_HIT (0x19)
#define ROLL_B_MAX (0xf)
#define ROLL_B_HIT (5)
/* sound_ambience_course_9_22 */
/* Per-frame ambience update for the course (inferred from the name and the calls).
 * Does nothing when ambience is muted for multiplayer and there are 2+ players.
 * Otherwise it refreshes three voices in g_sound_groups (slot at +0x470, slot at +0x6b0,
 * and the slot after that) with a rate and a position, then sometimes starts a random
 * group voice (roll A, roll B). Takes no parameters, returns nothing.
 * Note: the locals are reused by the decompiler, so `slot` also holds the random rolls and
 * `voice_rec` is briefly set to the FUN_005b7300 result (unused). */

/* "course ids 0x09 and 0x22" */

void sound_ambience_course_9_22(void)

{
  float offset_x;
  undefined4 fx_result;
  int slot;
  char *voice_rec;
  undefined4 *sound_pos;
  int iVar6;
  double rand_val;
  float offset [3];

  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    /* Voice A: rate VOICE_DEFAULT_FREQ_BITS, volume 0.5 (inferred). */
    slot = SOUND_GROUP_FIRST_VOICE_OF(7);
    if ((*(byte *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_RATE) = VOICE_DEFAULT_FREQ_BITS;
    iVar6 = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
    fx_result = 0;
    if (iVar6 != 0) {
      fx_result = FUN_005b7300(iVar6,4,1,VOICE_RATE_DEFAULT_F);
    }
    voice_set_volume(slot,0.5);
    /* Roll A: 1 in (ROLL_A_MAX + 1) chance of a random group voice with a jittered rate and a
     * random offset. Position is (offset_x, 0, offset[2]) (inferred). */
    slot = rp_random_int_range(0,ROLL_A_MAX);
    if (slot == ROLL_A_HIT) {
      slot = rp_random_int_range(0,2);
      slot = *(int *)(slot * SOUND_GROUP_STRIDE + SOUND_GROUP_BASE + g_sound_groups);
      if ((*(byte *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
        FUN_005b6a40(g_voices[slot].voice,1);
      }
      rand_val = (double)random_range(0,RATE_JITTER_MAX);
      *(float *)(g_voices[slot].voice + VOICE_RATE) = (float)(rand_val + (double)k_f_22050);
      voice_rec = g_voices[slot].voice;
      if (*(int *)(voice_rec + VOICE_OFF_HANDLE) != 0) {
        rand_val = (double)random_range(0,RATE_JITTER_MAX);
        voice_rec = FUN_005b7300(*(undefined4 *)(g_voices[slot].voice + VOICE_OFF_HANDLE),4,1,
                             (float)(rand_val + (double)k_f_22050));
      }
      voice_set_volume(slot,1.0);
      rand_val = (double)random_range(0,OFFSET_X_RANGE);
      offset_x = (float)(rand_val - (double)float_50_0);
      rand_val = (double)random_range(0,OFFSET_Z_RANGE);
      voice_rec = g_voices[slot].voice;
      offset[2] = (float)(rand_val - (double)k_f_74);
      offset[1] = 0.0;
      *(float *)(voice_rec + VOICE_OFF_POS_X) = offset_x;
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = 0;
      *(float *)(voice_rec + VOICE_OFF_POS_Z) = offset[2];
      slot = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
      offset[0] = offset_x;
      if (slot != 0) {
        FUN_005b72a0(slot,0,1,offset);
      }
    }
    /* Voice B: rate VOICE_DEFAULT_FREQ_BITS, volume 1.0, fixed position (1.32, -7.7, -30.7) (inferred). */
    slot = SOUND_GROUP_FIRST_VOICE_OF(11);
    if ((*(byte *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + VOICE_RATE) = VOICE_DEFAULT_FREQ_BITS;
    iVar6 = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
    fx_result = 0;
    if (iVar6 != 0) {
      fx_result = FUN_005b7300(iVar6,4,1,VOICE_RATE_DEFAULT_F);
    }
    voice_set_volume(slot,1.0);
    voice_rec = g_voices[slot].voice;
    offset[0] = 1.32;
    offset[1] = -7.7;
    offset[2] = -30.7;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_X) = 0x3fa8f5c3;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = 0xc0f66666;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = 0xc1f5999a;
    iVar6 = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
    if (iVar6 != 0) {
      FUN_005b72a0(iVar6,0,1,offset);
    }
    /* Voice C (slot + 1): volume 1.0, fixed position (11.0, -5.7, -61.7) (inferred). */
    iVar6 = slot + 1;
    if ((*(byte *)(g_voices[slot + 1].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
      FUN_005b6a40(g_voices[slot + 1].voice,1);
    }
    *(undefined4 *)(g_voices[iVar6].voice + VOICE_RATE) = VOICE_DEFAULT_FREQ_BITS;
    slot = *(int *)(g_voices[iVar6].voice + VOICE_OFF_HANDLE);
    fx_result = 0;
    if (slot != 0) {
      fx_result = FUN_005b7300(slot,4,1,VOICE_RATE_DEFAULT_F);
    }
    voice_set_volume(iVar6,1.0);
    voice_rec = g_voices[iVar6].voice;
    offset[0] = 11.0;
    offset[1] = -5.7;
    offset[2] = -61.7;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_X) = 0x41300000;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = 0xc0b66666;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = 0xc276cccd;
    slot = *(int *)(g_voices[iVar6].voice + VOICE_OFF_HANDLE);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,offset);
    }
    /* Roll B: 1 in (ROLL_B_MAX + 1) chance of a random group voice placed at the position from
     * sound_get_6af57c() (inferred). */
    slot = rp_random_int_range(0,ROLL_B_MAX);
    if (slot == ROLL_B_HIT) {
      slot = rp_random_int_range(0,2);
      slot = *(int *)(slot * SOUND_GROUP_STRIDE + SOUND_GROUP_BASE + g_sound_groups);
      if ((*(byte *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_ACTIVE) == 0) {
        FUN_005b6a40(g_voices[slot].voice,1);
      }
      *(undefined4 *)(g_voices[slot].voice + VOICE_RATE) = VOICE_DEFAULT_FREQ_BITS;
      iVar6 = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
      fx_result = 0;
      if (iVar6 != 0) {
        fx_result = FUN_005b7300(iVar6,4,1,VOICE_RATE_DEFAULT_F);
      }
      voice_set_volume(slot,1.0);
      sound_pos = (undefined4 *)sound_get_6af57c();
      voice_rec = g_voices[slot].voice;
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_X) = *sound_pos;
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = sound_pos[1];
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = sound_pos[2];
      slot = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
      if (slot != 0) {
        FUN_005b72a0(slot,0,1,sound_pos);
      }
    }
  }
  return;
}


