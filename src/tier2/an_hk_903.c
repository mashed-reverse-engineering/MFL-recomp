#include "mfl_tier2.h"
typedef int CourseId;
/*00409f50*/ extern char *player_route_get_record_ptr();
/*0040fdc0*/ extern int race_get_phase();
/*004170a0*/ extern int __cdecl ai_find_target_a(undefined4 p0, void *p1, void *p2, undefined4 p3, undefined4 p4, void *p5, undefined4 p6);
/*0041ac70*/ extern int __cdecl ai_drive_counter_phase_check(undefined4 p0, void *p1, void *p2, undefined4 p3);
/*004175c0*/ extern int __cdecl ai_find_target_c(undefined4 p0, void *p1, void *p2, undefined4 p3);
/*00417780*/ extern int __cdecl ai_trigger_zone_target_for_car(undefined4 p0, void *p1, void *p2, undefined4 p3);
/*00417a50*/ extern int __cdecl ai_hazard_target_for_car(undefined4 p0, void *p1, void *p2, undefined4 p3);
/*00417b70*/ extern int __cdecl ai_find_leader_a(undefined4 p0);
/*00417c30*/ extern int __cdecl ai_cell_is_value2(void *p0, undefined4 p1);
/*00417d70*/ extern int __cdecl ai_bot_stuck_check(undefined4 p0, void *p1, void *p2, undefined4 p3, undefined4 p4);
/*004183d0*/ extern int __cdecl ai_find_target_b(undefined4 p0, void *p1, void *p2, undefined4 p3);
/*00418870*/ extern int __cdecl ai_bot_obstacle_ahead(undefined4 p0);
/*00418990*/ extern double ai_steer_towards_point();
/*00418be0*/ extern int __cdecl ai_target_check(void *p0, void *p1);
/*00418d60*/ extern int ai_route_target_for_car();
/*0042b110*/ extern int current_course_id();
/*0044c540*/ extern int camera_get_car_8c747c();
/*0044c920*/ extern float ai_route_closest_point_search(undefined4 p0, void *p1, undefined4 p2, void *p3, undefined4 p4);
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
/*005e64a4*/ extern float k_f_0;
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
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/* ai_drive_c */
#define COURSE_WARZONE (33)
#define ROUTE_SEARCH_RADIUS_BITS (0x41200000) /* float 10.0f, passed to ai_route_closest_point_search (inferred radius) */
#define BOT_STATE_STRIDE (0x14) /* bytes per bot in g_ai_data.bot_state (4 bots x 20 bytes) */
#define STEER_HOLD_MS (200) /* ms after the last steer on one side before the other side may steer; the opposite input fades meanwhile */
#define BLINK_DIVISOR (0x3c) /* (inferred) */
#define BLINK_BIT (0x20U) /* (inferred) */

/* Steering and throttle for AI bot car_index (variant c).
 * Picks a target point (route target, then ai_find_target_a/b/c, trigger zone, and a hazard
 * target on COURSE_WARZONE), steers towards it and writes the control bytes:
 *   controls[0] / controls[1]: PlayerInput.left / right steer strength, 0..255
 *   controls[4] / controls[5]: PlayerInput.accelerate / brake, 0..255
 * ai_ctx is the AI/route context. unused_arg is not read.
 * (was: "bot steering: target point from ai_find_target_a/b/c with speed and forward vector (variant c)") */

void __cdecl ai_drive_c(undefined4 ai_ctx,int car_index,undefined1 *controls,undefined4 unused_arg)

{
  struct AiBotRecord *bot;
  byte *unused_byte_ptr;
  float prev_car_b0c;
  float prev_steer;
  bool unused_flag;
  float unused_float;
  undefined1 unused_byte;
  int result;
  CourseId course;
  undefined4 arg_scratch;
  float *route_dir;
  __declspec(align(8)) double steer_result;
  double prev_opposite_steer;
  double steer_amount;
  double route_dot;
  double route_cross;
  float *bot_state;
  uint *target_ptr;
  struct {
    int target_kind;
    float steer_angle;
    float heading_error;
    float car_speed;
    float car_b0c;
    int race_phase;
    char *car_matrix_ptr;
    uint candidate_x;
    undefined4 candidate_z;
    uint target_x;
    undefined4 target_z;
    undefined4 car_pos_x;
    undefined4 car_pos_z;
    float car_fwd_x;
    float car_fwd_y;
    float car_fwd_z;
  } frame;
  
  /* Car state for this frame: race phase, b0c value, speed, matrix, position and forward vector. */
  frame.race_phase = race_get_phase();
  car_get_field_b0c(&frame.car_b0c,car_index);
  car_get_speed(&frame.car_speed,car_index);
  car_current_matrix(&frame.car_matrix_ptr,car_index);
  frame.car_pos_x = *(undefined4 *)(frame.car_matrix_ptr + 0x30);
  frame.car_pos_z = *(undefined4 *)(frame.car_matrix_ptr + 0x38);
  /* Heading error to the closest route point (inferred), folded into 0..180 degrees. */
  ai_route_closest_point_search(ai_ctx, &frame.car_pos_x, ROUTE_SEARCH_RADIUS_BITS, &frame.heading_error, 0x0);
  if (k_f_180 < frame.heading_error) {
    frame.heading_error = k_f_360 - frame.heading_error;
  }
  /* Target search: route target first, then ai_find_target_a, b and c (each returns 0 = none). */
  target_ptr = &frame.target_x;
  arg_scratch = ai_ctx;
  result = car_index;
  ai_route_target_for_car(ai_ctx, &frame.target_x, car_index);
  frame.target_kind = 0;
  result = ai_find_target_a(ai_ctx,&frame.target_x,&frame.candidate_x,car_index,arg_scratch,target_ptr,result);
  if ((result == 0) || (result = ai_target_check(&frame.car_pos_x,&frame.candidate_x), result == 0)) {
    result = ai_find_target_b(ai_ctx,&frame.target_x,&frame.candidate_x,car_index);
    if ((result == 0) ||
       ((result = ai_target_check(&frame.car_pos_x,&frame.candidate_x), result == 0 ||
        (result = ai_bot_obstacle_ahead(car_index), result != 0)))) {
      result = ai_find_target_c(ai_ctx,&frame.target_x,&frame.candidate_x,car_index);
      if (result == 1) {
        result = ai_target_check(&frame.car_pos_x,&frame.candidate_x);
        if (result == 0) goto LAB_00418f37;
LAB_00418f66:
        frame.target_x = frame.candidate_x;
        frame.target_z = frame.candidate_z;
        frame.target_kind = 3;
      }
      else {
        if (result == 2) {
          controls[4] = 0;
          controls[5] = 0xff;
          return;
        }
LAB_00418f37:
        result = ai_trigger_zone_target_for_car(ai_ctx,&frame.target_x,&frame.candidate_x,car_index);
        if (result == 1) {
          result = ai_target_check(&frame.car_pos_x,&frame.candidate_x);
          if (result != 0) goto LAB_00418f66;
        }
        else if ((result == 2) &&
                (result = ai_target_check(&frame.car_pos_x,&frame.candidate_x), result != 0)) {
          frame.target_x = frame.candidate_x;
          frame.target_z = frame.candidate_z;
          frame.target_kind = 7;
          goto LAB_00418fe2;
        }
        result = ai_cell_is_value2(&frame.car_pos_x,car_index);
        if (result == 1) {
          frame.target_kind = 9;
        }
      }
    }
    else {
      frame.target_x = frame.candidate_x;
      frame.target_z = frame.candidate_z;
      frame.target_kind = 2;
    }
  }
  else {
    frame.target_x = frame.candidate_x;
    frame.target_z = frame.candidate_z;
    frame.target_kind = 1;
  }
LAB_00418fe2:
  course = current_course_id();
  if ((course == COURSE_WARZONE) &&
     (result = ai_hazard_target_for_car(ai_ctx,&frame.target_x,&frame.candidate_x,car_index), result != 0)) {
    frame.target_x = frame.candidate_x;
    frame.target_z = frame.candidate_z;
    frame.target_kind = 10;
  }
  if ((frame.race_phase == 6) && (result = camera_get_car_8c747c(), result == 0)) {
    if (frame.target_kind == 0) {
      result = ai_drive_counter_phase_check(ai_ctx,&frame.target_x,&frame.candidate_x,car_index);
      if ((result != 0) && (result = ai_target_check(&frame.car_pos_x,&frame.candidate_x), result != 0)) {
        controls[5] = 0xff;
        *controls = 0;
        controls[1] = 0;
        return;
      }
      result = ai_find_leader_a(car_index);
      if (result != 0) {
        frame.target_kind = 5;
      }
    }
    if (player_weapon_slots[car_index].descriptor == 0) {
      g_ai_bots[car_index].weapon_held_ms = 0;
      g_ai_bots[car_index].fire_delay = 0;
      g_ai_bots[car_index].must_fire = 0;
    }
    else {
      arg_scratch = (undefined4)(int)frame.heading_error;
      result = ai_bot_stuck_check(ai_ctx,&frame.target_x,&frame.candidate_x,car_index,arg_scratch);
      if ((result != 0) && (result = ai_target_check(&frame.car_pos_x,&frame.candidate_x), result != 0)) {
        frame.target_kind = 8;
      }
    }
  }
  bot = &g_ai_bots[car_index];
  bot->state_70 = frame.target_kind;
  steer_result = (double)ai_steer_towards_point(car_index,frame.target_x,frame.target_z);
  frame.steer_angle = (float)steer_result;
  prev_opposite_steer = (double)k_f_0;
  bot_state = (float *)(g_ai_data.bot_state + car_index * BOT_STATE_STRIDE);
  /* Steer angle below 180 degrees: left side, writes controls[0]. */
  if (frame.steer_angle < k_f_180) {
    prev_steer = bot_state[3];
    bot_state[2] = 360.0f;
    if (prev_steer < frame.steer_angle) {
      prev_opposite_steer = (double)prev_steer;
    }
    bot_state[3] = frame.steer_angle;
    if (float_1_0 < frame.steer_angle) {
      if ((bot->steer_side == 1) ||
         (result = g_race_clock_a - bot->field_0x30, 199 < result)) {
        steer_amount = (double)frame.steer_angle * (double)frame.car_speed * (double)k_f_0_003;
        if ((frame.target_kind == 0) && (k_f_20 < frame.heading_error)) {
          steer_amount = steer_amount * ((double)frame.heading_error * (double)k_f_0_05);
        }
        if (k_f_255 < steer_amount) {
          steer_amount = (double)k_f_255;
        }
        controls[0] = (undefined1)(int)steer_amount;
        bot->steer_amount = (int)steer_amount;
        bot->steer_side = 1;
        bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_x = STEER_HOLD_MS - result;
        {
          float fade = (float)(int)frame.candidate_x * k_f_0_005;      /* cl reassociated (steer_amount * k) * cand; the original rounds cand * k first (24-bit) */
          controls[1] = (undefined1)(int)(fade * (float)bot->steer_amount);
        }
      }
    }
  }
  /* Steer angle above 180 degrees: right side, writes controls[1]. */
  if (k_f_180 < frame.steer_angle) {
    prev_steer = bot_state[2];
    bot_state[3] = 0.0f;
    if (frame.steer_angle < prev_steer) {
      prev_opposite_steer = (double)k_f_360 - (double)prev_steer;
    }
    bot_state[2] = frame.steer_angle;
    if (frame.steer_angle < k_f_359) {
      if ((bot->steer_side == 2) ||
         (result = g_race_clock_a - bot->field_0x30, 199 < result)) {
        steer_amount = ((double)k_f_360 - (double)frame.steer_angle) * (double)frame.car_speed * (double)k_f_0_003;
        if ((frame.target_kind == 0) && (k_f_20 < frame.heading_error)) {
          steer_amount = steer_amount * ((double)frame.heading_error * (double)k_f_0_05);
        }
        if (k_f_255 < steer_amount) {
          steer_amount = (double)k_f_255;
        }
        controls[1] = (undefined1)(int)steer_amount;
        bot->steer_amount = (int)steer_amount;
        bot->steer_side = 2;
        bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_x = STEER_HOLD_MS - result;
        {
          float fade = (float)(int)frame.candidate_x * k_f_0_005;      /* cl reassociated (steer_amount * k) * cand; the original rounds cand * k first (24-bit) */
          controls[0] = (undefined1)(int)(fade * (float)bot->steer_amount);
        }
      }
    }
  }
  controls[4] = 0xff;
  prev_car_b0c = bot_state[4];
  bot_state[4] = frame.car_b0c;
  if ((k_f_15 < frame.car_b0c - prev_car_b0c) && (float_10_0 < frame.car_b0c)) {
    controls[4] = 0;
    controls[5] = 0xff;
  }
  if ((k_f_20 < prev_opposite_steer) && (k_f_2000 < frame.car_speed)) {
    controls[4] = 0;
    controls[5] = 0xff;
  }
  if ((steer_result < (double)k_f_180) && ((double)k_f_30 < steer_result)) {
    controls[4] = 0xff;
    controls[5] = 0xff;
    controls[0] = 0xff;
  }
  if (((double)k_f_180 < steer_result) && (steer_result < (double)k_f_330)) {
    controls[4] = 0xff;
    controls[5] = 0xff;
    controls[1] = 0xff;
  }
  if (frame.target_kind == 7) {
    controls[4] = 0x40;
    goto LAB_00419560;
  }
  /* Target kind 5 (leader, inferred): leader_distance > 4 cuts the throttle (controls[4] = 0), > 6 sets controls[5] (brake) to 0x40; on alternate BLINK_DIVISOR race-clock periods the left steer (controls[0]) is forced to 0xff. */
  if (frame.target_kind == 5) {
    if (k_f_4 < bot->leader_distance) {
      controls[4] = 0;
    }
    if (k_f_6 < bot->leader_distance) {
      controls[5] = 0x40;
    }
    if ((g_race_clock_b / BLINK_DIVISOR & BLINK_BIT) != 0) {
      *controls = 0xff;
      goto LAB_00419560;
    }
  }
  else {
    if (frame.target_kind == 9) {
      if (k_f_1750 < frame.car_speed) {
        controls[4] = 0;
        controls[5] = 0xff;
      }
      if (k_f_1250 < frame.car_speed) {
        controls[4] = 0;
      }
      goto LAB_00419560;
    }
    /* Target kind 2: route follow. route_dot is the dot product of the route direction and the
     * car's forward vector (along the route); route_cross is the 2D cross product (side). */
    if (frame.target_kind != 2) goto LAB_00419560;
    route_dir = (float *)player_route_get_record_ptr(car_index);
    car_get_forward(&frame.car_fwd_x,car_index);
    route_dot = route_dir[1] * frame.car_fwd_y + frame.car_fwd_x * *route_dir + frame.car_fwd_z * route_dir[2];
    frame.heading_error = (float)route_dot;
    route_cross = frame.car_fwd_x * route_dir[2] - frame.car_fwd_z * *route_dir;
    if (route_dot < k_f_0) {
      route_dot = -route_dot;
    }
    if (route_cross < k_f_0) {
      if (route_dot < k_f_0_9) {
        controls[1] = 0;
      }
      if (route_dot < float_0_8) {
        *controls = 0xff;
      }
      goto LAB_00419560;
    }
    if (route_dot < k_f_0_9) {
      *controls = 0;
    }
    if (!(route_dot < float_0_8)) goto LAB_00419560;
  }
  controls[1] = 0xff;
  /* Clear controls[4] and controls[5] unless the race phase is 5, 6, 9, 10 or 11 (inferred). */
LAB_00419560:
  if ((((frame.race_phase != 6) && (frame.race_phase != 5)) && (frame.race_phase != 9)) &&
     ((frame.race_phase != 10 && (frame.race_phase != 0xb)))) {
    controls[4] = 0;
    controls[5] = 0;
  }
  return;
}
