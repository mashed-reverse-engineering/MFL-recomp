#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0044a4f0*/ extern int camera_get_position();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dec0c*/ extern float k_f_47;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*008bd7b0*/ extern undefined4 g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* offsets into g_sound_groups (inferred) */
#define SOUND_GROUP_A_OFF (0x470)
#define SOUND_GROUP_B_OFF (0x740)
#define SOUND_GROUP_RANDOM_BASE (0x500)
#define SOUND_GROUP_RANDOM_STRIDE (0x90)
/* voice record offsets and flags (inferred) */
#define VOICE_OFF_SUB (0x30)
#define VOICE_OFF_FLAGS (0x34)
#define VOICE_FLAG_STARTED (0x2)
#define VOICE_OFF_POS_X (0x58)
#define VOICE_OFF_POS_Y (0x5c)
#define VOICE_OFF_POS_Z (0x60)
/* random roll: hit when the roll equals AMBIENCE_ROLL_HIT (inferred) */
#define AMBIENCE_ROLL_SIDES (0x32)
#define AMBIENCE_ROLL_HIT (0x19)
/* ambience source positions and volume (inferred) */
#define AMBIENCE_VOLUME_QUIET (0.3)
#define AMBIENCE_POS_X (39.7)
#define AMBIENCE_POS_Y (-20.3)
#define AMBIENCE_POS_Z (31.5)
#define AMBIENCE_RAND_POS_Y (-11.0)
#define AMBIENCE_RAND_POS_Z (-17.0)

/* sound_ambience_course_12_18
 * Starts the ambience sound voices for one frame. Takes no parameters, returns nothing.
 * Two fixed bed voices are set up on every call: group A quiet (volume AMBIENCE_VOLUME_QUIET,
 * no position written), group B full volume at the source position (AMBIENCE_POS_X/Y/Z). With a 1 in AMBIENCE_ROLL_SIDES roll (inferred) a
 * third voice is also started from one of two random sound groups, at its own position.
 * Muted when g_sound_mute_when_multiplayer is set and 2+ players are present.
 * The original note said course ids 0x12 and 0x18; this function does not check the
 * course id itself, so the caller (inferred) selects it by course. */

void sound_ambience_course_12_18(void)

{
  int sub_handle;
  char *voice_rec;
  undefined4 sub_result;
  int voice_slot;
  float10 rand_value;
  struct {
    float local_18 [3];
    undefined1 local_c [12];
  } ambience_pos;
  
  /* Skip everything when multiplayer mute is on and 2+ players are present. */
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    /* Read the camera position into the tail of ambience_pos (not used after). */
    camera_get_position(ambience_pos.local_c);
    /* Bed voice 1: group A slot, quiet (no position set). */
    voice_slot = *(int *)(g_sound_groups + SOUND_GROUP_A_OFF);
    if ((*(byte *)(g_voices[voice_slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[voice_slot].voice,1);
    }
    *(undefined4 *)(g_voices[voice_slot].voice + 0x3c) = 0x46ac4400;
    sub_handle = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB);
    sub_result = 0;
    if (sub_handle != 0) {
      sub_result = FUN_005b7300(sub_handle,4,1,22050.0f);
    }
    voice_set_volume(voice_slot,AMBIENCE_VOLUME_QUIET);
    /* Bed voice 2: group B slot, full volume, at the fixed source position. */
    voice_slot = *(int *)(g_sound_groups + SOUND_GROUP_B_OFF);
    if ((*(byte *)(g_voices[voice_slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[voice_slot].voice,1);
    }
    *(undefined4 *)(g_voices[voice_slot].voice + 0x3c) = 0x46ac4400;
    sub_handle = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB);
    sub_result = 0;
    if (sub_handle != 0) {
      sub_result = FUN_005b7300(sub_handle,4,1,22050.0f);
    }
    voice_set_volume(voice_slot,1.0);
    voice_rec = g_voices[voice_slot].voice;
    ambience_pos.local_18[0] = AMBIENCE_POS_X;
    ambience_pos.local_18[1] = AMBIENCE_POS_Y;
    ambience_pos.local_18[2] = AMBIENCE_POS_Z;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_X) = 0x421ecccd;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = 0xc1a26666;
    *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = 0x41fc0000;
    voice_slot = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB);
    if (voice_slot != 0) {
      FUN_005b72a0(voice_slot,0,1,ambience_pos.local_18);
    }
    /* Random extra voice: fires only when the roll equals AMBIENCE_ROLL_HIT (inferred),
       then picks one of two random-group slots (inferred: 0 or 1). */
    voice_slot = rp_random_int_range(0,AMBIENCE_ROLL_SIDES);
    if (voice_slot == AMBIENCE_ROLL_HIT) {
      voice_slot = rp_random_int_range(0,1);
      voice_slot = *(int *)(voice_slot * SOUND_GROUP_RANDOM_STRIDE + SOUND_GROUP_RANDOM_BASE + g_sound_groups);
      if ((*(byte *)(g_voices[voice_slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_STARTED) == 0) {
        FUN_005b6a40(g_voices[voice_slot].voice,1);
      }
      *(undefined4 *)(g_voices[voice_slot].voice + 0x3c) = 0x46ac4400;
      sub_handle = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB);
      sub_result = 0;
      if (sub_handle != 0) {
        sub_result = FUN_005b7300(sub_handle,4,1,22050.0f);
      }
      voice_set_volume(voice_slot,1.0);
      rand_value = (float10)random_range(0,0x41200000);
      voice_rec = g_voices[voice_slot].voice;
      ambience_pos.local_18[1] = AMBIENCE_RAND_POS_Y;
      ambience_pos.local_18[2] = AMBIENCE_RAND_POS_Z;
      ambience_pos.local_18[0] = (float)(rand_value - (float10)k_f_47);
      *(float *)(voice_rec + VOICE_OFF_POS_X) = ambience_pos.local_18[0];
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Y) = 0xc1300000;
      *(undefined4 *)(voice_rec + VOICE_OFF_POS_Z) = 0xc1880000;
      voice_slot = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB);
      if (voice_slot != 0) {
        FUN_005b72a0(voice_slot,0,1,ambience_pos.local_18);
      }
    }
  }
  return;
}


