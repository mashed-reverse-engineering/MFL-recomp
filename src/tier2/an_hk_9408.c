#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*004a20c0*/ extern int get_app_state();
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, void *p3);
/*006187f8*/ extern int g_sfx_table[];
/* sfx_move_playing_voice */

/* "(effect, position*, voice index): updates the position of a voice that is already playing (flag
   2)" */

void sfx_move_playing_voice(int param_1, undefined4 *param_2, int param_3)

{
  char *voice;
  int app_state;
  
  app_state = get_app_state();
  if (app_state == 3) {
    app_state = g_sfx_table[param_1 * 0x25];
    voice = *(char **)&g_voices[app_state + param_3].voice;
    if ((*(byte *)(voice + 0x34) & 2) != 0) {
      *(undefined4 *)(voice + 0x58) = *param_2;
      *(undefined4 *)(voice + 0x5c) = param_2[1];
      *(undefined4 *)(voice + 0x60) = param_2[2];
      app_state = *(int *)(*(char **)&g_voices[app_state + param_3].voice + 0x30);
      if (app_state != 0) {
        FUN_005b72a0(app_state,0,1,param_2);
      }
    }
  }
  return;
}


