#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase(void);
/*00469ac0*/ extern void voice_set_position_from_car(int slot, int car);
/*0046a0c0*/ extern void voice_set_volume(int slot, float volume);
/*0042b110*/ extern int current_course_id(void);
/*0047ac90*/ extern void car_get_int_from_float_field(int *out, int car);
/*0047b360*/ extern void car_wheel_get_field_88_a(int car, int wheel, unsigned int *colour);
/*0047b750*/ extern void car_get_speed(float *out, int car);
/*00481fd0*/ extern float random_range(float low, float high);
/*005b6a40*/ extern void sound_voice_set_enabled(unsigned int voice, int enable);
/*005b7300*/ extern void sound_voice_set_float_param(int sound_object, int param, int enable, float value);
/*005dd330*/ extern float float_1_0;
/*005e64a4*/ extern float k_f_0;
/*005dfcd0*/ extern float k_f_2_2222222en4;
/*005dd61c*/ extern float k_f_0_001;
/*005dfccc*/ extern float k_f_5000;
/*005df99c*/ extern float k_f_22050;
/*008bd7b0*/ extern char *g_sound_groups;
/*006ad080*/ extern int g_sound_mute_when_multiplayer;
/*008d98bc*/ extern int g_sound_player_count;
#define VOICE_FIELD(slot, type, off) (*(type *)(*(char **)&g_voices[slot].voice + (off)))
#define GROUP_BASE(off) (*(int *)(g_sound_groups + (off)))
#define COURSE_FOREST 34
/* surface colour classes of the roll sounds (the voices of a class are the voices of one sound group of g_sound_groups) */
#define ROLL_NONE 0
#define ROLL_A 1      /* group +0x2c0, speed scale 1/4500 */
#define ROLL_B 2      /* group +0x350, speed scale 1/4500 */
#define ROLL_C 3      /* group +0x3e0, speed scale 0.001 */
#define ROLL_D 4      /* forest only: groups +0x740 and +0x7d0, speed scale 0.001 */
/* sound_surface_roll_voices_update */
/* "per car sets the three roll voice volumes by the surface colour id under the wheels and speed scaling (course 0x22 special)" */
/* TIER2: unvalidated 2026-10-21 authority=diffemu */
void sound_surface_roll_voices_update(int car)
{
  int course;
  int mode;
  int wheel;
  int slot;
  int slot2;
  int roll;
  unsigned int colour;
  float speed;
  float volume;
  float pitch;

  if (g_sound_mute_when_multiplayer != 0 && g_sound_player_count > 1) {
    return;
  }
  voice_set_volume(GROUP_BASE(0x2c0) + car, 0.0f);
  voice_set_volume(GROUP_BASE(0x350) + car, 0.0f);
  voice_set_volume(GROUP_BASE(0x3e0) + car, 0.0f);
  course = current_course_id();
  if (course == COURSE_FOREST) {
    voice_set_volume(GROUP_BASE(0x740) + car, 0.0f);
    voice_set_volume(GROUP_BASE(0x7d0) + car, 0.0f);
  }
  car_get_int_from_float_field(&mode, car);
  if (mode != 4) {
    return;
  }
  for (wheel = 0; wheel < 2; wheel++) {
    car_wheel_get_field_88_a(car, wheel + 2, &colour);
    car_get_speed(&speed, car);
    switch (colour) {
    case 0xff804000: case 0xff646464: case 0xff804040: case 0xffc81e5a: case 0xff961e5a: case 0xff968080: case 0xffc88080:
      roll = ROLL_A;
      break;
    case 0xff80c800: case 0xff1e80b4: case 0xff14508c: case 0xff0080c8: case 0xff005ab4: case 0xff0064b4:
    case 0xffa08080: case 0xffb48080: case 0xffb4b4b4: case 0xffdc8080:
      roll = ROLL_B;
      break;
    case 0xff00c8ff: case 0xff1446aa: case 0xffff32ff:
      roll = ROLL_C;
      break;
    case 0xffff8040:
      roll = ROLL_D;
      break;
    default:
      roll = ROLL_NONE;
      break;
    }
    if (roll == ROLL_NONE) {
      return;
    }
    if (roll == ROLL_A) {
      volume = speed * k_f_2_2222222en4;
      slot = GROUP_BASE(0x2c0) + car;
    }
    else if (roll == ROLL_B) {
      volume = speed * k_f_2_2222222en4;
      slot = GROUP_BASE(0x350) + car;
    }
    else if (roll == ROLL_C) {
      slot = GROUP_BASE(0x3e0) + car;
      volume = speed * k_f_0_001;
    }
    else {
      slot = GROUP_BASE(0x740) + car;
      slot2 = GROUP_BASE(0x7d0) + car;
      volume = speed * k_f_0_001;
    }
    if (volume > float_1_0) {      /* NaN passes both clamps unchanged (original: fcomp, test ah,0x41, jne) */
      volume = 1.0f;
    }
    else if (volume < k_f_0) {
      volume = 0.0f;
    }
    voice_set_position_from_car(slot, car);
    if ((VOICE_FIELD(slot, unsigned char, 0x34) & 2) == 0) {
      sound_voice_set_enabled(*(unsigned int *)&g_voices[slot].voice, 1);
    }
    pitch = volume * k_f_5000;
    VOICE_FIELD(slot, float, 0x3c) = (float)((k_f_22050 - random_range(0.0f, 441.0f)) + pitch);
    if (VOICE_FIELD(slot, int, 0x30) != 0) {
      sound_voice_set_float_param(VOICE_FIELD(slot, int, 0x30), 4, 1, (float)((k_f_22050 - random_range(0.0f, 441.0f)) + pitch));
    }
    voice_set_volume(slot, race_get_phase() == RACE_PHASE_RACING ? volume : 0.0f);
    if (course == COURSE_FOREST && roll == ROLL_D) {
      voice_set_position_from_car(slot2, car);
      if ((VOICE_FIELD(slot2, unsigned char, 0x34) & 2) == 0) {
        sound_voice_set_enabled(*(unsigned int *)&g_voices[slot2].voice, 1);
      }
      VOICE_FIELD(slot2, float, 0x3c) = 22050.0f;
      if (VOICE_FIELD(slot2, int, 0x30) != 0) {
        sound_voice_set_float_param(VOICE_FIELD(slot2, int, 0x30), 4, 1, 22050.0f);
      }
      if (race_get_phase() != RACE_PHASE_RACING) {
        volume = 0.0f;
      }
      voice_set_volume(slot2, volume);
    }
  }
}
