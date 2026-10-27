#include "mfl_tier2.h"
/*005b8c40*/ extern int sound_stream_state(int p0);
/*005b8c50*/ extern int __cdecl FUN_005b8c50(int a1);
/*006ae07c*/ extern undefined4 g_sound_stream_handles;
/*006ae080*/ extern char g_sound_6ae080[];
/* sound_stream_state() return value for a stream that is playing (inferred from the comment below) */
#define SOUND_STREAM_PLAYING (2)
/* last byte offset from g_sound_6ae080 inside the 4-slot stream handle table (table starts at 0x6ae07c) */
#define STREAM_HANDLE_TABLE_LAST_OFS (0xb)
/* sound_streams_are_idle */
/*
 * Returns 1 if the sound streams are idle, 0 if they are busy (inferred).
 * Takes no parameters. Walks the 4 stream handles at 0x6AE07C; returns 0 only when every slot
 * is occupied and none of them hits an early exit (inferred: a stream is still active).
 * Original note: "true when no stream handle at 0x6AE07C is in the playing state"
 */
undefined4 sound_streams_are_idle(void)
{
  int state;
  int *handle_ptr;

  handle_ptr = &g_sound_stream_handles;
  /* walk the handle table, one 4-byte slot per iteration */
  do {
    /* empty slot: nothing more to check */
    if (*handle_ptr == 0) {
      return 1;
    }
    /* first status query; 0 ends the check as idle (meaning of FUN_005b8c50 unknown) */
    state = FUN_005b8c50(*handle_ptr);
    if (state == 0) {
      return 1;
    }
    /* second status query on the same handle; 2 ends the check as idle */
    state = FUN_005b8c50(*handle_ptr);
    if (state == 2) {
      return 1;
    }
    /* stream state check: a playing stream ends the walk (inferred) */
    state = sound_stream_state(*handle_ptr);
    if (state == SOUND_STREAM_PLAYING) {
      return 1;
    }
    handle_ptr = handle_ptr + 1;
  } while ((LONG_PTR)handle_ptr < ((LONG_PTR)&g_sound_6ae080 + STREAM_HANDLE_TABLE_LAST_OFS + 1));
  return 0;
}
