#include "mfl_tier2.h"
/*006ad210*/ extern VoiceSlot g_voices[];
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
/*006ad8b0*/ extern int g_sound_car_6ad8b0;      /* surface sound group (out of sound_surface_ambience_lookup) */
/*006ad078*/ extern int g_sound_car_6ad078;      /* second surface sound group */
/*0040fdc0*/ extern int race_get_phase(void);
/*00469ac0*/ extern void voice_set_position_from_car(int slot, int car);
/*0046a0c0*/ extern void voice_set_volume(int index, float volume);
/*0046e5c0*/ extern int sound_surface_ambience_lookup(int course_param, int surface, int *group_a, int *group_b);
/*0047ac90*/ extern void car_get_int_from_float_field(int *out, int car);
/*0047b360*/ extern void car_wheel_get_field_88_a(int car, int wheel, int *out);
/*0047b720*/ extern void car_get_field_b0c(float *out, int car);
/*00481fd0*/ extern float random_range(float low, float high);
/*005b6a40*/ extern void sound_voice_set_enabled(uint voice, int enable);
/*005b7300*/ extern void sound_voice_set_float_param(int sound_object, int param, int enable, float value);
extern double sin(double x);
/*005deb2c*/ extern float k_f_1_57;      /* 1.57f (about pi/2) */
/*005dfcc8*/ extern float k_f_4_71;      /* 4.71f (about 3 pi/2): sin starts at -1 */

#define VOICE_FIELD(slot, type, off) (*(type *)(g_voices[slot].voice + (off)))
#define GROUP_FIRST_VOICE(group) (*(int *)(g_sound_groups + (group) * 0x90 + 0x80))
#define RACE_PHASE_RUNNING 6

/* Silences the four engine / tyre voice groups of `car`; when the car is in mode 4 the two surface voices of wheel 2 play instead: volume from the car speed (field b0c, capped at 100) on a ... */
/* Silences the four engine / tyre voice groups of `car`; when the car is in mode 4 the two surface voices of
   wheel 2 play instead: volume from the car speed (field b0c, capped at 100) on a sine ramp, pitch 22050 Hz
   minus a random 0..2205 (first voice) or minus volume * 2205 (second voice, only if the lookup found one). */
void sound_car_voices_mute(int car, int course_param)
{
  int mode;
  int surface;
  int found;
  float speed;
  float volume;
  float rate;
  int voice_a;
  int voice_b;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  voice_set_volume(*(int *)(g_sound_groups + 0x80) + car, 0.0f);
  voice_set_volume(*(int *)(g_sound_groups + 0x110) + car, 0.0f);
  voice_set_volume(*(int *)(g_sound_groups + 0x1a0) + car, 0.0f);
  voice_set_volume(*(int *)(g_sound_groups + 0x230) + car, 0.0f);
  car_get_int_from_float_field(&mode, car);
  if (mode != 4) {
    return;
  }
  car_wheel_get_field_88_a(car, 2, &surface);
  found = sound_surface_ambience_lookup(course_param, surface, &g_sound_car_6ad8b0, &g_sound_car_6ad078);
  voice_a = GROUP_FIRST_VOICE(g_sound_car_6ad8b0) + car;
  voice_b = GROUP_FIRST_VOICE(g_sound_car_6ad078) + car;
  car_get_field_b0c(&speed, car);
  if (speed > 100.0f) {
    speed = 100.0f;
  }
  volume = (float)(sin((speed * 0.01f) * k_f_1_57 + k_f_4_71) + 1.0);
  if (volume > 1.0f) {
    volume = 1.0f;
  } else if (volume < 0.0f) {
    volume = 0.0f;
  }
  voice_set_position_from_car(voice_a, car);
  voice_set_position_from_car(voice_b, car);
  if ((VOICE_FIELD(voice_a, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[voice_a].voice, 1);
  }
  if ((VOICE_FIELD(voice_b, uchar, 0x34) & 2) == 0) {
    sound_voice_set_enabled(g_voices[voice_b].voice, 1);
  }
  if (race_get_phase() != RACE_PHASE_RUNNING) {
    voice_set_volume(voice_a, 0.0f);
    voice_set_volume(voice_b, 0.0f);
    return;
  }
  /* the voice and its sound object each get their own random pitch */
  VOICE_FIELD(voice_a, float, 0x3c) = 22050.0f - random_range(0.0f, 2205.0f);
  if (VOICE_FIELD(voice_a, int, 0x30) != 0) {
    sound_voice_set_float_param(VOICE_FIELD(voice_a, int, 0x30), 4, 1, 22050.0f - random_range(0.0f, 2205.0f));
  }
  voice_set_volume(voice_a, volume);
  if (found != 0) {
    rate = 22050.0f - volume * 2205.0f;
    VOICE_FIELD(voice_b, float, 0x3c) = rate;
    if (VOICE_FIELD(voice_b, int, 0x30) != 0) {
      sound_voice_set_float_param(VOICE_FIELD(voice_b, int, 0x30), 4, 1, rate);
    }
    voice_set_volume(voice_b, volume);
  }
}
