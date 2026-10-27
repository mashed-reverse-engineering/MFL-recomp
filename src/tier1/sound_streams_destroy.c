#include "mfl_tier2.h"
/*0046ef30*/ extern int sound_stub_return_0();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*00561120*/ extern void RtFSManagerSyncAllFiles();
/*005baf80*/ extern void FUN_005baf80(undefined4 p0, undefined4 p1, undefined4 p2);
/*00617958*/ extern undefined4 g_soundengine_617958;
/*006ae07c*/ extern undefined4 g_sound_stream_handles;
/*006ae0e4*/ extern char *g_sound_6ae0e4;
/*006ae110*/ extern undefined4 race_audio_active;
/*008be7c0*/ extern char *g_sound_stream_slots;
/*008d9940*/ extern char *g_sound_8d9940;
/*006ae08c*/ extern char g_sound_streams_6ae08c[];
/* byte size of the per-slot tables walked by sound_streams_destroy (inferred: 4-byte entries, 6 slots) */
#define STREAM_SLOT_TABLE_BYTES (0x18)
/* sound_streams_destroy */
/* "'DestroyStream': syncs the file manager and destroys the 4 streams" */
/* Destroys all sound streams and resets their slot bookkeeping at race teardown.
 * Takes no parameters, returns nothing. Does nothing unless the sound engine is not stubbed
 * out (sound_stub_return_0() == 0) and race audio is active (race_audio_active == 1).
 * Afterwards race_audio_active is cleared and g_soundengine_617958 is set to 1 (inferred: "streams destroyed" flag). */
void sound_streams_destroy(void)
{
  int slot_offset;
  undefined4 *handle_ptr;

  /* sound_stub_return_0() result is tested first; the same local is reused below as a byte offset (inferred) */
  slot_offset = sound_stub_return_0();
  if ((slot_offset == 0) && (race_audio_active == 1)) {
    FID_conflict__wprintf
              (
              "\\mashed_fully_loaded\\toast_us\\TOAST\\CODE\\SRC\\APPCODE\\AUDIO\\soundengine.c: DestroyStreams\n"
              );
    RtFSManagerSyncAllFiles();
    /* Walk the stream handle array up to g_sound_streams_6ae08c, calling FUN_005baf80 on each handle (inferred: stop/close) */
    handle_ptr = &g_sound_stream_handles;
    do {
      FUN_005baf80(*handle_ptr,0,0);
      handle_ptr = handle_ptr + 1;
    } while ((LONG_PTR)handle_ptr < ((LONG_PTR)&g_sound_streams_6ae08c));
    /* Walk the slot tables in 4-byte steps. For each occupied slot clear the word at +0xc of the
     * object it points to (purpose unknown), then clear the matching entries of the two tables */
    slot_offset = 0;
    do {
      if (*(int *)((char *)&g_sound_stream_slots + slot_offset) != 0) {
        *(undefined4 *)(*(char **)((char *)&g_sound_stream_slots + slot_offset) + 0xc) = 0;
        *(undefined4 *)((char *)&g_sound_8d9940 + slot_offset) = 0;
      }
      *(undefined4 *)((char *)&g_sound_6ae0e4 + slot_offset) = 0;
      slot_offset = slot_offset + 4;
    } while (slot_offset < STREAM_SLOT_TABLE_BYTES);
    race_audio_active = 0;
    g_soundengine_617958 = 1;
  }
  return;
}
