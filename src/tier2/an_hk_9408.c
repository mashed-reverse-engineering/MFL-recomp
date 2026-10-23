#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*004a20c0*/ extern int get_app_state();
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/* layout of *voice in sfx_move_playing_voice, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct sfx_move_playing_voice_voice_layout {
    char pad_0x0[52];
    byte f_0x34;
    char pad_0x35[35];
    undefined4 f_0x58;
    undefined4 f_0x5c;
    undefined4 f_0x60;
} sfx_move_playing_voice_voice_layout;

/*006187f8*/ extern int g_sfx_table[];
/* sfx_move_playing_voice */

/* "(effect, position*, voice index): updates the position of a voice that is already playing (flag
   2)" */

void sfx_move_playing_voice(int param_1, undefined4 *param_2, int param_3)

{
  sfx_move_playing_voice_voice_layout *voice;
  int app_state;
  
  app_state = get_app_state();
  if (app_state == APP_STATE_RACE) {
    app_state = g_sfx_table[param_1 * 0x25];
    voice = (sfx_move_playing_voice_voice_layout *)(*(char **)&g_voices[app_state + param_3].voice);
    if ((voice->f_0x34 & 2) != 0) {
      voice->f_0x58 = *param_2;
      voice->f_0x5c = param_2[1];
      voice->f_0x60 = param_2[2];
      app_state = *(int *)(*(char **)&g_voices[app_state + param_3].voice + 0x30);
      if (app_state != 0) {
        FUN_005b72a0(app_state,0,1,param_2);
      }
    }
  }
  return;
}


