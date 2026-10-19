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
  int iVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  float local_10;
  
  fVar2 = g_sound_master_fade;
  fVar2 = (float)(option_get_volume_0() * fVar2);
  iVar6 = g_sound_music_voice;
  iVar5 = g_sound_frame_619fac;
  iVar4 = g_sound_frame_619e84;
  iVar3 = g_sound_frame_619138;
  iVar7 = g_sound_frame_6190a4;
  iVar8 = 0;
  if (0 < (int)g_sound_8bd7b4) {
    do {
      if ((((iVar8 != iVar6) && (iVar8 != iVar3)) && (iVar8 != iVar7)) &&
         ((iVar8 != iVar4 && (iVar8 != iVar5)))) {
        local_10 = g_voices[iVar8].volume * fVar2;
        if (local_10 <= float_1_0) {
          if (local_10 < k_f_0) {
            local_10 = 0.0;
          }
        }
        else {
          local_10 = 1.0;
        }
        *(float *)(g_voices[iVar8].voice + 0x40) = local_10;
        iVar1 = *(int *)(g_voices[iVar8].voice + 0x30);
        if (iVar1 != 0) {
          FUN_005b7300(iVar1,5,1,local_10);
        }
      }
      iVar8 = iVar8 + 1;
    } while (iVar8 < (int)g_sound_8bd7b4);
  }
  iVar7 = message_is_controller_missing_shown();
  if (iVar7 != 0) {
    FUN_005cc2a0(g_sound_6ae0bc,0);
    return;
  }
  FUN_005cc2a0(g_sound_6ae0bc,0x3f800000);
  return;
}


