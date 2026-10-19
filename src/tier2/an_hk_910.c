#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*0046e5c0*/ extern int sound_surface_ambience_lookup();
/*0047a7b0*/ extern int car_get_field_bf8();
/*0047b360*/ extern int car_wheel_get_field_88_a();
/*00481fd0*/ extern double random_range(float p0, float p1);
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005ddc28*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*005df99c*/ extern float k_f_22050;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*006ae0c4*/ extern undefined4 g_sound_frame_6ae0c4;
/*006ae190*/ extern undefined g_sound_frame_6ae190;
/*008bd7b0*/ extern undefined4 g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;
/* sound_surface_skid_voice_update */

/* "per car looks up the surface ambience, fades the skid voice with random pitch when the car is
   sliding" */

void __cdecl sound_surface_skid_voice_update(int param_1,undefined4 param_2,char *param_3,char *param_4,char *param_5,
          char *param_6)

{
  undefined4 *puVar1;
  uint uVar2;
  float fVar3;
  int race_phase;
  float *pfVar4;
  int iVar5;
  float10 fVar6;
  struct {
    undefined4 local_c;
    int local_8;
    undefined1 local_4 [4];
  } frame;
  
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    car_wheel_get_field_88_a(param_1,2,&frame.local_c);
    sound_surface_ambience_lookup(param_2,frame.local_c,&frame.local_8,frame.local_4);
    iVar5 = *(int *)(frame.local_8 * 0x90 + 0x80 + g_sound_groups) + param_1;
    frame.local_8 = car_get_field_bf8(param_1);
    race_phase = *(int *)((char *)&g_sound_frame_6ae190 + param_1 * 4);
    puVar1 = (undefined4 *)((char *)&g_sound_frame_6ae190 + param_1 * 4);
    if (race_phase != 0) {
      if (race_phase == 1) {
        uVar2 = g_voices[iVar5].voice;
        pfVar4 = (float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + param_1 * 4);
        *pfVar4 = 1.0;
        if ((*(byte *)(uVar2 + 0x34) & 2) == 0) {
          FUN_005b6a40(uVar2,1);
        }
        fVar6 = (float10)random_range(0.0,441.0);
        *(float *)(g_voices[iVar5].voice + 0x3c) = (float)((float10)k_f_22050 - fVar6);
        if (*(int *)(g_voices[iVar5].voice + 0x30) != 0) {
          fVar6 = (float10)random_range(0.0,441.0);
          FUN_005b7300(*(undefined4 *)(g_voices[iVar5].voice + 0x30),4,1,
                       (float)((float10)k_f_22050 - fVar6));
        }
        if (frame.local_8 == 0) {
          *puVar1 = 2;
        }
      }
      else {
        if (race_phase != 2) {
          return;
        }
        pfVar4 = (float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + param_1 * 4);
        fVar3 = *(float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + param_1 * 4) - k_f_0_02;
        *pfVar4 = fVar3;
        if (fVar3 < k_f_0) {
          *pfVar4 = 0.0;
          *puVar1 = 0;
        }
      }
      voice_set_position_from_car(iVar5,param_1);
      race_phase = race_get_phase();
      if (race_phase != 6) {
        voice_set_volume(iVar5,0.0);
        return;
      }
      voice_set_volume(iVar5,*pfVar4);
      return;
    }
    if (frame.local_8 != 0) {
      *puVar1 = 1;
    }
  }
  return;
}





