#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00437750*/ extern double option_get_volume_0();
/*00437ee0*/ extern int message_is_controller_missing_shown();
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005cc2a0*/ extern void FUN_005cc2a0(undefined4 p0, undefined4 p1);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*00617948*/ extern float g_sound_master_fade;
/*006190a4*/ extern undefined4 g_sound_frame_6190a4;
/*00619138*/ extern undefined4 g_sound_frame_619138;
/*00619e84*/ extern undefined4 g_sound_frame_619e84;
/*00619fac*/ extern undefined4 g_sound_frame_619fac;
/*0061a168*/ extern undefined4 g_sound_music_voice;
/*006ae0bc*/ extern undefined4 g_sound_6ae0bc;
/*008bd7b4*/ extern undefined4 g_sound_8bd7b4;
/* sound_music_volume_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "music volume from option_get_volume_0 with the menu state (ui_fn_437ee0)" */

void sound_music_volume_update(void)

{
  int filter_handle;
  float music_gain;
  int skip_voice_b;
  int skip_voice_c;
  int skip_voice_d;
  int skip_voice_music;
  int skip_voice_a;
  int voice_i;
  float voice_volume;
  
  music_gain = g_sound_master_fade;
  music_gain = (float)(option_get_volume_0() * music_gain);
  skip_voice_music = g_sound_music_voice;
  skip_voice_d = g_sound_frame_619fac;
  skip_voice_c = g_sound_frame_619e84;
  skip_voice_b = g_sound_frame_619138;
  skip_voice_a = g_sound_frame_6190a4;
  voice_i = 0;
  if (0 < (int)g_sound_8bd7b4) {
    do {
      if ((((voice_i != skip_voice_music) && (voice_i != skip_voice_b)) && (voice_i != skip_voice_a)) &&
         ((voice_i != skip_voice_c && (voice_i != skip_voice_d)))) {
        voice_volume = g_voices[voice_i].volume * music_gain;
        if (voice_volume > float_1_0) {      /* NaN passes both clamps unchanged (original: fcomp / test ah,0x41 / jne) */
          voice_volume = 1.0;
        }
        else if (voice_volume < k_f_0) {
          voice_volume = 0.0;
        }
        *(float *)(g_voices[voice_i].voice + 0x40) = voice_volume;
        filter_handle = *(int *)(g_voices[voice_i].voice + 0x30);
        if (filter_handle != 0) {
          FUN_005b7300(filter_handle,5,1,voice_volume);
        }
      }
      voice_i = voice_i + 1;
    } while (voice_i < (int)g_sound_8bd7b4);
  }
  skip_voice_a = message_is_controller_missing_shown();
  if (skip_voice_a != 0) {
    FUN_005cc2a0(g_sound_6ae0bc,0);
    return;
  }
  FUN_005cc2a0(g_sound_6ae0bc,0x3f800000);
  return;
}


