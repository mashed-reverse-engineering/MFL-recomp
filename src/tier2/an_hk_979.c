#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00430ad0*/ extern int ui_state_is_0();
/*0046a0c0*/ extern int voice_set_volume(int voice, float volume);
/*005dd330*/ extern float float_1_0;
/*005ddb00*/ extern float k_f_0_025;
/*005e64a4*/ extern float k_f_0;
/*00617940*/ extern float g_sound_global_fade;
/*008bd7b4*/ extern undefined4 g_sound_8bd7b4;
/* sound_global_fade_update */

/* "fades g_sound_frame_617940 by 0.025 per step by ui state and applies it to all voice volumes" */

void sound_global_fade_update(void)

{
  int iVar1;
  
  iVar1 = ui_state_is_0();
  if (iVar1 == 0) {
    g_sound_global_fade = g_sound_global_fade - k_f_0_025;
  }
  else {
    g_sound_global_fade = g_sound_global_fade + float_1_0;
  }
  /* original: fcomp / test ah,0x41 / jne and fcomp / test ah,5 / jp: a NaN fade passes both clamps unchanged */
  if (g_sound_global_fade > float_1_0) {
    g_sound_global_fade = 1.0;
    return;
  }
  if (g_sound_global_fade < k_f_0) {
    g_sound_global_fade = 0.0;
    return;
  }
  if (((g_sound_global_fade < float_1_0) && (k_f_0 < g_sound_global_fade)) &&
     (iVar1 = 0, 0 < (int)g_sound_8bd7b4)) {
    do {
      voice_set_volume(iVar1,
                       *(float *)(g_voices[iVar1].voice + 0x40) * g_sound_global_fade);
      iVar1 = iVar1 + 1;
    } while (iVar1 < (int)g_sound_8bd7b4);
  }
  return;
}


