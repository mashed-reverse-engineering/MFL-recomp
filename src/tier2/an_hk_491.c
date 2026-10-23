#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004a20c0*/ extern int get_app_state();
/*005b6a40*/ extern int FUN_005b6a40(void *p0, undefined4 p1);
/*006187f8*/ extern undefined4 g_sfx_table;
#define SFX_TABLE_STRIDE (0x25) /* (inferred) words per effect in g_sfx_table */
#define VOICE_FLAGS_OFFSET (0x34) /* byte offset of the flags in the voice record */
#define VOICE_FLAG_PLAYING (2) /* (inferred) */
/* sfx_voice_is_active */

/* "(effect, voice): voice of the effect is playing (only in state 3)" */

/*
 * Silences one voice of a sound effect, but only while the game is in the
 * playing state. effect_id selects the effect's block in g_sfx_table, and
 * voice_index is the voice's index within that block. If the voice's record
 * has VOICE_FLAG_PLAYING set, its volume is set to 0 and it is stopped via
 * FUN_005b6a40 (inferred: stop). Returns nothing.
 */
void sfx_voice_is_active(int effect_id,int voice_index)

{
  int app_state;

  app_state = get_app_state();
  /* Only act while the game is in the playing state. */
  if (app_state == APP_STATE_RACE) {
    /* Turn effect id + voice index into a global voice slot index. */
    voice_index = (&g_sfx_table)[effect_id * SFX_TABLE_STRIDE] + voice_index;
    /* Voice is playing: mute it and stop it. */
    if ((*(byte *)(g_voices[voice_index].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_PLAYING) != 0) {
      voice_set_volume(voice_index,0.0);
      FUN_005b6a40(g_voices[voice_index].voice,0);
    }
  }
  return;
}


