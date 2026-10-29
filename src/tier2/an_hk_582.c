#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*006187f8*/ extern undefined4 g_sfx_table;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;

#define SFX_TABLE_STRIDE (0x25) /* voice slots per sfx type in g_sfx_table (inferred) */
#define SFX_TYPE_MULTIPLAYER_MUTED (0x20) /* sfx type silenced by the multiplayer mute (inferred) */

/* sound_sfx_set_volume_clamped */

/* "sets the volume of a playing sfx voice clamped to [0,1]"
 *
 * sfx_type:    sfx type id; selects the voice block in g_sfx_table.
 * voice_index: index of the voice inside that block; the global slot is
 *              g_sfx_table[sfx_type * SFX_TABLE_STRIDE] + voice_index.
 * volume:      requested volume. Values above 1.0 are set to 1.0, values
 *              below 0.0 are set to 0.0. NaN is passed through unchanged.
 *
 * Does nothing when the voice is not active, when the game is not in the
 * racing state, or when multiplayer mute applies to this sfx type.
 * Returns nothing; calls voice_set_volume() at most once.
 */

void sound_sfx_set_volume_clamped(int sfx_type,int voice_index,float volume)

{
  int app_state;
  undefined2 voice_hi;

  /* Skip everything only when multiplayer mute is on, 2+ players are present
     and this is the muted sfx type. */
  if (((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) || (sfx_type != SFX_TYPE_MULTIPLAYER_MUTED)) {
    app_state = get_app_state();
    /* Only update voices while the game is in the racing state. */
    if (app_state == APP_STATE_RACE) {
      voice_index = (&g_sfx_table)[sfx_type * SFX_TABLE_STRIDE] + voice_index;
      /* Only voices with flag bit 0x2 set at +0x34 of the voice record (inferred: active). */
      if ((*(byte *)(g_voices[voice_index].voice + 0x34) & 2) != 0) {
        /* High 16 bits of the voice pointer. Read but never used. */
        voice_hi = (undefined2)(g_voices[voice_index].voice >> 0x10);
        /* Volume above 1.0: set full volume and stop. */
        if (volume > float_1_0) {
          voice_set_volume(voice_index,1.0);
          return;
        }
        /* Clamp negative volume to 0.0. */
        if (volume < k_f_0) {
          volume = 0.0;
        }
        voice_set_volume(voice_index,volume);
      }
    }
  }
  return;
}


