#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*00413b10*/ extern int replay_current_get_split_time();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*0061966c*/ extern undefined4 g_sound_frame_61966c;
/*006ae100*/ extern undefined4 g_sound_frame_6ae100;
/*008d98bc*/ extern int g_sound_player_count;

#define MULTIPLAYER_MIN_PLAYERS (2)
#define VOICE_FLAGS_OFF (0x34) /* byte flags in the voice record (inferred) */
#define VOICE_FLAG_BUSY (2) /* bit set while the voice is in use (inferred) */
#define VOICE_SUB_OFF (0x30) /* pointer to the voice's sub-record, passed to FUN_005b7300 (inferred) */
#define BEEP_VOLUME (0.5)

/* sound_replay_split_beep */

/* "plays the beep voice when a replay split time becomes current (single player)"
 * Called once per frame (inferred). Takes no parameters and returns nothing.
 * Asks replay_current_get_split_time() for a split; g_sound_frame_6ae100 is the split cursor and
 * is advanced each time one is found. The voice slot is g_sound_frame_61966c.
 */

void sound_replay_split_beep(void)

{
  int slot;
  int result; /* split-found flag, then the voice sub-record pointer */
  undefined4 tone_ret; /* return value of FUN_005b7300, unused */
  struct {
    undefined1 local_c [4];
    undefined1 local_8 [4];
    undefined1 local_4 [4];
  } split_args; /* output arguments of replay_current_get_split_time */

  slot = g_sound_frame_61966c;
  /* only in single player, and only when a split time was returned */
  if ((g_sound_player_count < MULTIPLAYER_MIN_PLAYERS) &&
     (result = replay_current_get_split_time(g_sound_frame_6ae100,split_args.local_4,split_args.local_8,split_args.local_c),
     result != 0)) {
    g_sound_frame_6ae100 = g_sound_frame_6ae100 + 1;
    /* start the beep only if the voice is not already busy */
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFF) & VOICE_FLAG_BUSY) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
      *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x47188a00;
      result = *(int *)(g_voices[slot].voice + VOICE_SUB_OFF);
      tone_ret = 0;
      /* play the tone only if the voice has a sub-record */
      if (result != 0) {
        tone_ret = FUN_005b7300(result,4,1,39050.0f);
      }
      voice_set_volume(slot,BEEP_VOLUME);
    }
  }
  return;
}


