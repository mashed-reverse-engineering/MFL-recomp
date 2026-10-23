#include "mfl_tier2.h"
extern char s_333_Lap_frames_005df368[];
extern struct VoiceSlot g_voices[210];
/*00410040*/ extern int race_course_loaded_event_get();
/*00410050*/ extern int race_course_loaded_event_clear();
/*00434cf0*/ extern int ui_get_menu_69d6a8();
/*00434d00*/ extern int ui_get_message_69d6ac();
/*00434d10*/ extern int ui_get_menu_69d6b0();
/*00437750*/ extern double option_get_volume_0();
/*00437790*/ extern double option_get_volume_2();
/*00437e80*/ extern int menu_is_page_13_selected_1();
/*00437eb0*/ extern int menu_is_page_13_selected_2();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*006190a4*/ extern undefined4 g_sound_frame_6190a4;
/*00619138*/ extern undefined4 g_sound_frame_619138;
/*00619ba0*/ extern undefined4 g_sound_619ba0;
/*00619fac*/ extern undefined4 g_sound_frame_619fac;

/* voice record offsets (no voice struct in ghidra_types.h; channel/flags names inferred) */
#define VOICE_CHANNEL_OFFSET (0x30) /* inferred: handle passed to FUN_005b7300 when non-zero */
#define VOICE_FLAGS_OFFSET (0x34) /* byte field */
#define VOICE_FREQUENCY_OFFSET (0x3c) /* playback frequency in Hz (float) */
#define VOICE_FLAG_PLAYING (2) /* inferred: set while the voice is playing */

/* playback frequencies in Hz: the float bits written to the record, and the same value passed to FUN_005b7300 */
#define VOICE_FREQUENCY_DEFAULT (22050.0f)
#define VOICE_FREQUENCY_DEFAULT_BITS (0x46ac4400) /* 22050.0f */
#define VOICE_FREQUENCY_MESSAGE (19845.0f)
#define VOICE_FREQUENCY_MESSAGE_BITS (0x469b0a00) /* 19845.0f */

/* sound_menu_voices_trigger */

/*
 * Starts the menu and event voices whose UI or event trigger is active.
 * Each trigger reads a voice slot index from one of the g_sound_frame_* / g_sound_619ba0 globals
 * (0x619138, 0x6190a4, 0x619ba0, 0x619fac), then for that slot's voice record: calls FUN_005b6a40
 * (inferred: restart), sets the playback frequency, sets the channel handle via FUN_005b7300
 * (args 4 and 1 unknown) when the record has one, and sets the volume.
 * Takes no parameters, returns nothing.
 * voice_volume = option_get_volume_0() * the float at 0x5df368 (Ghidra label s_333_Lap_frames, used as a float scale).
 * option_volume: scratch float, first option_get_volume_0() (for voice_volume), then option_get_volume_2()
 * (applied at the end to the slot from g_sound_frame_619fac if it is playing).
 * (was: "starts the menu/event voices (0x619138, 0x6190a4, 0x619ba0) scaled by the option volume")
 */
void sound_menu_voices_trigger(void)

{
  float voice_volume;
  int voice_slot;
  int check_result;
  undefined4 fx_result;
  uint voice_addr;
  double option_volume;

  option_volume = (double)option_get_volume_0();
  voice_volume = (float)(option_volume * (double)*(float *)((char *)&s_333_Lap_frames_005df368 + 0));
  option_volume = (double)option_get_volume_2();
  voice_slot = g_sound_frame_619138;
  /* menu UI 69d6a8 open: voice on slot g_sound_frame_619138 at VOICE_FREQUENCY_DEFAULT */
  check_result = ui_get_menu_69d6a8();
  if (check_result != 0) {
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_DEFAULT_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    fx_result = 0;
    if (check_result != 0) {
      fx_result = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_DEFAULT);
    }
    voice_set_volume(voice_slot,voice_volume);
  }
  /* race course loaded event: same voice as above, then consume the event */
  check_result = race_course_loaded_event_get();
  if (check_result != 0) {
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_DEFAULT_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    fx_result = 0;
    if (check_result != 0) {
      fx_result = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_DEFAULT);
    }
    voice_set_volume(voice_slot,voice_volume);
    race_course_loaded_event_clear();
  }
  voice_slot = g_sound_frame_6190a4;
  /* message UI 69d6ac open: slot g_sound_frame_6190a4 at VOICE_FREQUENCY_MESSAGE */
  check_result = ui_get_message_69d6ac();
  if (check_result != 0) {
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_MESSAGE_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    fx_result = 0;
    if (check_result != 0) {
      fx_result = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_MESSAGE);
    }
    voice_set_volume(voice_slot,voice_volume);
  }
  /* menu UI 69d6b0 open: slot g_sound_frame_6190a4 at VOICE_FREQUENCY_DEFAULT */
  check_result = ui_get_menu_69d6b0();
  if (check_result != 0) {
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_DEFAULT_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    fx_result = 0;
    if (check_result != 0) {
      fx_result = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_DEFAULT);
    }
    voice_set_volume(voice_slot,voice_volume);
  }
  voice_slot = g_sound_619ba0;
  /* page 13 selected (1): slot g_sound_619ba0, only when that voice is not already playing */
  check_result = menu_is_page_13_selected_1();
  if ((check_result != 0) && ((*(byte *)(g_voices[voice_slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_PLAYING) == 0)) {
    FUN_005b6a40(g_voices[voice_slot].voice,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_DEFAULT_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    fx_result = 0;
    if (check_result != 0) {
      fx_result = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_DEFAULT);
    }
    voice_set_volume(voice_slot,voice_volume);
  }
  voice_slot = g_sound_frame_619fac;
  /* page 13 selected (2): slot g_sound_frame_619fac, starts it only when not playing; volume is set at the end */
  check_result = menu_is_page_13_selected_2();
  voice_addr = 0;
  if ((check_result != 0) && (voice_addr = g_voices[voice_slot].voice, (*(byte *)(voice_addr + VOICE_FLAGS_OFFSET) & VOICE_FLAG_PLAYING) == 0)) {
    FUN_005b6a40(voice_addr,1);
    *(undefined4 *)(g_voices[voice_slot].voice + VOICE_FREQUENCY_OFFSET) = VOICE_FREQUENCY_DEFAULT_BITS;
    check_result = *(int *)(g_voices[voice_slot].voice + VOICE_CHANNEL_OFFSET);
    voice_addr = 0;
    if (check_result != 0) {
      voice_addr = FUN_005b7300(check_result,4,1,VOICE_FREQUENCY_DEFAULT);
    }
  }
  /* slot from g_sound_frame_619fac: if it is playing, apply the option_volume_2 volume */
  if ((*(byte *)(g_voices[voice_slot].voice + VOICE_FLAGS_OFFSET) & VOICE_FLAG_PLAYING) != 0) {
    voice_set_volume(voice_slot,(float)option_volume);
  }
  return;
}


