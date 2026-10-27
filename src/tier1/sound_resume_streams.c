#include "mfl_tier2.h"
/*004687d0*/ extern int voice_queue_slots_clear();
/*005b8c00*/ extern void __cdecl FUN_005b8c00(int p0, int p1);
/*005b8c40*/ extern int __cdecl FUN_005b8c40(int p0);
/*006ae07c*/ extern int g_sound_stream_handles[4];
/*006ae0e4*/ extern int g_sound_6ae0e4[4];
/*008be7c0*/ extern int *g_sound_stream_slots[4];
/* sound_resume_streams */
/* "restarts the streams that were stopped (inferred)" */
undefined4 sound_resume_streams(void)
{
  undefined4 result;
  int i;
  
  result = 0;
  i = 0;
  do {
    if ((g_sound_stream_slots[i] != 0) && (g_sound_stream_slots[i][3] == 1)) {
      if (FUN_005b8c40(g_sound_stream_handles[i]) != 3) {
        FUN_005b8c00(g_sound_stream_handles[i],1);
        g_sound_6ae0e4[i] = 6;
        result = 1;
      }
    }
    i = i + 1;
  } while (i < 4);
  voice_queue_slots_clear();
  return result;
}
