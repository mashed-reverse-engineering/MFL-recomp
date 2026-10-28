#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*00420b10*/ extern int endpoint_panel_get_value();
/*00434bb0*/ extern int get_event_type();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005ddf54*/ extern float k_f_1_86;
/*005de3a0*/ extern float k_f_1_3;
/*005de3a4*/ extern float k_f_0_82;
/*005de3a8*/ extern float k_f_0_34;
/*006191cc*/ extern undefined4 g_sound_frame_6191cc;
/*00619700*/ extern undefined4 g_sound_frame_619700;
/*006ae104*/ extern undefined4 g_sound_endpoint_cue_state;
/* voice record offsets (same as sound_ambience_course_12_18) */

/* sound_endpoint_panel_progress_cues */

/* "state machine 0..4 on the endpoint panel value (0.34, 0.82, 1.3, 1.86), plays the cue at the
   last step" */

void sound_endpoint_panel_progress_cues(void)

{
  bool reached_next_step;
  int voice_b_index;
  int scratch;
  int sub_handle;
  undefined4 sub_result;
  float panel_value;
  
  scratch = race_get_phase();
  if (scratch != 5) {
    return;
  }
  scratch = get_event_type();
  if (scratch == EVENT_QUICK_RACE) {
    return;
  }
  endpoint_panel_get_value(&panel_value);
  voice_b_index = g_sound_frame_619700;
  scratch = g_sound_frame_6191cc;
  switch(g_sound_endpoint_cue_state) {
  case 0:
    if (panel_value < k_f_0_34) {
      return;
    }
    g_sound_endpoint_cue_state = 1;
    goto LAB_0046f7eb;
  case 1:
    reached_next_step = k_f_0_82 <= panel_value;
    if (reached_next_step) {
      g_sound_endpoint_cue_state = 2;
    }
    if (panel_value < k_f_0_34) {
      g_sound_endpoint_cue_state = 0;
    }
    break;
  case 2:
    reached_next_step = k_f_1_3 <= panel_value;
    if (reached_next_step) {
      g_sound_endpoint_cue_state = 3;
    }
    if (panel_value < k_f_0_82) {
      g_sound_endpoint_cue_state = 1;
    }
    break;
  case 3:
    reached_next_step = k_f_1_86 <= panel_value;
    if (reached_next_step) {
      g_sound_endpoint_cue_state = 4;
    }
    if (panel_value < k_f_1_3) {
      g_sound_endpoint_cue_state = 2;
    }
    if (!reached_next_step) {
      return;
    }
    if ((*(byte *)(g_voices[g_sound_frame_619700].voice + VOICE_OFF_FLAGS) & 2) != 0) {
      return;
    }
    FUN_005b6a40(g_voices[g_sound_frame_619700].voice,1);
    *(undefined4 *)(g_voices[voice_b_index].voice + VOICE_OFF_FREQ) = VOICE_DEFAULT_FREQ_BITS;
    sub_handle = *(int *)(g_voices[voice_b_index].voice + VOICE_OFF_HANDLE);
    scratch = voice_b_index;
    goto LAB_0046f8d8;
  case 4:
    if (k_f_1_86 <= panel_value) {
      return;
    }
  default:
    g_sound_endpoint_cue_state = 0;
    return;
  }
  if (reached_next_step) {
LAB_0046f7eb:
    if ((*(byte *)(g_voices[g_sound_frame_6191cc].voice + VOICE_OFF_FLAGS) & 2) == 0) {
      FUN_005b6a40(g_voices[g_sound_frame_6191cc].voice,1);
      *(undefined4 *)(g_voices[scratch].voice + VOICE_OFF_FREQ) = VOICE_DEFAULT_FREQ_BITS;
      sub_handle = *(int *)(g_voices[scratch].voice + VOICE_OFF_HANDLE);
LAB_0046f8d8:
      sub_result = 0;
      if (sub_handle != 0) {
        sub_result = FUN_005b7300(sub_handle,4,1,22050.0f);
      }
      voice_set_volume(scratch,0.6);
    }
  }
  return;
}


