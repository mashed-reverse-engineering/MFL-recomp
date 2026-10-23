#include "mfl_tier2.h"
typedef int CourseId;
/*00409f50*/ extern char *player_route_get_record_ptr();
/*0040fdc0*/ extern int race_get_phase();
/*004170a0*/ extern int __cdecl ai_find_target_a(undefined4 p0, void *p1, void *p2, undefined4 p3, undefined4 p4, void *p5, undefined4 p6);
/*004173e0*/ extern int __cdecl ai_catchup_bot_state_update(undefined4 p0, void *p1, void *p2, undefined4 p3);
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
/* ai_drive_a */
#define COURSE_WARZONE (33)
#define BOT_STATE_STRIDE (0x14) /* sizeof one bot_state record (80 bytes / 4 bots) */
#define BOT_TURN_SIDE_OFFSET (0x2c) /* last turn side, 1 = left, 2 = right (inferred) */
#define BOT_TURN_STRENGTH_OFFSET (0x34) /* last turn strength (inferred) */
#define STEER_MODE_TARGET_A (1) /* ai_find_target_a chose the target */
#define STEER_MODE_TARGET_B (2) /* ai_find_target_b chose the target */
#define STEER_MODE_TARGET_C (3) /* ai_find_target_c chose the target */
#define STEER_MODE_LEADER (5) /* ai_find_leader_a */
#define STEER_MODE_TRIGGER_ZONE (7) /* ai_trigger_zone_target_for_car */
#define STEER_MODE_STUCK (8) /* ai_bot_stuck_check */
#define STEER_MODE_CELL_VALUE2 (9) /* ai_cell_is_value2 */
#define STEER_MODE_HAZARD (10) /* ai_hazard_target_for_car (warzone) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* bot steering: target point from ai_find_target_a/b/c with speed and forward vector (variant a).
 * Per frame, for bot car_index: choose a target (route, find_target_a/b/c, trigger zone, hazard,
 * catch-up, stuck fallback), steer towards it, and write the turn/throttle bytes into controls
 * (inferred: controls[0] / [1] = turn left / right, [4] / [5] = throttle / brake).
 * ai_ctx is the AI state (inferred). param_4 is unused.
 * (was: "bot steering: target point from ai_find_target_a/b/c with speed and forward vector (variant a)")
 */

void __cdecl ai_drive_a(undefined4 ai_ctx,int car_index,undefined1 *controls,undefined4 param_4)

{
  struct AiBotRecord *bot;
  byte *pbVar1;
  float prev_car_b0c;
  float prev_value;
  bool bVar4;
  float fVar5;
  undefined1 uVar6;
  int result;
  CourseId current_course;
  undefined4 arg_scratch;
  float *route_record;
  __declspec(align(8)) double steer_angle;
  double prev_steer;
  double control_value;
  double route_dot;
  double route_cross;
  float *bot_state;
  uint *target_ptr;
  /* per-frame locals; candidate_a / candidate_b are also reused as scratch */
  struct {
    int target_mode;
    float steer_angle_f;
    float route_angle;
    float speed;
    float car_field_b0c;
    int race_phase;
    char *car_matrix;
    uint candidate_a;
    undefined4 candidate_b;
    uint target_a;
    undefined4 target_b;
    undefined4 car_pos_x;
    undefined4 car_pos_z;
    float fwd_x;
    float fwd_y;
    float fwd_z;
  } frame;

  frame.race_phase = race_get_phase();
  car_get_field_b0c(&frame.car_field_b0c,car_index);
  car_get_speed(&frame.speed,car_index);
  car_current_matrix(&frame.car_matrix,car_index);
  frame.car_pos_x = *(undefined4 *)(frame.car_matrix + 0x30);
  frame.car_pos_z = *(undefined4 *)(frame.car_matrix + 0x38);
  /* Angle to the route point; folded into 0..180 degrees. */
  ai_route_closest_point_search(ai_ctx, &frame.car_pos_x, 0x41200000, &frame.route_angle, 0x0);
  if (k_f_180 < frame.route_angle) {
    frame.route_angle = k_f_360 - frame.route_angle;
  }
  target_ptr = &frame.target_a;
  arg_scratch = ai_ctx;
  result = car_index;
  ai_route_target_for_car(ai_ctx, &frame.target_a, car_index);
  frame.target_mode = 0;
  /* Target selection: find_target_a, then b, then c, then the trigger zone; each sets target_mode. */
  result = ai_find_target_a(ai_ctx,&frame.target_a,&frame.candidate_a,car_index,arg_scratch,target_ptr,result);
  if ((result == 0) || (result = ai_target_check(&frame.car_pos_x,&frame.candidate_a), result == 0)) {
    result = ai_find_target_b(ai_ctx,&frame.target_a,&frame.candidate_a,car_index);
    if ((result == 0) ||
       ((result = ai_target_check(&frame.car_pos_x,&frame.candidate_a), result == 0 ||
        (result = ai_bot_obstacle_ahead(car_index), result != 0)))) {
      result = ai_find_target_c(ai_ctx,&frame.target_a,&frame.candidate_a,car_index);
      if (result == 1) {
        result = ai_target_check(&frame.car_pos_x,&frame.candidate_a);
        if (result == 0) goto LAB_00418f37;
LAB_00418f66:
        frame.target_a = frame.candidate_a;
        frame.target_b = frame.candidate_b;
        frame.target_mode = STEER_MODE_TARGET_C;
      }
      else {
        if (result == 2) {
          controls[4] = 0;
          controls[5] = 0xff;
          return;
        }
LAB_00418f37:
        result = ai_trigger_zone_target_for_car(ai_ctx,&frame.target_a,&frame.candidate_a,car_index);
        if (result == 1) {
          result = ai_target_check(&frame.car_pos_x,&frame.candidate_a);
          if (result != 0) goto LAB_00418f66;
        }
        else if ((result == 2) &&
                (result = ai_target_check(&frame.car_pos_x,&frame.candidate_a), result != 0)) {
          frame.target_a = frame.candidate_a;
          frame.target_b = frame.candidate_b;
          frame.target_mode = STEER_MODE_TRIGGER_ZONE;
          goto LAB_00418fe2;
        }
        result = ai_cell_is_value2(&frame.car_pos_x,car_index);
        if (result == 1) {
          frame.target_mode = STEER_MODE_CELL_VALUE2;
        }
      }
    }
    else {
      frame.target_a = frame.candidate_a;
      frame.target_b = frame.candidate_b;
      frame.target_mode = STEER_MODE_TARGET_B;
    }
  }
  else {
    frame.target_a = frame.candidate_a;
    frame.target_b = frame.candidate_b;
    frame.target_mode = STEER_MODE_TARGET_A;
  }
LAB_00418fe2:
  /* Warzone hazard override. */
  current_course = current_course_id();
  if ((current_course == COURSE_WARZONE) &&
     (result = ai_hazard_target_for_car(ai_ctx,&frame.target_a,&frame.candidate_a,car_index), result != 0)) {
    frame.target_a = frame.candidate_a;
    frame.target_b = frame.candidate_b;
    frame.target_mode = STEER_MODE_HAZARD;
  }
  /* Race phase 6 with no camera car (inferred): catch-up, leader and stuck fallbacks, weapon timers. */
  if ((frame.race_phase == 6) && (result = camera_get_car_8c747c(), result == 0)) {
    if (frame.target_mode == 0) {
      result = ai_catchup_bot_state_update(ai_ctx,&frame.target_a,&frame.candidate_a,car_index);
      if ((result != 0) && (result = ai_target_check(&frame.car_pos_x,&frame.candidate_a), result != 0)) {
        controls[5] = 0xff;
        *controls = 0;
        controls[1] = 0;
        return;
      }
      result = ai_find_leader_a(car_index);
      if (result != 0) {
        frame.target_mode = STEER_MODE_LEADER;
      }
    }
    if (player_weapon_slots[car_index].descriptor == 0) {
      g_ai_bots[car_index].weapon_held_ms = 0;
      g_ai_bots[car_index].fire_delay = 0;
      g_ai_bots[car_index].must_fire = 0;
    }
    else {
      arg_scratch = (undefined4)(int)frame.route_angle;
      result = ai_bot_stuck_check(ai_ctx,&frame.target_a,&frame.candidate_a,car_index,arg_scratch);
      if ((result != 0) && (result = ai_target_check(&frame.car_pos_x,&frame.candidate_a), result != 0)) {
        frame.target_mode = STEER_MODE_STUCK;
      }
    }
  }
  /* Record the chosen mode, then steer towards the target point. */
  bot = &g_ai_bots[car_index];
  bot->state_70 = frame.target_mode;
  steer_angle = (double)ai_steer_towards_point(car_index,frame.target_a,frame.target_b);
  frame.steer_angle_f = (float)steer_angle;
  prev_steer = (double)k_f_0;
  bot_state = (float *)(g_ai_data.bot_state + car_index * BOT_STATE_STRIDE);
  /* Left turn (angle below 180): strength = steer * speed * 0.003, capped at 255. If the last turn was
   * on the right, it waits for the 200 ms hold first; the else branch fades controls[1] meanwhile. */
  if (frame.steer_angle_f < k_f_180) {
    prev_value = bot_state[3];
    bot_state[2] = 360.0f;
    if (prev_value < frame.steer_angle_f) {
      prev_steer = (double)prev_value;
    }
    bot_state[3] = frame.steer_angle_f;
    if (float_1_0 < frame.steer_angle_f) {
      if ((*(int *)((char *)bot + BOT_TURN_SIDE_OFFSET) == 1) ||
         (result = g_race_clock_a - *(int *)&bot->field_0x30, 199 < result)) {
        control_value = (double)frame.steer_angle_f * (double)frame.speed * (double)k_f_0_003;
        if ((frame.target_mode == 0) && (k_f_20 < frame.route_angle)) {
          control_value = control_value * ((double)frame.route_angle * (double)k_f_0_05);
        }
        if (k_f_255 < control_value) {
          control_value = (double)k_f_255;
        }
        controls[0] = (undefined1)(int)control_value;
        *(int *)((char *)bot + BOT_TURN_STRENGTH_OFFSET) = (int)control_value;
        *(int *)((char *)bot + BOT_TURN_SIDE_OFFSET) = 1;
        *(int *)&bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_a = 200 - result;
        controls[1] = (undefined1)(int)((double)frame.candidate_a * (double)k_f_0_005 * (double)*(int *)((char *)bot + BOT_TURN_STRENGTH_OFFSET));
      }
    }
  }
  /* Right turn (angle above 180): mirror of the left turn. */
  if (k_f_180 < frame.steer_angle_f) {
    prev_value = bot_state[2];
    bot_state[3] = 0.0f;
    if (frame.steer_angle_f < prev_value) {
      prev_steer = (double)k_f_360 - (double)prev_value;
    }
    bot_state[2] = frame.steer_angle_f;
    if (frame.steer_angle_f < k_f_359) {
      if ((*(int *)((char *)bot + BOT_TURN_SIDE_OFFSET) == 2) ||
         (result = g_race_clock_a - *(int *)&bot->field_0x30, 199 < result)) {
        control_value = ((double)k_f_360 - (double)frame.steer_angle_f) * (double)frame.speed * (double)k_f_0_003;
        if ((frame.target_mode == 0) && (k_f_20 < frame.route_angle)) {
          control_value = control_value * ((double)frame.route_angle * (double)k_f_0_05);
        }
        if (k_f_255 < control_value) {
          control_value = (double)k_f_255;
        }
        controls[1] = (undefined1)(int)control_value;
        *(int *)((char *)bot + BOT_TURN_STRENGTH_OFFSET) = (int)control_value;
        *(int *)((char *)bot + BOT_TURN_SIDE_OFFSET) = 2;
        *(int *)&bot->field_0x30 = g_race_clock_a;
      }
      else {
        frame.candidate_a = 200 - result;
        controls[0] = (undefined1)(int)((double)frame.candidate_a * (double)k_f_0_005 * (double)*(int *)((char *)bot + BOT_TURN_STRENGTH_OFFSET));
      }
    }
  }
  /* Brake / throttle flags from the speed change and the turn size (inferred). */
  controls[4] = 0xff;
  prev_car_b0c = bot_state[4];
  bot_state[4] = frame.car_field_b0c;
  if ((k_f_15 < frame.car_field_b0c - prev_car_b0c) && (float_10_0 < frame.car_field_b0c)) {
    controls[4] = 0;
    controls[5] = 0xff;
  }
  if ((k_f_20 < prev_steer) && (k_f_2000 < frame.speed)) {
    controls[4] = 0;
    controls[5] = 0xff;
  }
  if ((steer_angle < (double)k_f_180) && ((double)k_f_30 < steer_angle)) {
    controls[4] = 0xff;
    controls[5] = 0xff;
    controls[0] = 0xff;
  }
  if (((double)k_f_180 < steer_angle) && (steer_angle < (double)k_f_330)) {
    controls[4] = 0xff;
    controls[5] = 0xff;
    controls[1] = 0xff;
  }
  /* Mode overrides: trigger zone, leader chase, cell-value2 (inferred). */
  if (frame.target_mode == STEER_MODE_TRIGGER_ZONE) {
    controls[4] = 0x40;
    goto LAB_00419560;
  }
  if (frame.target_mode == STEER_MODE_LEADER) {
    if (k_f_4 < bot->leader_distance) {
      controls[4] = 0;
    }
    if (k_f_6 < bot->leader_distance) {
      controls[5] = 0x40;
    }
    if ((g_race_clock_b / 0x3c & 0x20U) != 0) {
      *controls = 0xff;
      goto LAB_00419560;
    }
  }
  else {
    if (frame.target_mode == STEER_MODE_CELL_VALUE2) {
      if (k_f_1750 < frame.speed) {
        controls[4] = 0;
        controls[5] = 0xff;
      }
      if (k_f_1250 < frame.speed) {
        controls[4] = 0;
      }
      goto LAB_00419560;
    }
    if (frame.target_mode != STEER_MODE_TARGET_B) goto LAB_00419560;
    route_record = (float *)player_route_get_record_ptr(car_index);
    car_get_forward(&frame.fwd_x,car_index);
    route_dot = route_record[1] * frame.fwd_y + frame.fwd_x * *route_record + frame.fwd_z * route_record[2];
    frame.route_angle = (float)route_dot;
    route_cross = frame.fwd_x * route_record[2] - frame.fwd_z * *route_record;
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
LAB_00419560:
  /* Catch-up class 1 scales controls[4] by k_f_0_4. */
  if (g_ai_catchup_class == 1) {
    frame.candidate_a = (uint)(byte)controls[4];
    controls[4] = (undefined1)(int)((double)frame.candidate_a * (double)k_f_0_4);
  }
  /* Race phases other than 5, 6, 9, 10, 11 clear controls[4] and [5]. */
  if ((((frame.race_phase != 6) && (frame.race_phase != 5)) && (frame.race_phase != 9)) &&
     ((frame.race_phase != 10 && (frame.race_phase != 0xb)))) {
    controls[4] = 0;
    controls[5] = 0;
  }
  return;
}
