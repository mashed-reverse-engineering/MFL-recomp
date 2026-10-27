#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int surface_arg, float unused_3);
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
/*008bd7b0*/ extern char *g_sound_groups;
/*008d98bc*/ extern int g_sound_player_count;
/* sound_surface_skid_voice_update */

/* "per car looks up the surface ambience, fades the skid voice with random pitch when the car is
   sliding" */

/* (inferred names) group_or_slide: first the sound group looked up for the wheel surface, then the car sliding flag (car_get_field_bf8); state_then_phase: first the
   per car skid voice state (0 off, 1 start, 2 fade out), then the race phase. */
void __cdecl sound_surface_skid_voice_update(int car_index,undefined4 surface_arg,char *unused_3,char *unused_4,char *unused_5,
          char *unused_6)

{
  undefined4 *state_ptr;
  uint voice;
  float new_fade;
  int state_then_phase;
  float *fade_ptr;
  int voice_index;
  double pitch_jitter;
  struct {
    undefined4 wheel_surface;
    int group_or_slide;
    undefined1 lookup_extra [4];
  } frame;
  
  if ((g_sound_mute_when_multiplayer == 0) || (g_sound_player_count < 2)) {
    car_wheel_get_field_88_a(car_index,2,&frame.wheel_surface);
    sound_surface_ambience_lookup(surface_arg,frame.wheel_surface,&frame.group_or_slide,frame.lookup_extra);
    voice_index = *(int *)(frame.group_or_slide * 0x90 + 0x80 + g_sound_groups) + car_index;
    frame.group_or_slide = car_get_field_bf8(car_index);
    state_then_phase = ((int *)&g_sound_frame_6ae190)[car_index];
    state_ptr = (undefined4 *)((char *)&g_sound_frame_6ae190 + car_index * 4);
    if (state_then_phase != 0) {
      if (state_then_phase == 1) {
        voice = g_voices[voice_index].voice;
        fade_ptr = (float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + car_index * 4);
        *fade_ptr = 1.0;
        if ((*(byte *)(voice + 0x34) & 2) == 0) {
          FUN_005b6a40(voice,1);
        }
        pitch_jitter = (double)random_range(0.0,441.0);
        *(float *)(g_voices[voice_index].voice + 0x3c) = (float)((double)k_f_22050 - pitch_jitter);
        if (*(int *)(g_voices[voice_index].voice + 0x30) != 0) {
          pitch_jitter = (double)random_range(0.0,441.0);
          FUN_005b7300(*(undefined4 *)(g_voices[voice_index].voice + 0x30),4,1,
                       (float)((double)k_f_22050 - pitch_jitter));
        }
        if (frame.group_or_slide == 0) {
          *state_ptr = 2;
        }
      }
      else {
        if (state_then_phase != 2) {
          return;
        }
        fade_ptr = (float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + car_index * 4);
        new_fade = *(float *)((char *)((undefined *)&g_sound_frame_6ae0c4) + car_index * 4) - k_f_0_02;
        *fade_ptr = new_fade;
        if (new_fade < k_f_0) {
          *fade_ptr = 0.0;
          *state_ptr = 0;
        }
      }
      voice_set_position_from_car(voice_index,car_index);
      state_then_phase = race_get_phase();
      if (state_then_phase != 6) {
        voice_set_volume(voice_index,0.0);
        return;
      }
      voice_set_volume(voice_index,*fade_ptr);
      return;
    }
    if (frame.group_or_slide != 0) {
      *state_ptr = 1;
    }
  }
  return;
}





