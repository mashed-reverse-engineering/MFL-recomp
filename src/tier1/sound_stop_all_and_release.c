#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0046f6b0*/ extern int sound_streams_destroy();
/*005b66f0*/ extern void FUN_005b66f0(void);
/*005b6860*/ extern void FUN_005b6860(void);
/*005b6a80*/ extern void FUN_005b6a80(undefined4 p0, undefined4 p1);
/*005b77f0*/ extern void FUN_005b77f0(void);
/*005b81c0*/ extern void FUN_005b81c0(undefined4 p0);
/*005baf80*/ extern void FUN_005baf80(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ccf30*/ extern void FUN_005ccf30(void);
/*006ad118*/ extern undefined4 g_sound_6ad118;
/*006ad1e8*/ extern undefined4 g_sound_6ad1e8;
/*006ad8a4*/ extern undefined4 g_sound_6ad8a4;
/*006ad8a8*/ extern undefined4 g_sound_6ad8a8;
/*006ad8f0*/ extern undefined4 g_sound_6ad8f0;
/*006ae09c*/ extern undefined4 g_sound_6ae09c;
/*006ae0bc*/ extern undefined4 g_sound_6ae0bc;
/*006ae0c0*/ extern undefined4 g_sound_6ae0c0;
#define VOICE_SLOT_SIZE (8) /* sizeof(struct VoiceSlot): voice +0x0, volume +0x4 */
#define VOICE_TABLE_BYTES (0x690) /* g_voices: 210 slots * VOICE_SLOT_SIZE */
/* sound_stop_all_and_release */
/* "stops all voices and releases the sound buffers" */
/*
 * Stops every sound voice and releases the sound buffers (inferred from the callees).
 * Takes no parameters and returns nothing. The first block runs only when g_sound_6ae09c is set.
 */
void sound_stop_all_and_release(void)
{
  uint index; /* loop index: element index in the count loops, byte offset in the voice-table loop */

  if (g_sound_6ae09c != 0) {
    /* for each of the g_sound_6ad8a4 active voices: FUN_005b6a80(voice, 0) (inferred: stop) */
    index = 0;
    if (g_sound_6ad8a4 > 0) {
      do {
        FUN_005b6a80(g_voices[index].voice,0);
        index = index + 1;
      } while (index < g_sound_6ad8a4);
    }
    /* for each of the g_sound_6ad8a8 entries in g_sound_6ad118: thunk_FUN_005bc0a0 (inferred: release) */
    index = 0;
    if (g_sound_6ad8a8 > 0) {
      do {
        thunk_FUN_005bc0a0((&g_sound_6ad118)[index]);
        index = index + 1;
      } while (index < g_sound_6ad8a8);
    }
    FUN_005b81c0(g_sound_6ae09c);
  }
  /* walk the voice table by byte offset, one VOICE_SLOT_SIZE step per slot: FUN_005baf80 on each slot's voice */
  index = 0;
  do {
    FUN_005baf80(*(undefined4 *)((char *)&g_voices[0].voice + index),0,0);
    index = index + VOICE_SLOT_SIZE;
  } while (index < VOICE_TABLE_BYTES);
  FUN_005b77f0();
  /* for each of the g_sound_6ae0c0 entries in g_sound_6ad8f0: FUN_005baf80(entry, 0, 0) */
  index = 0;
  if (g_sound_6ae0c0 > 0) {
    do {
      FUN_005baf80((&g_sound_6ad8f0)[index],0,0);
      index = index + 1;
    } while (index < g_sound_6ae0c0);
  }
  /* stream subsystem shut-down, then the two single handles, then the remaining teardown calls */
  sound_streams_destroy();
  FUN_005baf80(g_sound_6ae0bc,0,0);
  FUN_005baf80(g_sound_6ad1e8,0,0);
  FUN_005ccf30();
  FUN_005b66f0();
  FUN_005b6860();
  return;
}
