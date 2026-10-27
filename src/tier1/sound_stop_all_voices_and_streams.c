#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00469a30*/ extern int sound_voices_stop_all_but_music();
/*00556950*/ extern RwBool __cdecl RpSplineDestroy(struct RpSpline *p0);
/*005b6a80*/ extern void FUN_005b6a80(undefined4 p0, undefined4 p1);
/*005b81c0*/ extern void FUN_005b81c0(undefined4 p0);
/*006ad084*/ extern undefined4 g_game_6ad084;
/*006ad088*/ extern undefined4 g_sound_6ad088;
/*006ad1e4*/ extern char *g_sound_6ad1e4;
/*006ad1f8*/ extern undefined4 g_sound_6ad1f8;
/*006ad8a4*/ extern undefined4 g_sound_6ad8a4;
/*006ae098*/ extern undefined4 g_sound_6ae098;
/*008bd7b4*/ extern undefined4 g_sound_8bd7b4;
/* sound_stop_all_voices_and_streams */
/* "stops all non-music voices and streams when active" */
/* Stops all sound except background music, but only when the stream in g_sound_6ad1f8 is
   set (inferred: that gate covers the whole first block). Takes no parameters, returns nothing.
   Order: sound_voices_stop_all_but_music(); stop voice records g_voices[g_sound_6ad8a4 ..
   g_sound_8bd7b4); stop channel handles from g_sound_6ad088; close the stream and clear
   g_sound_6ad1f8. Then, separately, destroy the spline in g_sound_6ad1e4 if set and clear it.
   `current` holds a voice index in the first loop and a handle in the second (reused). */
void sound_stop_all_voices_and_streams(void)
{
  int current;
  uint channel;

  if (g_sound_6ad1f8 != 0) {
    sound_voices_stop_all_but_music();
    /* Voice records from g_sound_6ad8a4 up to (not including) g_sound_8bd7b4. */
    current = g_sound_6ad8a4;
    if ((int)g_sound_6ad8a4 < (int)g_sound_8bd7b4) {
      do {
        FUN_005b6a80(g_voices[current].voice,0);
        current = current + 1;
      } while (current < (int)g_sound_8bd7b4);
    }
    /* Channel handles 0 .. g_sound_6ae098 - 1. Channel 0 is always stopped; the others
       only when the handle differs from g_game_6ad084[channel] (inferred: that entry is
       a handle the game keeps alive). */
    channel = 0;
    if (0 < (uint)g_sound_6ae098) {
      do {
        if ((int)channel > 0) {
          current = (&g_sound_6ad088)[channel];
          if (current != (&g_game_6ad084)[channel]) {
            thunk_FUN_005bc0a0(current);
          }
        }
        else {
          thunk_FUN_005bc0a0((&g_sound_6ad088)[channel]);
        }
        channel = channel + 1;
      } while (channel < (uint)g_sound_6ae098);
    }
    FUN_005b81c0(g_sound_6ad1f8);
    g_sound_6ad1f8 = 0;
  }
  /* Destroy the spline, if any, and clear the pointer. */
  if (g_sound_6ad1e4 != 0) {
    RpSplineDestroy((struct RpSpline *)g_sound_6ad1e4);
    g_sound_6ad1e4 = (char *)(RpSpline *)0x0;
  }
  return;
}
