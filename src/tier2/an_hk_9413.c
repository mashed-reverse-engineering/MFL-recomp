#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*0061a168*/ extern undefined4 g_sound_music_voice;
/* sound_voices_stop_all_but_music */

/* "stops every one of the 0xD2 voices except g_sound_61a168" */

void sound_voices_stop_all_but_music(void)

{
  int iVar1;
  
  iVar1 = 0;
  do {
    if (iVar1 != g_sound_music_voice) {
      FUN_005b6a40(g_voices[iVar1].voice,0);
    }
    iVar1 = iVar1 + 1;
  } while (iVar1 < 0xd2);
  return;
}


