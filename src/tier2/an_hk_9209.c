#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int player_or_speed_a, float car_b);
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

/* "(voice, car, ...): positions the voice at the car with speed dependent volume" */

/* (inferred names) player_or_speed_a: the parameter slot is reused, first the player index (selects the voice table row), then the float speed of car_a (car_get_speed writes it there); the volume is max(speed_a, speed_b) (or speed_override + 500) times a per player scale */
void __cdecl sound_play_impact_voice(int car_a,int player_or_speed_a,int car_b,float speed_override)

{
  int voice_index;
  float volume_scale;
  int player;
  float speed_b;
  double pitch_jitter;

  player = player_or_speed_a;
  if ((((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count <= 1)) &&
      (voice_index = *(int *)((char *)&g_sound_play_618d2c + player * 0x94) + car_a,
      (*(byte *)(g_voices[voice_index].voice + 0x34) & 2) == 0)) &&
     ((*(byte *)(g_voices[*(int *)((char *)&g_sound_play_618d2c + player * 0x94) + car_b].voice +
                0x34) & 2) == 0)) {
    voice_set_position_from_car(voice_index,car_a);
    car_get_speed((float *)&player_or_speed_a,car_a);
    car_get_speed(&speed_b,car_b);
    if (speed_override == k_f_0) {
      if (*(float *)&player_or_speed_a < speed_b) {
        *(float *)&player_or_speed_a = speed_b;
      }
    }
    else {
      *(float *)&player_or_speed_a = speed_override + k_f_500;
    }
    volume_scale = k_f_2_5en4;
    if (player == 1) {
      volume_scale = k_f_1_8181818en4;
    }
    g_sound_play_62925c = *(float *)&player_or_speed_a * volume_scale;
    if (float_1_0 < g_sound_play_62925c) {
      g_sound_play_62925c = 1.0;
    }
    FUN_005b6a40(g_voices[voice_index].voice,1);
    pitch_jitter = (double)random_range(0,0x4509d000);
    *(float *)(g_voices[voice_index].voice + 0x3c) = (float)(pitch_jitter + (double)k_f_22050);
    if (*(int *)(g_voices[voice_index].voice + 0x30) != 0) {
      pitch_jitter = (double)random_range(0,0x4509d000);
      FUN_005b7300(*(undefined4 *)(g_voices[voice_index].voice + 0x30),4,1,
                   (float)(pitch_jitter + (double)k_f_22050));
    }
    voice_set_volume(voice_index,g_sound_play_62925c);
  }
  return;
}
