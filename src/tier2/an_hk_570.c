#include "mfl_tier2.h"
/* typed view of VoiceSlot (header: uint voice): record pointer as a real pointer, same 8 byte layout on x86 */
struct VoiceSlotP { char *voice; float volume; };
extern struct VoiceSlotP g_voices[210];
/*0044a4f0*/ extern int camera_get_position();
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*00481fd0*/ extern double random_range();
/*004c45c0*/ extern int rp_random_int_range(int p0, int p1);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b72a0*/ extern int FUN_005b72a0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd418*/ extern float k_f_12;
/*005dd61c*/ extern float k_f_0_001;
/*005df5c0*/ extern float k_f_49;
/*005df9a0*/ extern float k_f_3_14;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ae1e4*/ extern float g_sound_ambience_6ae1e4;
/*008bd7b0*/ extern undefined4 g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;

/* voice record layout (inferred from use): handle, flags, position */
#define VOICE_HANDLE_OFFSET (0x30)
#define VOICE_FLAGS_OFFSET (0x34)
#define VOICE_FLAG_STARTED (2)
#define VOICE_POS_X_OFFSET (0x58)
#define VOICE_POS_Y_OFFSET (0x5c)
#define VOICE_POS_Z_OFFSET (0x60)
/* ambience: multiplayer gate and random one-shot emitter (inferred) */
#define MULTIPLAYER_MIN_PLAYERS (2)
#define RANDOM_EMITTER_ROLL (0x19)
#define RANDOM_EMITTER_ROLL_RANGE (100)
#define RANDOM_EMITTER_BASE (2000)
#define RANDOM_EMITTER_STRIDE (0x90)
/* sound_ambience_course_1a */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "course id 0x1A" */
/*
 * Per-frame update of the ambience emitters for course 0x1A (COURSE_CITY in the
 * enum table, inferred from the course id string). Takes no parameters, returns nothing.
 * Does nothing when multiplayer muting is on and 2 or more players are present.
 * Each emitter voice is looked up through g_sound_groups, started if its flag bit
 * is clear, given a volume, and given a world position (x, y, z) in the voice record.
 */
void sound_ambience_course_1a(void)

{
  float emitter_x;
  char *voice_ptr;
  int voice_handle;
  undefined4 handle_call_result; /* FUN_005b7300 result, 0 if no handle; never read */
  int slot;
  int scratch; /* reused: slot + 1 index, then the voice handle */
  float10 random_offset;
  /* frame: local_18 = emitter position (x, y, z), local_c = camera position (never read) */
  struct {
    float local_18 [3];
    undefined1 local_c [12];
  } frame;
  
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < MULTIPLAYER_MIN_PLAYERS)) {
    /* camera position is fetched into frame.local_c but not used (inferred leftover) */
    camera_get_position(frame.local_c);
    /* timer g_sound_ambience_6ae1e4 advances each call; wraps to 0 past k_f_3_14 (inferred cycle) */
    g_sound_ambience_6ae1e4 = g_sound_ambience_6ae1e4 + k_f_0_001;
    if (k_f_3_14 < g_sound_ambience_6ae1e4) {
      g_sound_ambience_6ae1e4 = 0.0;
    }
    /* group 0x590: volume 0.2, no position written */
    slot = *(int *)(g_sound_groups + 0x590);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,0.2);
    /* group 0x470: three emitters in a row at x=36.7, y=2.6, z=-13.1 / -19.1 / -26.1 (slot, slot+1, slot+2) */
    slot = *(int *)(g_sound_groups + 0x470);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = 36.7;
    frame.local_18[1] = 2.6;
    frame.local_18[2] = -13.1;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x4212cccd;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x40266666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc151999a;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (scratch != 0) {
      FUN_005b72a0(scratch,0,1,frame.local_18);
    }
    scratch = slot + 1;
    if ((*(byte *)(g_voices[slot + 1].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot + 1].voice,1);
    }
    *(undefined4 *)(g_voices[scratch].voice + 0x3c) = 0x46ac4400;
    voice_handle = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (voice_handle != 0) {
      handle_call_result = FUN_005b7300(voice_handle,4,1,22050.0f);
    }
    voice_set_volume(scratch,1.0);
    voice_ptr = g_voices[scratch].voice;
    frame.local_18[0] = 36.7;
    frame.local_18[1] = 2.6;
    frame.local_18[2] = -19.1;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x4212cccd;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x40266666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc198cccd;
    scratch = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    if (scratch != 0) {
      FUN_005b72a0(scratch,0,1,frame.local_18);
    }
    scratch = slot + 2;
    if ((*(byte *)(g_voices[slot + 2].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot + 2].voice,1);
    }
    *(undefined4 *)(g_voices[scratch].voice + 0x3c) = 0x46ac4400;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (slot != 0) {
      handle_call_result = FUN_005b7300(slot,4,1,22050.0f);
    }
    voice_set_volume(scratch,1.0);
    voice_ptr = g_voices[scratch].voice;
    frame.local_18[0] = 36.7;
    frame.local_18[1] = 2.6;
    frame.local_18[2] = -26.1;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x4212cccd;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x40266666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc1d0cccd;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
    /* group 0x500: emitter at (85.6, 0.3, 21.97) */
    slot = *(int *)(g_sound_groups + 0x500);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = 85.6;
    frame.local_18[1] = 0.3;
    frame.local_18[2] = 21.97;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x42ab3333;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3e99999a;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0x41afc28f;
    slot = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
    /* group 0x6b0: emitter at (96.6, 0.8, -22.8) */
    slot = *(int *)(g_sound_groups + 0x6b0);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = 96.6;
    frame.local_18[1] = 0.8;
    frame.local_18[2] = -22.8;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x42c13333;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3f4ccccd;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc1b66666;
    slot = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
    /* group 0x740: emitter at (-1.06, 0.6, -29.32) */
    slot = *(int *)(g_sound_groups + 0x740);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = -1.06;
    frame.local_18[1] = 0.6;
    frame.local_18[2] = -29.32;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0xbf87ae14;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3f19999a;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc1ea8f5c;
    slot = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
    /* group 0x620: emitter at (36.7, 3.3, 42.5), and slot+1 at (96.7, 1.3, 14.2) */
    slot = *(int *)(g_sound_groups + 0x620);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = 36.7;
    frame.local_18[1] = 3.3;
    frame.local_18[2] = 42.5;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x4212cccd;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x40533333;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0x422a0000;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (scratch != 0) {
      FUN_005b72a0(scratch,0,1,frame.local_18);
    }
    scratch = slot + 1;
    if ((*(byte *)(g_voices[slot + 1].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot + 1].voice,1);
    }
    *(undefined4 *)(g_voices[scratch].voice + 0x3c) = 0x46b41400;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (slot != 0) {
      handle_call_result = FUN_005b7300(slot,4,1,23050.0f);
    }
    voice_set_volume(scratch,1.0);
    voice_ptr = g_voices[scratch].voice;
    frame.local_18[0] = 96.7;
    frame.local_18[1] = 1.3;
    frame.local_18[2] = 14.2;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x42c16666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3fa66666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0x41633333;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
    /* rare roll (inferred one-shot): on a hit, play one random emitter at a random x/z, y = 0 */
    slot = rp_random_int_range(0,RANDOM_EMITTER_ROLL_RANGE);
    if (slot == RANDOM_EMITTER_ROLL) {
      /* pick one of the two random emitter groups */
      slot = rp_random_int_range(0,1);
      slot = *(int *)(slot * RANDOM_EMITTER_STRIDE + RANDOM_EMITTER_BASE + g_sound_groups);
      if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
        FUN_005b6a40(g_voices[slot].voice,1);
      }
      *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
      scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
      handle_call_result = 0;
      if (scratch != 0) {
        handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
      }
      voice_set_volume(slot,1.0);
      /* x = random in [0, 17.0) + k_f_49; z = random in [0, 70.0) - k_f_12 */
      random_offset = (float10)random_range(0,0x41880000);
      emitter_x = (float)(random_offset + (float10)k_f_49);
      random_offset = (float10)random_range(0,0x428c0000);
      voice_ptr = g_voices[slot].voice;
      frame.local_18[2] = (float)(random_offset - (float10)k_f_12);
      frame.local_18[1] = 0.0;
      *(float *)(voice_ptr + VOICE_POS_X_OFFSET) = emitter_x;
      *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0;
      *(float *)(voice_ptr + VOICE_POS_Z_OFFSET) = frame.local_18[2];
      slot = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
      frame.local_18[0] = emitter_x;
      if (slot != 0) {
        FUN_005b72a0(slot,0,1,frame.local_18);
      }
    }
    /* group 0x8f0: emitters at (63.6, 0.5, -28) and slot+1 at (77.0, 0.5, -28) */
    slot = *(int *)(g_sound_groups + 0x8f0);
    if ((*(byte *)(g_voices[slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot].voice,1);
    }
    *(undefined4 *)(g_voices[slot].voice + 0x3c) = 0x46ac4400;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (scratch != 0) {
      handle_call_result = FUN_005b7300(scratch,4,1,22050.0f);
    }
    voice_set_volume(slot,1.0);
    voice_ptr = g_voices[slot].voice;
    frame.local_18[0] = 63.6;
    frame.local_18[1] = 0.5;
    frame.local_18[2] = -28.0;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x427e6666;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3f000000;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc1e00000;
    scratch = *(int *)(g_voices[slot].voice + VOICE_HANDLE_OFFSET);
    if (scratch != 0) {
      FUN_005b72a0(scratch,0,1,frame.local_18);
    }
    scratch = slot + 1;
    if ((*(byte *)(g_voices[slot + 1].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_STARTED) == 0) {
      FUN_005b6a40(g_voices[slot + 1].voice,1);
    }
    *(undefined4 *)(g_voices[scratch].voice + 0x3c) = 0x46ac4400;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    handle_call_result = 0;
    if (slot != 0) {
      handle_call_result = FUN_005b7300(slot,4,1,22050.0f);
    }
    voice_set_volume(scratch,1.0);
    voice_ptr = g_voices[scratch].voice;
    frame.local_18[0] = 77.0;
    frame.local_18[1] = 0.5;
    frame.local_18[2] = -28.0;
    *(undefined4 *)(voice_ptr + VOICE_POS_X_OFFSET) = 0x429a0000;
    *(undefined4 *)(voice_ptr + VOICE_POS_Y_OFFSET) = 0x3f000000;
    *(undefined4 *)(voice_ptr + VOICE_POS_Z_OFFSET) = 0xc1e00000;
    slot = *(int *)(g_voices[scratch].voice + VOICE_HANDLE_OFFSET);
    if (slot != 0) {
      FUN_005b72a0(slot,0,1,frame.local_18);
    }
  }
  return;
}


