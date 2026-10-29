#include "mfl_tier2.h"
extern struct VoiceSlot g_voices[210];
/*0040fdc0*/ extern int race_get_phase();
/*00422050*/ extern int vehicle_get_object_field_80();
/*00423f80*/ extern double car_damage_sum_front_back();
/*00469ac0*/ extern int voice_set_position_from_car();
/*0046a0c0*/ extern int voice_set_volume(int param_2, float param_3);
/*0047a830*/ extern int car_is_active();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b750*/ extern int car_get_speed();
/*00481fd0*/ extern double random_range();
/*005b6a40*/ extern int sound_voice_set_enabled(undefined4 p0, undefined4 p1);
/*005b7300*/ extern int sound_voice_set_float_param(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb58*/ extern float k_f_1000;
/*005e64a4*/ extern float k_f_0;
/*005de398*/ extern float k_f_0_35;
/*005df458*/ extern float k_f_200;
/*005df4d0*/ extern float k_f_3_5;
/*005dfcc0*/ extern float k_f_1_1111111en4;
/*006187f8*/ extern undefined4 g_sfx_table;
/*00618808*/ extern undefined4 g_sound_car_618808;
/*006291a0*/ extern undefined4 g_sound_car_6291a0;
/*006291b0*/ extern float g_sound_car_6291b0;
/*006291c4*/ extern float g_sound_car_6291c4;
/*006291d8*/ extern undefined g_sound_car_6291d8;
/*006291ec*/ extern undefined g_sound_car_6291ec;
/*00629200*/ extern undefined g_sound_car_629200;
/*00629214*/ extern undefined g_sound_car_629214;
/*00629228*/ extern undefined g_sound_car_629228;
/*0062923c*/ extern undefined4 g_sound_car_62923c;
/*00629240*/ extern float g_sound_car_629240;
/*00629244*/ extern float g_sound_car_629244;
/*00629248*/ extern undefined g_sound_car_629248;
/*006ae0d4*/ extern undefined g_sound_car_6ae0d4;
/*006ae15c*/ extern undefined g_sound_car_6ae15c;
/*006ae17c*/ extern undefined g_sound_car_6ae17c;
/*006ae18c*/ extern undefined4 g_sound_car_6ae18c;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008d9960*/ extern undefined4 g_sound_8d9960;
extern double sin(double);

/* engine sound tuning (values from the original code; names are inferred from use) */
#define VOICE_SECOND_OFFSET (4) /* the car's second voice is voice_index + VOICE_SECOND_OFFSET (inferred) */
#define CAR_CLASS_TABLE_STRIDE (0x4a) /* entries per vehicle-class row in g_sfx_table / g_sound_car_618808 (inferred) */
#define ENGINE_FREQ_BASE_LOW (1500.0) /* base frequency for some vehicle classes (inferred) */
#define ENGINE_FREQ_BASE_HIGH (2000.0) /* base frequency for the other classes (inferred) */
#define ENGINE_FREQ_CAP (10000.0) /* upper clamp on engine_freq, set in state 4 (inferred) */
#define FREQ_JITTER_WIDE (0x437a0000) /* random_range max, float bits of 250.0f (inferred use) */
#define FREQ_JITTER_NARROW (0x42c80000) /* random_range max, float bits of 100.0f (inferred use) */
#define ENGINE_STATE_HOLD (300) /* state value with no case: falls to default (inferred) */
#define ENGINE_PHASE_LAST (3) /* last phase index before wrap to 0 (inferred) */
#define WAVE_LEVEL_FLOOR (90.0) /* minimum wave level in mode 0 (inferred) */
#define WAVE_GAIN (2.2860000133514404) /* sine amplitude scale (inferred) */
#define WAVE_BIAS (0.699999988079071) /* sine offset (inferred) */

/* sound_car_engine_update */

/* "(player): engine voices from speed and damage (car_damage_sum_front_back,
   voice_set_position_from_car)" */

/* Per-frame engine sound update for one car, car_index (inferred: player slot).
   Picks the sfx class row and base frequency from the vehicle class, runs the
   engine state machine that moves engine_freq (throttle, speed, phase), then
   pushes frequency and volume into the car's two engine voices. Returns nothing. */
void sound_car_engine_update(int car_index)

{
  float *engine_freq;
  float prev_freq;
  float phase_value;
  float voice_base_freq;
  int voice_index; /* first holds vehicle_get_object_field_80() (switched on /6), then the voice index */
  uint car_inactive;
  undefined4 fx_result;
  int scratch_int;
  double wave_level;
  struct {
    float smoothed_freq;
    undefined1 pad_14[4];
    float car_speed;
    int car_int_field;
    float car_index_f;
    float damage_sum;
  } frame;
  
  voice_index = vehicle_get_object_field_80(car_index);
  wave_level = (double)car_damage_sum_front_back(car_index);
  frame.damage_sum = (float)wave_level;
  car_inactive = car_is_active(car_index);
  car_inactive = car_inactive ^ 1;
  g_sound_car_62923c = 1;
  /* pick the sfx class row (g_sound_car_6ae18c) and the base frequency from the vehicle class */
  switch(voice_index / 6) {
  case 0:
  case 4:
  case 5:
  case 0xb:
    g_sound_car_6ae18c = 3;
    g_sound_car_629244 = ENGINE_FREQ_BASE_LOW;
    break;
  case 1:
  case 3:
  case 0xd:
    g_sound_car_6ae18c = 1;
    goto LAB_0046ff1d;
  case 2:
  case 6:
  case 8:
  case 9:
    g_sound_car_6ae18c = 2;
    g_sound_car_629244 = ENGINE_FREQ_BASE_LOW;
    break;
  default:
    g_sound_car_6ae18c = 0;
LAB_0046ff1d:
    g_sound_car_629244 = ENGINE_FREQ_BASE_HIGH;
  }
  /* random jitter on the base frequency (wave_level is reused as scratch here) */
  wave_level = (double)random_range(0,FREQ_JITTER_WIDE);
  prev_freq = ((float *)&g_sound_car_6ae0d4)[car_index];
  g_sound_car_629244 = (float)(wave_level + (double)g_sound_car_629244);
  frame.car_index_f = (float)car_index;
  voice_base_freq = (float)car_index * k_f_200 + *(float *)&(&g_sound_car_618808)[g_sound_car_6ae18c * CAR_CLASS_TABLE_STRIDE];
  voice_index = (&g_sfx_table)[g_sound_car_6ae18c * CAR_CLASS_TABLE_STRIDE] + car_index;
  engine_freq = (float *)((char *)&g_sound_car_6ae0d4 + car_index * 4);
  /* (re)start the primary voice unless flag 0x2 in its record (+0x34) is set (inferred), then the secondary */
  if (((*(byte *)(g_voices[voice_index].voice + 0x34) & 2) == 0) &&
     (sound_voice_set_enabled(g_voices[voice_index].voice,1), g_sound_car_62923c != 0)) {
    sound_voice_set_enabled(g_voices[voice_index + VOICE_SECOND_OFFSET].voice,1);
  }
  voice_set_position_from_car(voice_index,car_index);
  if (g_sound_car_62923c != 0) {
    voice_set_position_from_car(voice_index + VOICE_SECOND_OFFSET,car_index);
  }
  /* read the car's speed and its integer state field */
  car_get_speed(&frame.car_speed,car_index);
  car_get_int_from_float_field(&frame.car_int_field,car_index);
  /* per-car engine state machine: state in g_sound_car_6ae17c, phase index in g_sound_car_6ae15c (inferred names); engine_freq is the output */
  switch(((undefined4 *)&g_sound_car_6ae17c)[car_index]) {
  case 0:
    if ((*(float *)((char *)&g_sound_car_6291ec + ((int *)&g_sound_car_6ae15c)[car_index] * 4) <=
         *engine_freq) && (g_player_inputs[g_player_slots[car_index].controller].accelerate != 0)) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 1;
    }
    if (*engine_freq < *(float *)((char *)&g_sound_car_629228 + ((int *)&g_sound_car_6ae15c)[car_index] * 4))
    {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 2;
    }
    if (frame.car_speed < k_f_100) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 3;
      ((undefined4 *)&g_sound_car_6ae15c)[car_index] = 0;
    }
    if (frame.car_int_field != 4) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 4;
      ((undefined4 *)&g_sound_car_6ae15c)[car_index] = 0;
    }
    wave_level = (double)random_range(0,FREQ_JITTER_NARROW);
    phase_value = *(float *)((char *)&g_sound_car_629248 + ((int *)&g_sound_car_6ae15c)[car_index] * 4);
    *(undefined4 *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) = 0;
    *engine_freq = (float)((double)frame.car_speed * (double)phase_value + wave_level + (double)k_f_1000);
    break;
  case 1:
    /* spelled !(a > b): with `a <= b` the compiler emits the other NaN form */
    if (!(*engine_freq > *(float *)((char *)&g_sound_car_629200 + ((int *)&g_sound_car_6ae15c)[car_index] * 4))) {
      scratch_int = ((int *)&g_sound_car_6ae15c)[car_index] + 1;
      ((int *)&g_sound_car_6ae15c)[car_index] = scratch_int;
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
      if (ENGINE_PHASE_LAST < scratch_int) {
        ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
        ((undefined4 *)&g_sound_car_6ae15c)[car_index] = 0;
      }
    }
    phase_value = *(float *)((char *)&g_sound_car_629200 + ((int *)&g_sound_car_6ae15c)[car_index] * 4);
    *(undefined4 *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) = 1;
    *engine_freq = phase_value;
    break;
  case 2:
    if (*(float *)((char *)&g_sound_car_629214 + ((int *)&g_sound_car_6ae15c)[car_index] * 4) <= *engine_freq
       ) {
      ((int *)&g_sound_car_6ae15c)[car_index] = ((int *)&g_sound_car_6ae15c)[car_index] + -1
      ;
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
    }
    phase_value = *(float *)((char *)&g_sound_car_629214 + ((int *)&g_sound_car_6ae15c)[car_index] * 4);
    *(undefined4 *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) = 2;
    *engine_freq = phase_value;
    break;
  case 3:
    if (k_f_100 < frame.car_speed) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
    }
    scratch_int = race_get_phase();
    if (((g_player_inputs[g_player_slots[car_index].controller].accelerate == 0) || (car_inactive != 0))
       || (scratch_int == 7)) {
      *engine_freq = *engine_freq - g_sound_car_6291c4;
    }
    else {
      *engine_freq = g_sound_car_6291b0 + *engine_freq;
    }
    break;
  case 4:
    if (frame.car_int_field == 4) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
    }
    if (car_inactive != 0) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = ENGINE_STATE_HOLD;
    }
    g_sound_car_629240 = ENGINE_FREQ_CAP;
    *engine_freq = g_sound_car_6291b0 + g_sound_car_6291b0 + *engine_freq;
    break;
  default:
    *engine_freq = 0.0;
    *(undefined4 *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) = 0;
    ((undefined4 *)&g_sound_car_6ae15c)[car_index] = 0;
    if (car_inactive == 0) {
      ((undefined4 *)&g_sound_car_6ae17c)[car_index] = 0;
    }
  }
  /* clamp engine_freq to [g_sound_car_629244 (floor), g_sound_car_629240 (cap)] */
  if (g_sound_car_629240 < *engine_freq) {
    *engine_freq = g_sound_car_629240;
  }
  if (*engine_freq < g_sound_car_629244) {
    *engine_freq = g_sound_car_629244;
  }
  /* smoothed_freq: engine_freq nudged by a per-phase step, offset by prev_freq (inferred) */
  frame.smoothed_freq = *engine_freq;
  scratch_int = ((int *)&g_sound_car_6ae15c)[car_index] * 4;
  if (((int *)&g_sound_car_6ae17c)[car_index] == 2) {
    if (*(float *)((char *)&g_sound_car_6291d8 + scratch_int) < *engine_freq - prev_freq) {
      frame.smoothed_freq = *(float *)((char *)&g_sound_car_6291d8 + scratch_int);
      goto LAB_004702b5;
    }
  }
  else if (*(float *)((char *)((undefined *)&g_sound_car_6291b0) + scratch_int) < *engine_freq - prev_freq) {
    frame.smoothed_freq = frame.car_index_f * float_10_0 + *(float *)((char *)((undefined *)&g_sound_car_6291b0) + scratch_int);
LAB_004702b5:
    frame.smoothed_freq = frame.smoothed_freq + prev_freq;
  }
  if (*engine_freq - prev_freq < -*(float *)((char *)((undefined *)&g_sound_car_6291c4) + scratch_int)) {
    frame.smoothed_freq = prev_freq - *(float *)((char *)((undefined *)&g_sound_car_6291c4) + scratch_int);
  }
  /* wave_level: sine of (smoothed_freq - 1000) scaled by k_f_1_1111111en4, then WAVE_GAIN / WAVE_BIAS (inferred),
     plus 1% of damage_sum when damage is above k_f_0 */
  wave_level = (double)k_f_1000;
  *engine_freq = frame.smoothed_freq;
  wave_level = (double)sin(((double)frame.smoothed_freq - wave_level) * (double)k_f_1_1111111en4);
  wave_level = wave_level * (double)(WAVE_GAIN) - (double)(WAVE_BIAS);
  if (k_f_0 < frame.damage_sum) {
    wave_level = (double)frame.damage_sum * (double)k_f_0_01 + wave_level;
  }
  /* mode 0 (g_sound_8d9960 == 0): wave_level below WAVE_LEVEL_FLOOR is lifted to it, otherwise capped at 1.0.
     As written this looks like it always lands on one of those two values; check disassembly (inferred) */
  if (*(int *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) == 0) {
    if (wave_level < (double)(WAVE_LEVEL_FLOOR)) {
      wave_level = (double)(WAVE_LEVEL_FLOOR);
      goto LAB_00470356;
    }
    if (wave_level <= (double)float_1_0) goto LAB_00470356;
  }
  wave_level = (double)float_1_0;
LAB_00470356:
  if (g_sound_car_62923c != 0) {
    if (*(int *)((char *)((undefined *)&g_sound_8d9960) + car_index * 4) == 0) {
      voice_set_volume(voice_index + VOICE_SECOND_OFFSET,
                       frame.smoothed_freq * k_f_1_0en5 + *(float *)&(&g_sound_car_6291a0)[g_sound_car_6ae18c]);
    }
    else {
      voice_set_volume(voice_index + VOICE_SECOND_OFFSET,
                       (float)(wave_level * (double)k_f_0_35 +
                              (double)*(float *)&(&g_sound_car_6291a0)[g_sound_car_6ae18c]));
    }
  }
  /* voice record +0x3c is the playback frequency: engine_freq * 3.5 (k_f_3_5) + voice_base_freq;
     +0x30 is a second sub-object given the same frequency (inferred) */
  *(float *)(g_voices[voice_index].voice + 0x3c) = *engine_freq * k_f_3_5 + voice_base_freq;
  scratch_int = *(int *)(g_voices[voice_index].voice + 0x30);
  fx_result = 0;
  if (scratch_int != 0) {
    fx_result = sound_voice_set_float_param(scratch_int,4,1,*engine_freq * k_f_3_5 + voice_base_freq);
  }
  /* primary voice volume: engine_freq * 1e-5 (k_f_1_0en5) plus the class volume table entry */
  voice_set_volume(voice_index,
                   *engine_freq * k_f_1_0en5 + *(float *)&(&g_sound_car_6291a0)[g_sound_car_6ae18c]);
  if (g_sound_car_62923c != 0) {
    *(float *)(g_voices[voice_index + VOICE_SECOND_OFFSET].voice + 0x3c) = *engine_freq * k_f_3_5 + voice_base_freq;
    voice_index = *(int *)(g_voices[voice_index + VOICE_SECOND_OFFSET].voice + 0x30);
    if (voice_index != 0) {
      sound_voice_set_float_param(voice_index,4,1,*engine_freq * k_f_3_5 + voice_base_freq);
    }
  }
  return;
}


