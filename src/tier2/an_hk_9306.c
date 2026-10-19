#include "mfl_tier2.h"
/*0046eee0*/ extern int sound_streams_are_idle();
/*0046f940*/ extern void __cdecl sound_ambient_pitch_randomise(int p0);
/*005b8980*/ extern void __cdecl FUN_005b8980(int p0, void *p1);
/*005b8c00*/ extern void __cdecl FUN_005b8c00(int p0, int p1);
/*005b8c40*/ extern int __cdecl FUN_005b8c40(int p0);
/*006ae07c*/ extern int g_sound_stream_handles[4];
/*008be7c0*/ extern int *g_sound_stream_slots[4];
/*008d9940*/ extern int g_sound_8d9940[4];
/* sound_stream_start_if_idle */

/* "starts the stream of a slot when streams are idle and state 0" */

undefined4 sound_stream_start_if_idle(int param_1)

{
  struct {
    int *self;
    int one;
    int value;
    int three;
    int zero;
    int two;
  } frame;
  
  if (g_sound_stream_slots[param_1] != 0) {
    if (sound_streams_are_idle() == 0) {
      if (g_sound_stream_slots[param_1][3] == 0) {
        if (FUN_005b8c40(g_sound_stream_handles[param_1]) != 6) {
          frame.self = &frame.value;
          frame.one = 1;
          frame.three = 3;
          frame.two = 2;
          frame.zero = 0;
          frame.value = g_sound_stream_slots[param_1][2];
          FUN_005b8980(g_sound_stream_handles[param_1],&frame);
          FUN_005b8c00(g_sound_stream_handles[param_1],2);
          g_sound_stream_slots[param_1][3] = 1;
          g_sound_8d9940[param_1] = 1;
          sound_ambient_pitch_randomise(0);
        }
      }
      return 1;
    }
  }
  return 0;
}
