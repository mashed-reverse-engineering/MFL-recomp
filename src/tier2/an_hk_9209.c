#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*0047b750*/ extern int car_get_speed();
/*00481fd0*/ extern double random_range();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd330*/ extern float float_1_0;
/*005ddc28*/ extern float k_f_0;
/*005de244*/ extern float k_f_500;
/*005df2f0*/ extern float k_f_2_5en4;
/*005df99c*/ extern float k_f_22050;
/*005dfcd4*/ extern float k_f_1_8181818en4;
/*00618d2c*/ extern undefined g_sound_play_618d2c;
/*0062925c*/ extern float g_sound_play_62925c;
/*006ad080*/ extern undefined4 g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/* sound_play_impact_voice */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(voice, car, ...): positions the voice at the car with speed dependent volume" */

void __cdecl sound_play_impact_voice(int param_1,int param_2,int param_3,float param_4)

{
  int iVar1;
  float fVar2;
  int player;
  float speed_b;
  float10 fVar5;

  player = param_2;
  if ((((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count <= 1)) &&
      (iVar1 = *(int *)((char *)&g_sound_play_618d2c + player * 0x94) + param_1,
      (*(byte *)(g_voices[iVar1].voice + 0x34) & 2) == 0)) &&
     ((*(byte *)(g_voices[*(int *)((char *)&g_sound_play_618d2c + player * 0x94) + param_3].voice +
                0x34) & 2) == 0)) {
    voice_set_position_from_car(iVar1,param_1);
    car_get_speed((float *)&param_2,param_1);
    car_get_speed(&speed_b,param_3);
    if (param_4 == k_f_0) {
      if (*(float *)&param_2 < speed_b) {
        *(float *)&param_2 = speed_b;
      }
    }
    else {
      *(float *)&param_2 = param_4 + k_f_500;
    }
    fVar2 = k_f_2_5en4;
    if (player == 1) {
      fVar2 = k_f_1_8181818en4;
    }
    g_sound_play_62925c = *(float *)&param_2 * fVar2;
    if (float_1_0 < g_sound_play_62925c) {
      g_sound_play_62925c = 1.0;
    }
    FUN_005b6a40(g_voices[iVar1].voice,1);
    fVar5 = (float10)random_range(0,0x4509d000);
    *(float *)(g_voices[iVar1].voice + 0x3c) = (float)(fVar5 + (float10)k_f_22050);
    if (*(int *)(g_voices[iVar1].voice + 0x30) != 0) {
      fVar5 = (float10)random_range(0,0x4509d000);
      FUN_005b7300(*(undefined4 *)(g_voices[iVar1].voice + 0x30),4,1,
                   (float)(fVar5 + (float10)k_f_22050));
    }
    voice_set_volume(iVar1,g_sound_play_62925c);
  }
  return;
}
