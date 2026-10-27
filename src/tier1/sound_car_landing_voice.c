#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*0047ac30*/ extern int car_get_mode();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b750*/ extern int car_get_speed();
/*005b6a40*/ extern int FUN_005b6a40(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int FUN_005b7300(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd620*/ extern float float_10_0;
/*0061795c*/ extern int g_sound_frame_61795c[];
/*006199e4*/ extern undefined4 g_sound_frame_6199e4;
/*006ae120*/ extern int g_sound_frame_6ae120[];
/* voice record offsets, in bytes from the voice pointer in g_voices[].voice */
#define VOICE_OFF_FLAGS (0x34)
#define VOICE_OFF_HANDLE (0x30)
#define VOICE_OFF_FREQ (0x3c)
#define VOICE_FLAG_BUSY (2) /* set = voice busy, sfx not started (inferred) */
#define SFX_CAR_FREQ (0x46ac4400) /* float bit pattern 22050.0f */
#define SFX_CAR_FREQ_F (22050.0f)
/* sound_car_landing_voice */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "plays the landing voice when the car mode changes from air to ground at speed >10" */

void sound_car_landing_voice(int car)
{
  int slot;
  int *landing;
  int stream;
  int mode;
  int mode_ticks;
  float speed;

  car_get_mode(car, &mode, &mode_ticks);
  car_get_speed(&speed, car);
  if (mode != 2 || mode_ticks <= 0) {
    landing = &g_sound_frame_61795c[car];
    car_get_int_from_float_field(landing, car);
    if (*landing == 0) {
      g_sound_frame_6ae120[car] = 1;
    }
    if (g_sound_frame_6ae120[car] == 1 && *landing >= 2 && speed > float_10_0) {
      slot = g_sound_frame_6199e4 + car;
      if ((*(byte *)(g_voices[slot].voice + VOICE_OFF_FLAGS) & VOICE_FLAG_BUSY) == 0) {
        voice_set_position_from_car(slot, car);
        FUN_005b6a40(g_voices[slot].voice, 1);
        *(undefined4 *)(g_voices[slot].voice + VOICE_OFF_FREQ) = SFX_CAR_FREQ;
        stream = *(int *)(g_voices[slot].voice + VOICE_OFF_HANDLE);
        if (stream != 0) {
          FUN_005b7300(stream, 4, 1, SFX_CAR_FREQ_F);
        }
        voice_set_volume(slot, 1.0);
      }
      g_sound_frame_6ae120[car] = 0;
    }
    if (g_sound_frame_61795c[car] >= 2) {
      g_sound_frame_6ae120[car] = 0;
    }
  }
}
