#include "mfl_tier2.h"
typedef int CourseId;
/*00409f50*/ extern int player_route_get_record_ptr();
/*0040fdc0*/ extern int race_get_phase();
/*004170a0*/ extern int __cdecl ai_find_target_a(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6);
/*004175c0*/ extern int __cdecl ai_find_target_c(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00417780*/ extern int __cdecl ai_trigger_zone_target_for_car(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00417c30*/ extern int __cdecl ai_cell_is_value2(undefined4 p0, undefined4 p1);
/*00417d70*/ extern int __cdecl ai_bot_stuck_check(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004183d0*/ extern int __cdecl ai_find_target_b(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00418870*/ extern int __cdecl ai_bot_obstacle_ahead(undefined4 p0);
/*00418990*/ extern double ai_steer_towards_point();
/*00418be0*/ extern int __cdecl ai_target_check(undefined4 p0, undefined4 p1);
/*00418d60*/ extern int ai_route_target_for_car();
/*0044c540*/ extern int camera_get_car_8c747c();
/*0044c920*/ extern float ai_route_closest_point_search(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*0047b720*/ extern int car_get_field_b0c();
/*0047b750*/ extern int car_get_speed();
/*004b2b48*/ extern int _ftol();
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd36c*/ extern float k_f_0_4;
/*005dd9ac*/ extern float k_f_255;
/*005ddafc*/ extern float k_f_0_05;
/*005de258*/ extern float k_f_0_003;
/*005de25c*/ extern float k_f_0_005;
/*005dd620*/ extern float float_10_0;
/*005dd7f0*/ extern float k_f_30;
/*005dd814*/ extern float k_f_6;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddb0c*/ extern float k_f_15;
/*005ddb18*/ extern float float_0_8;
/*005ddb20*/ extern float k_f_0_9;
/*005ddc28*/ extern float k_f_0;
/*005ddea0*/ extern float k_f_20;
/*005de238*/ extern float k_f_2000;
/*005de248*/ extern float k_f_1250;
/*005de24c*/ extern float k_f_1750;
/*005de250*/ extern float k_f_330;
/*005de254*/ extern float k_f_359;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*007f0284*/ extern struct AiData g_ai_data;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/* ai_drive_b */
#define COURSE_WARZONE (33)
#define ROUTE_SEARCH_RANGE (0x41200000) /* float bits of 10.0f, passed as undefined4 (inferred: search radius) */
#define STEER_PULSE_MS (200) /* (inferred) steering pulse period, race clock ms */
#define BOT_STATE_STRIDE (0x14) /* bytes per bot entry in g_ai_data.bot_state */
#define INPUT_MAX (0xff) /* full value of an input byte */
#define INPUT_PARTIAL (0x40) /* partial value of an input byte (inferred) */
#define BLINK_DIVISOR (0x3c) /* divisor of g_race_clock_b for the blink counter (inferred) */
#define BLINK_BIT (0x20U) /* blink flag bit of the counter (inferred) */
#define MATRIX_POS_X_OFFSET (0x30) /* RwMatrix translation x */
#define MATRIX_POS_Z_OFFSET (0x38) /* RwMatrix translation z */
#define STEER_MODE_TARGET_A (1) /* set when ai_find_target_a gives a valid target */
#define STEER_MODE_TARGET_B (2) /* set when ai_find_target_b gives a valid target */
#define STEER_MODE_TARGET_C (3) /* set when ai_find_target_c, or the zone check with result 1, gives a valid target (inferred) */
#define STEER_MODE_ZONE_TARGET (7) /* set when ai_trigger_zone_target_for_car returns 2 and its check passes (inferred) */
#define STEER_MODE_STUCK (8) /* set when ai_bot_stuck_check fires */
#define STEER_MODE_CELL_VALUE2 (9) /* set when ai_cell_is_value2 returns 1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "bot steering: target point from ai_find_target_a/b/c with speed and forward vector (variant b)"
    */
/* Per-frame steering and drive inputs for the AI bot driving car `car_index` (inferred).
 * Picks the point to steer at (route target, a candidate from ai_find_target_a/b/c,
 * a trigger zone, or stuck recovery), turns it into a steering angle in degrees (0..360),
 * and writes the bot's input bytes into `inputs`: [0] and [1] are the two steering sides,
 * [4] and [5] the drive / fire bytes (inferred). `route_set` picks the route set;
 * `unused_arg` is not read. Returns nothing.
 */

void __cdecl ai_drive_b(undefined4 route_set,int car_index,undefined1 *inputs,undefined4 unused_arg)

{
  struct AiBotRecord *bot;
  byte *unused_byte_ptr;
  float prev_field_b0c;
  float prev_side_angle;
  bool unused_flag;
  float unused_float;
  undefined1 unused_byte;
  int result;
  CourseId unused_course;
  undefined4 call_arg;
  float *route_record;
  __declspec(align(8)) double steer_raw;
  double prev_turn_angle;
  double turn_strength;
  double route_dot;
  double route_cross;
  float *bot_state_ptr;
  uint *target_ptr;
  /* scratch values: callee outputs (targets, car state, route heading) and temporaries reused by later branches (inferred) */
  struct {
    float steer_angle;
    int steer_mode;
    float route_angle;
    float car_speed;
    float field_b0c;
    int car_matrix;
    uint candidate_x;
    undefined4 candidate_z;
    int race_phase;
    uint target_x;
    undefined4 target_z;
    undefined4 car_pos_x;
    undefined4 car_pos_z;
    float car_fwd_x;
    float car_fwd_y;
    float car_fwd_z;
  } frame;
  
  frame.race_phase = race_get_phase();
  car_get_field_b0c(&frame.field_b0c,car_index);
  car_get_speed(&frame.car_speed,car_index);
  car_current_matrix(&frame.car_matrix,car_index);
  frame.car_pos_x = *(undefined4 *)(frame.car_matrix + MATRIX_POS_X_OFFSET);
  frame.car_pos_z = *(undefined4 *)(frame.car_matrix + MATRIX_POS_Z_OFFSET);
  ai_route_closest_point_search(route_set, (undefined4)&frame.car_pos_x, ROUTE_SEARCH_RANGE, (undefined4)&frame.route_angle, 0x0);
  if (k_f_180 < frame.route_angle) {
    frame.route_angle = k_f_360 - frame.route_angle;
  }
  target_ptr = &frame.target_x;
  call_arg = route_set;
  result = car_index;
  ai_route_target_for_car(route_set, &frame.target_x, car_index);
  frame.steer_mode = 0;
  result = ai_find_target_a(route_set,(undefined4)&frame.target_x,(undefined4)&frame.candidate_x,car_index,call_arg,(undefined4)target_ptr,result);
  if ((result == 0) || (result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x), result == 0)) {
    result = ai_find_target_b(route_set,(undefined4)&frame.target_x,(undefined4)&frame.candidate_x,car_index);
    if ((result == 0) ||
       ((result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x), result == 0 ||
        (result = ai_bot_obstacle_ahead(car_index), result != 0)))) {
      result = ai_find_target_c(route_set,(undefined4)&frame.target_x,(undefined4)&frame.candidate_x,car_index);
      if (result == 1) {
        result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x);
        if (result == 0) goto LAB_00418f37;
LAB_00418f66:
        frame.target_x = frame.candidate_x;
        frame.target_z = frame.candidate_z;
        frame.steer_mode = STEER_MODE_TARGET_C;
      }
      else {
        if (result == 2) {
          inputs[4] = 0;
          inputs[5] = INPUT_MAX;
          return;
        }
LAB_00418f37:
        result = ai_trigger_zone_target_for_car(route_set,(undefined4)&frame.target_x,(undefined4)&frame.candidate_x,car_index);
        if (result == 1) {
          result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x);
          if (result != 0) goto LAB_00418f66;
        }
        else if ((result == 2) &&
                (result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x), result != 0)) {
          frame.target_x = frame.candidate_x;
          frame.target_z = frame.candidate_z;
          frame.steer_mode = STEER_MODE_ZONE_TARGET;
          goto LAB_00418fe2;
        }
        result = ai_cell_is_value2((undefined4)&frame.car_pos_x,car_index);
        if (result == 1) {
          frame.steer_mode = STEER_MODE_CELL_VALUE2;
        }
      }
    }
    else {
      frame.target_x = frame.candidate_x;
      frame.target_z = frame.candidate_z;
      frame.steer_mode = STEER_MODE_TARGET_B;
    }
  }
  else {
    frame.target_x = frame.candidate_x;
    frame.target_z = frame.candidate_z;
    frame.steer_mode = STEER_MODE_TARGET_A;
  }
LAB_00418fe2:
  /* race phase 6 while the camera is not on this car: with no weapon reset the weapon timers, else run the stuck check (inferred) */
  if ((frame.race_phase == 6) && (result = camera_get_car_8c747c(), result == 0)) {
    if (player_weapon_slots[car_index].descriptor == 0) {
      g_ai_bots[car_index].weapon_held_ms = 0;
      g_ai_bots[car_index].fire_delay = 0;
      g_ai_bots[car_index].must_fire = 0;
    }
    else {
      call_arg = (undefined4)(int)frame.route_angle;
      result = ai_bot_stuck_check(route_set,(undefined4)&frame.target_x,(undefined4)&frame.candidate_x,car_index,call_arg);
      if ((result != 0) && (result = ai_target_check((undefined4)&frame.car_pos_x,(undefined4)&frame.candidate_x), result != 0)) {
        frame.steer_mode = STEER_MODE_STUCK;
      }
    }
  }
  /* this bot's record, and its 20-byte block in g_ai_data.bot_state: [2] last steer angle above 180,
   [3] last steer angle below 180, [4] last field_b0c */
  bot = &g_ai_bots[car_index];
  steer_raw = (float10)ai_steer_towards_point(car_index,frame.target_x,frame.target_z);
  frame.steer_angle = (float)steer_raw;
  prev_turn_angle = (float10)k_f_0;
  bot_state_ptr = (float *)(g_ai_data.bot_state + car_index * BOT_STATE_STRIDE);
  /* steer below 180: pulse input [0], decay into input [1] between pulses (inferred) */
  if (frame.steer_angle < k_f_180) {
    prev_side_angle = bot_state_ptr[3];
    bot_state_ptr[2] = 360.0f;
    if (prev_side_angle < frame.steer_angle) {
      prev_turn_angle = (float10)prev_side_angle;
    }
    bot_state_ptr[3] = frame.steer_angle;
    /* pulse: once STEER_PULSE_MS has passed since bot->field_0x30 (last pulse time, inferred) re-arm,
       otherwise decay the last value (inferred) */
    if (float_1_0 < frame.steer_angle) {
      if ((*(int *)((char *)bot + 0x2c) == 1) ||
         (result = g_race_clock_a - bot->field_0x30, (STEER_PULSE_MS - 1) < result)) {
        turn_strength = (float10)frame.steer_angle * (float10)frame.car_speed * (float10)k_f_0_003;
        if ((frame.steer_mode == 0) && (k_f_20 < frame.route_angle)) {
          turn_strength = turn_strength * ((float10)frame.route_angle * (float10)k_f_0_05);
        }
        if (k_f_255 < turn_strength) {
          turn_strength = (float10)k_f_255;
        }
        inputs[0] = (undefined1)(int)turn_strength;
        *(int *)((char *)bot + 0x34) = (int)turn_strength;
        *(int *)((char *)bot + 0x2c) = 1;
        bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_x = STEER_PULSE_MS - result;
        inputs[1] = (undefined1)(int)((float10)frame.candidate_x * (float10)k_f_0_005 * (float10)*(int *)((char *)bot + 0x34));
      }
    }
  }
  /* steer above 180: pulse input [1], decay into input [0] between pulses (inferred) */
  if (k_f_180 < frame.steer_angle) {
    prev_side_angle = bot_state_ptr[2];
    bot_state_ptr[3] = 0.0f;
    if (frame.steer_angle < prev_side_angle) {
      prev_turn_angle = (float10)k_f_360 - (float10)prev_side_angle;
    }
    bot_state_ptr[2] = frame.steer_angle;
    if (frame.steer_angle < k_f_359) {
      if ((*(int *)((char *)bot + 0x2c) == 2) ||
         (result = g_race_clock_a - bot->field_0x30, (STEER_PULSE_MS - 1) < result)) {
        turn_strength = ((float10)k_f_360 - (float10)frame.steer_angle) * (float10)frame.car_speed * (float10)k_f_0_003;
        if ((frame.steer_mode == 0) && (k_f_20 < frame.route_angle)) {
          turn_strength = turn_strength * ((float10)frame.route_angle * (float10)k_f_0_05);
        }
        if (k_f_255 < turn_strength) {
          turn_strength = (float10)k_f_255;
        }
        inputs[1] = (undefined1)(int)turn_strength;
        *(int *)((char *)bot + 0x34) = (int)turn_strength;
        *(int *)((char *)bot + 0x2c) = 2;
        bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_x = STEER_PULSE_MS - result;
        inputs[0] = (undefined1)(int)((float10)frame.candidate_x * (float10)k_f_0_005 * (float10)*(int *)((char *)bot + 0x34));
      }
    }
  }
  inputs[4] = INPUT_MAX;
  prev_field_b0c = bot_state_ptr[4];
  bot_state_ptr[4] = frame.field_b0c;
  if ((k_f_15 < frame.field_b0c - prev_field_b0c) && (float_10_0 < frame.field_b0c)) {
    inputs[4] = 0;
    inputs[5] = INPUT_MAX;
  }
  if ((k_f_20 < prev_turn_angle) && (k_f_2000 < frame.car_speed)) {
    inputs[4] = 0;
    inputs[5] = INPUT_MAX;
  }
  if ((steer_raw < (float10)k_f_180) && ((float10)k_f_30 < steer_raw)) {
    inputs[4] = INPUT_MAX;
    inputs[5] = INPUT_MAX;
    inputs[0] = INPUT_MAX;
  }
  if (((float10)k_f_180 < steer_raw) && (steer_raw < (float10)k_f_330)) {
    inputs[4] = INPUT_MAX;
    inputs[5] = INPUT_MAX;
    inputs[1] = INPUT_MAX;
  }
  if (frame.steer_mode == STEER_MODE_ZONE_TARGET) {
    inputs[4] = INPUT_PARTIAL;
    goto LAB_00419560;
  }
  /* mode 5 is never assigned in this function (inferred: leftover from another path) */
  if (frame.steer_mode == 5) {
    if (k_f_4 < bot->leader_distance) {
      inputs[4] = 0;
    }
    if (k_f_6 < bot->leader_distance) {
      inputs[5] = INPUT_PARTIAL;
    }
    if ((g_race_clock_b / BLINK_DIVISOR & BLINK_BIT) != 0) {
      *inputs = INPUT_MAX;
      goto LAB_00419560;
    }
  }
  else {
    if (frame.steer_mode == STEER_MODE_CELL_VALUE2) {
      if (k_f_1750 < frame.car_speed) {
        inputs[4] = 0;
        inputs[5] = INPUT_MAX;
      }
      if (k_f_1250 < frame.car_speed) {
        inputs[4] = 0;
      }
      goto LAB_00419560;
    }
    /* only target B continues: compare the route record direction with the car forward vector (inferred) */
    if (frame.steer_mode != STEER_MODE_TARGET_B) goto LAB_00419560;
    route_record = (float *)player_route_get_record_ptr(car_index);
    car_get_forward(&frame.car_fwd_x,car_index);
    route_dot = route_record[1] * frame.car_fwd_y + frame.car_fwd_x * *route_record + frame.car_fwd_z * route_record[2];
    frame.route_angle = (float)route_dot;
    route_cross = frame.car_fwd_x * route_record[2] - frame.car_fwd_z * *route_record;
    if (route_dot < k_f_0) {
      route_dot = -route_dot;
    }
    if (route_cross < k_f_0) {
      if (route_dot < k_f_0_9) {
        inputs[1] = 0;
      }
      if (route_dot < float_0_8) {
        *inputs = INPUT_MAX;
      }
      goto LAB_00419560;
    }
    if (route_dot < k_f_0_9) {
      *inputs = 0;
    }
    if (!(route_dot < float_0_8)) goto LAB_00419560;
  }
  inputs[1] = INPUT_MAX;
LAB_00419560:
  /* catch-up class 1: scale input [4] by a factor (inferred) */
  if (g_ai_catchup_class == 1) {
    frame.candidate_x = (uint)(byte)inputs[4];
    inputs[4] = (undefined1)(int)((float10)frame.candidate_x * (float10)k_f_0_4);
  }
  /* race phases 5, 6, 9, 10 and 11 keep the drive inputs; other phases clear [4] and [5] (phase meanings inferred) */
  if ((((frame.race_phase != 6) && (frame.race_phase != 5)) && (frame.race_phase != 9)) &&
     ((frame.race_phase != 10 && (frame.race_phase != 0xb)))) {
    inputs[4] = 0;
    inputs[5] = 0;
  }
  return;
}
