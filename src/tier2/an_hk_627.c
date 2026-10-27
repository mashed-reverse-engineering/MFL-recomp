#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0046a0c0*/ extern int voice_set_volume(int p1, float p2);
/*004b2b48*/ extern int _ftol();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd330*/ extern float float_1_0;
/*005df99c*/ extern float k_f_22050;
/*005dfcb4*/ extern float k_f_10000;
/*00619ba0*/ extern undefined4 g_sound_619ba0;
/*00619c34*/ extern undefined4 g_bomb_update_619c34;
/*00619cc8*/ extern undefined4 g_bomb_update_619cc8;
/*006ae154*/ extern undefined4 g_bomb_update_6ae154;
/*006ae158*/ extern undefined4 g_bomb_update_6ae158;

/* state values passed as unused_1 (inferred from the branches) */
#define BOMB_STATE_START (1)
#define BOMB_STATE_PITCH_UP (2)
#define BOMB_STATE_STOP (3)

/* voice record offsets (inferred from use) */
#define VOICE_SUBOBJECT_OFFSET (0x30)
#define VOICE_FLAGS_OFFSET (0x34)
#define VOICE_FREQUENCY_OFFSET (0x3c)

/* voice flag bit: when set the voice is skipped (inferred) */
#define VOICE_FLAG_SKIP (2)

/* raw bits of 22050.0f, the base playback frequency written to voice+0x3c */
#define VOICE_BASE_FREQUENCY_BITS (0x46ac4400)
#define VOICE_BASE_FREQUENCY_BITS_F (22050.0f) /* the same value as a float argument of FUN_005b7300 (`float p3`; the integer literal would convert to 1.18e9) */

/* pitch-up waits PITCH_UP_TICKS_BASE + fraction * PITCH_UP_TICKS_PER_FRACTION ticks (inferred) */
#define PITCH_UP_TICKS_BASE (5.0f)
#define PITCH_UP_TICKS_PER_FRACTION (20.0f)

/* sound_bomb_countdown_voices */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "state 1/2/3: start/pitch-up/stop voices of the bomb countdown from the fraction" */

/*
 * Drives the three voices of the bomb countdown sound.
 * state BOMB_STATE_START: restarts the start voice at base frequency and full volume.
 * state BOMB_STATE_PITCH_UP: counts ticks; once the wait is over, sets the pitch slot's
 *   frequency to 22050 + 10000 * (1 - fraction) and its volume to (1 - fraction).
 * state BOMB_STATE_STOP: restarts the stop voice at base frequency and full volume.
 * fraction: countdown fraction, expected 0..1 (inferred). Returns nothing.
 * The voice-skip checks and the calls to FUN_005b6a40 / FUN_005b7300 are not named (unknown callees).
 */
void sound_bomb_countdown_voices(int state,float fraction)

{
  float pitch_hz;
  int stop_slot;
  undefined4 sub_result;
  int pitch_slot;
  int start_slot;

  stop_slot = g_bomb_update_619cc8;
  start_slot = g_bomb_update_619c34;
  pitch_slot = g_sound_619ba0;
  /* start: restart the start voice; pitch_slot and start_slot are reused as scratch below */
  if (state == BOMB_STATE_START) {
    if ((*(byte *)(g_voices[g_bomb_update_619c34].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_SKIP) == 0) {
      FUN_005b6a40(g_voices[g_bomb_update_619c34].voice,1);
      *(undefined4 *)(g_voices[start_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_BASE_FREQUENCY_BITS;
      pitch_slot = *(int *)(g_voices[start_slot].voice + VOICE_SUBOBJECT_OFFSET);
      sub_result = 0;
      if (pitch_slot != 0) {
        sub_result = FUN_005b7300(pitch_slot,4,1,VOICE_BASE_FREQUENCY_BITS_F);
      }
      voice_set_volume(start_slot,1.0);
      g_bomb_update_6ae154 = 0;
      g_bomb_update_6ae158 = 0;
      return;
    }
  }
  /* pitch-up: once the wait has elapsed (flag 6ae154 == 1), raise the pitch slot */
  else if (state == BOMB_STATE_PITCH_UP) {
    if (((*(byte *)(g_voices[g_sound_619ba0].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_SKIP) == 0) && (g_bomb_update_6ae154 == 1)
       ) {
      /* fraction is inverted here: from now on it holds (1 - fraction) */
      fraction = float_1_0 - fraction;
      pitch_hz = k_f_10000 * fraction + k_f_22050;
      FUN_005b6a40(g_voices[g_sound_619ba0].voice,1);
      *(float *)(g_voices[pitch_slot].voice + VOICE_FREQUENCY_OFFSET) = pitch_hz;
      start_slot = *(int *)(g_voices[pitch_slot].voice + VOICE_SUBOBJECT_OFFSET);
      if (start_slot != 0) {
        FUN_005b7300(start_slot,4,1,pitch_hz);
      }
      voice_set_volume(pitch_slot,fraction);
      g_bomb_update_6ae154 = 0;
      return;
    }
    /* wait: count this tick (start_slot reused as the counter), and flag the pitch-up once the target is reached */
    start_slot = g_bomb_update_6ae158 + 1;
    g_bomb_update_6ae158 = start_slot;
    /* pitch_slot reused as the tick target: PITCH_UP_TICKS_BASE + fraction * PITCH_UP_TICKS_PER_FRACTION */
    pitch_slot = ((int)((fraction * PITCH_UP_TICKS_PER_FRACTION) + PITCH_UP_TICKS_BASE));
    if (pitch_slot <= start_slot) {
      g_bomb_update_6ae158 = 0;
      g_bomb_update_6ae154 = 1;
      return;
    }
  }
  /* stop: restart the stop voice at base frequency and full volume */
  else if (state == BOMB_STATE_STOP) {
    if ((*(byte *)(g_voices[g_bomb_update_619cc8].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_SKIP) == 0) {
      FUN_005b6a40(g_voices[g_bomb_update_619cc8].voice,1);
      *(undefined4 *)(g_voices[stop_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_BASE_FREQUENCY_BITS;
      pitch_slot = *(int *)(g_voices[stop_slot].voice + VOICE_SUBOBJECT_OFFSET);
      sub_result = 0;
      if (pitch_slot != 0) {
        sub_result = FUN_005b7300(pitch_slot,4,1,VOICE_BASE_FREQUENCY_BITS_F);
      }
      voice_set_volume(stop_slot,1.0);
    }
  }
  return;
}

