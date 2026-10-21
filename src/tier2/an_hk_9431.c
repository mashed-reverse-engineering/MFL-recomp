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
/* sound_endpoint_panel_progress_cues */

/* "state machine 0..4 on the endpoint panel value (0.34, 0.82, 1.3, 1.86), plays the cue at the
   last step" */

void sound_endpoint_panel_progress_cues(void)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 pu_eax;
  float local_4;
  
  iVar3 = race_get_phase();
  if (iVar3 != 5) {
    return;
  }
  iVar3 = get_event_type();
  if (iVar3 == 0x12) {
    return;
  }
  endpoint_panel_get_value(&local_4);
  iVar2 = g_sound_frame_619700;
  iVar3 = g_sound_frame_6191cc;
  switch(g_sound_endpoint_cue_state) {
  case 0:
    if (local_4 < k_f_0_34) {
      return;
    }
    g_sound_endpoint_cue_state = 1;
    goto LAB_0046f7eb;
  case 1:
    bVar1 = k_f_0_82 <= local_4;
    if (bVar1) {
      g_sound_endpoint_cue_state = 2;
    }
    if (local_4 < k_f_0_34) {
      g_sound_endpoint_cue_state = 0;
    }
    break;
  case 2:
    bVar1 = k_f_1_3 <= local_4;
    if (bVar1) {
      g_sound_endpoint_cue_state = 3;
    }
    if (local_4 < k_f_0_82) {
      g_sound_endpoint_cue_state = 1;
    }
    break;
  case 3:
    if (k_f_1_86 <= local_4) {
      g_sound_endpoint_cue_state = 4;
    }
    if (local_4 < k_f_1_3) {
      g_sound_endpoint_cue_state = 2;
    }
    if (k_f_1_86 > local_4) {
      return;
    }
    if ((*(byte *)(g_voices[g_sound_frame_619700].voice + 0x34) & 2) != 0) {
      return;
    }
    FUN_005b6a40(g_voices[g_sound_frame_619700].voice,1);
    *(undefined4 *)(g_voices[iVar2].voice + 0x3c) = 0x46ac4400;
    iVar4 = *(int *)(g_voices[iVar2].voice + 0x30);
    iVar3 = iVar2;
    goto LAB_0046f8d8;
  case 4:
    if (k_f_1_86 <= local_4) {
      return;
    }
  default:
    g_sound_endpoint_cue_state = 0;
    return;
  }
  if (bVar1) {
LAB_0046f7eb:
    if ((*(byte *)(g_voices[g_sound_frame_6191cc].voice + 0x34) & 2) == 0) {
      FUN_005b6a40(g_voices[g_sound_frame_6191cc].voice,1);
      *(undefined4 *)(g_voices[iVar3].voice + 0x3c) = 0x46ac4400;
      iVar4 = *(int *)(g_voices[iVar3].voice + 0x30);
LAB_0046f8d8:
      pu_eax = 0;
      if (iVar4 != 0) {
        pu_eax = FUN_005b7300(iVar4,4,1,22050.0f);
      }
      voice_set_volume(iVar3,0.6);
    }
  }
  return;
}


