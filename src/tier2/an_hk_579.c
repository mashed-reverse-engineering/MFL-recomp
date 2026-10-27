#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*006187f8*/ extern undefined4 g_sfx_table;
/*006187fc*/ extern undefined4 g_sound_6187fc;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*0076e7f4*/ extern undefined4 g_app_state;
/*008d98bc*/ extern int g_sound_player_count;
/*008d9930*/ extern undefined4 g_sfx_play_8d9930;

/* Effect table: one row of SFX_TABLE_STRIDE words per effect id. */
#define SFX_TABLE_STRIDE (0x25)
/* Effect silenced when multiplayer mutes sound (inferred). */
#define SFX_ID_MULTIPLAYER_MUTED (0x20) /* the only effect id silenced by the multiplayer mute (inferred) */
/* Effect whose plays are counted in g_sfx_play_8d9930. */
#define SFX_ID_PLAY_COUNT (0x1c)
/* Default voice frequency, bit pattern of 22050.0f. */
#define DEFAULT_SFX_FREQ (0x46ac4400)
#define DEFAULT_SFX_FREQ_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */
/* Byte offsets into a voice record (struct not in mfl_tier2.h). */
#define VOICE_OFF_SUB_OBJ (0x30)
#define VOICE_OFF_FLAGS (0x34)
#define VOICE_OFF_FREQ (0x3c)
#define VOICE_OFF_POS_X (0x58)
#define VOICE_OFF_POS_Y (0x5c)
#define VOICE_OFF_POS_Z (0x60)

/* sfx_play_at */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(effect, position*): when g_app_state is 3, starts every voice of the effect (table g_sfx_table:
   +0 first voice, +4 count) at the position with frequency 22050.0 (0x46AC4400) unless an effect
   0x20 special case applies" */
/* effect_id: index of the sound effect (row of g_sfx_table). position: three floats (x, y, z) copied into each started voice.
   Returns nothing. voice_slot and voices_left are each reused for two values, see the comments at their uses. */

void sfx_play_at(int effect_id,undefined4 *position)

{
  char *voice;
  int voice_slot;
  undefined4 freq_set_result;
  int voices_left;

  /* Play unless (multiplayer mute is on, 2+ players and effect_id == SFX_ID_MULTIPLAYER_MUTED),
     and only when get_app_state() is APP_STATE_RACE.
     voice_slot holds the app state for this check. */
  if ((((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) || (effect_id != SFX_ID_MULTIPLAYER_MUTED)) &&
     (voice_slot = get_app_state(), voice_slot == APP_STATE_RACE)) {
    /* From here voice_slot is the first voice slot of the effect; voices_left is the number of voices to start. */
    voice_slot = (&g_sfx_table)[effect_id * SFX_TABLE_STRIDE];
    for (voices_left = (&g_sound_6187fc)[effect_id * SFX_TABLE_STRIDE]; 0 < voices_left; voices_left = voices_left + -1) {
      /* Start the voice only if flag bit 2 is clear (inferred: voice already busy). voices_left is reused here for the sub-object handle at +0x30. */
      voice = g_voices[voice_slot].voice;
      if ((*(byte *)(voice + VOICE_OFF_FLAGS) & 2) == 0) {
        *(undefined4 *)(voice + VOICE_OFF_POS_X) = *position;
        *(undefined4 *)(voice + VOICE_OFF_POS_Y) = position[1];
        *(undefined4 *)(voice + VOICE_OFF_POS_Z) = position[2];
        voices_left = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB_OBJ);
        if (voices_left != 0) {
          FUN_005b72a0(voices_left,0,1,position);
        }
        FUN_005b6a40(g_voices[voice_slot].voice,1);
        *(undefined4 *)(g_voices[voice_slot].voice + VOICE_OFF_FREQ) = DEFAULT_SFX_FREQ;
        voices_left = *(int *)(g_voices[voice_slot].voice + VOICE_OFF_SUB_OBJ);
        freq_set_result = 0;
        if (voices_left != 0) {
          freq_set_result = FUN_005b7300(voices_left,4,1,DEFAULT_SFX_FREQ_F);
        }
        voice_set_volume(voice_slot,1.0);
        voices_left = 0;
      }
      voice_slot = voice_slot + 1;
    }
    /* Count plays of effect SFX_ID_PLAY_COUNT. */
    if (effect_id == SFX_ID_PLAY_COUNT) {
      g_sfx_play_8d9930 = g_sfx_play_8d9930 + 1;
    }
  }
  return;
}


