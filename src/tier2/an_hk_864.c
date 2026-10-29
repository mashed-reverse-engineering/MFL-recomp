#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*00409f50*/ extern char *player_route_get_record_ptr();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*00434bb0*/ extern int get_event_type();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*0047b750*/ extern int car_get_speed();
/*004b2b48*/ extern int _ftol();
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd328*/ extern float k_f_0_6;
/*005dd32c*/ extern float k_f_3;
/*005dd340*/ extern float k_f_0_5;
/*005dd374*/ extern float k_f_0_2;
/*005dda24*/ extern float k_f_0_75;
/*005e64a4*/ extern float k_f_0;
/*005de22c*/ extern float k_f_1500;
/*005de244*/ extern float k_f_500;
/*00603b90*/ extern undefined g_ai_find_603b90;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7f0*/ extern undefined4 g_tick_count;
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c9994*/ extern undefined4 g_car_8c9994;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9980*/ extern float g_ai_skill;

#define MAX_CARS (4) /* car slots scanned for a target (inferred) */
#define NO_CAR (-1) /* best_car value when no candidate was found */
#define INITIAL_SEARCH_GAP (2.0) /* largest gap a candidate may have (inferred) */
#define MATRIX_POS_X (0x30) /* position x inside the car matrix (RwMatrix layout, inferred) */
#define MATRIX_POS_Z (0x38) /* position z inside the car matrix (RwMatrix layout, inferred) */

/* ai_find_target_b: picks the car this AI bot should chase, or keeps the one it already has.
 * bot_index: AI bot slot, which is also the bot's own car index. It is declared float but
 *   holds an int, read as `*(int *)&bot_index`.
 * unused_1, unused_2: not read by the body (inferred).
 * target_pos: out, receives the target's matrix x (element 0) and z (element 1).
 * Returns 1 when the bot has a target (target_pos written), 0 otherwise.
 * (was: "inferred: second choice of target point")
 */

undefined4 __cdecl ai_find_target_b(undefined4 unused_1,undefined4 unused_2,undefined4 *target_pos,float bot_index)

{
  struct AiBotRecord *self_bot;
  int *cooldown_ptr;
  bool target_is_behind;
  int self_car;
  int best_car;
  int check_result;
  int car_index;
  float *route_dir;
  double rank_gap;
  struct {
    char *matrix_addr;
    float candidate_value;
    float search_gap;
    int candidate_mode;
    float self_rank_key;
    int ai_polygon_count;
    float route_error;
    float self_speed;
    int candidate_state;
    undefined1 car_mode_extra [4];
    float self_pos_x;
    float self_pos_z;
    RwV3d direction;
    RwV3d unit_dir;
    RwV3d self_forward_out;      /* car_get_forward writes 12 bytes: a 4-byte member here overwrote the return address (stack_lint class A) */
  } frame;

  self_car = *(int *)&bot_index;
  cooldown_ptr = (int *)(&g_ai_bots[*(int *)&bot_index].field_0x4c);
  /* search cooldown (inferred): counts down by g_tick_count and is clamped at 0 */
  *cooldown_ptr = *cooldown_ptr - g_tick_count;
  if (*cooldown_ptr < 0) {
    *(undefined4 *)&g_ai_bots[*(int *)&bot_index].field_0x4c = 0;
  }
  /* best_car holds the search mode here (inferred): table lookup by skill and g_car_8c9994,
     class 1 disables the search, class 2 uses 2, any other class uses the bot index.
     The scan below reuses best_car as the chosen car index. */
  if (g_ai_catchup_class == 0) {
    best_car = ((int)(g_ai_skill));
    best_car = *(int *)((char *)&g_ai_find_603b90 + (g_car_8c9994 + best_car * 5) * 4);
  }
  else if (g_ai_catchup_class == 1) {
    best_car = 0;
  }
  else {
    best_car = 2;
    if (g_ai_catchup_class != 2) {
      best_car = *(int *)&bot_index;
    }
  }
  if (game_mode == GAME_MODE_SURVIVAL) {
    best_car = 2;
  }
  get_event_type();
  /* no search when the mode is 0 or the cooldown is still running */
  if ((best_car == 0) || (*(int *)&g_ai_bots[*(int *)&bot_index].field_0x4c == 0)) {
    return 0;
  }
  /* own car: matrix position (x, z) and speed, plus its rank key */
  car_current_matrix(&frame.matrix_addr,*(int *)&bot_index);
  frame.self_pos_x = *(float *)(frame.matrix_addr + MATRIX_POS_X);
  frame.self_pos_z = *(float *)(frame.matrix_addr + MATRIX_POS_Z);
  car_get_speed(&frame.self_speed,*(int *)&bot_index);
  car_get_forward(&frame.self_forward_out,*(int *)&bot_index);
  frame.self_forward_out.y = 0;
  frame.ai_polygon_count = course_get_ai_polygon_count();
  rank_gap = (double)player_rank_key(*(int *)&bot_index);
  frame.self_rank_key = (float)rank_gap;
  best_car = NO_CAR;
  target_is_behind = false;
  frame.search_gap = INITIAL_SEARCH_GAP;
  /* scan the car slots: skip our own car, need an active car of slot type 1 (human player,
     per the dossier) with candidate_mode 0. rank_gap = candidate rank key minus ours.
     A gap that is positive, or below 5 - polygon count (wrap-around, inferred), takes the
     first branch (target ahead); otherwise it takes the second branch (target behind).
     The smallest gap below search_gap wins. The condition below also overwrites
     candidate_mode with 5 - ai_polygon_count when the first test fails. */
  car_index = 0;
  do {
    if ((((car_index != self_car) && (check_result = car_is_active(car_index), check_result != 0)) &&
        (check_result = race_setup_get_slot_type(car_index), check_result == 1)) &&
       (car_get_mode(car_index,&frame.candidate_mode,frame.car_mode_extra), frame.candidate_mode == 0)) {
      rank_gap = (double)player_rank_key(car_index);
      frame.candidate_value = (float)rank_gap;
      rank_gap = rank_gap - (double)frame.self_rank_key;
      if ((k_f_0 < rank_gap) ||
         (frame.candidate_mode = 5 - frame.ai_polygon_count, rank_gap < (double)frame.candidate_mode)) {
        if (rank_gap < (double)frame.search_gap) {
          frame.search_gap = (float)rank_gap;
          target_is_behind = false;
          best_car = car_index;
        }
      }
      else if (-rank_gap < (double)frame.search_gap) {
        frame.search_gap = (float)-rank_gap;
        target_is_behind = true;
        best_car = car_index;
      }
    }
    car_index = car_index + 1;
  } while (car_index < MAX_CARS);
  /* no target yet: adopt the best candidate if it passes the route and speed checks */
  if (g_ai_bots[*(int *)&bot_index].target_valid == 0) {
    if (best_car != NO_CAR) {
      car_current_matrix(&frame.matrix_addr,best_car);
      frame.direction.x = *(float *)(frame.matrix_addr + MATRIX_POS_X) - frame.self_pos_x;
      frame.direction.y = 0.0;
      frame.direction.z = *(float *)(frame.matrix_addr + MATRIX_POS_Z) - frame.self_pos_z;
      frame.search_gap = RwV3dLength(&frame.direction);
      if (target_is_behind) {
        RwV3dNormalize(&frame.unit_dir,&frame.direction);
        route_dir = (float *)player_route_get_record_ptr(best_car);
        frame.route_error = -((route_dir[2] * frame.unit_dir.z + frame.unit_dir.x * *route_dir + route_dir[1] * frame.unit_dir.y) *
                    frame.search_gap);
        car_get_speed(&frame.candidate_value,best_car);
        /* bot_index is reused as a float scratch here (|speed difference|); the index is not
           read again on this path (self_car keeps it) */
        bot_index = frame.candidate_value - frame.self_speed;
        if (bot_index < k_f_0) {
          bot_index = -bot_index;
        }
        car_get_int_from_float_field(&frame.candidate_state,best_car);
        if (((frame.candidate_state == 4) && (bot_index < k_f_1500)) &&
           ((k_f_500 < frame.candidate_value &&
            (((frame.route_error < k_f_0_6 && (frame.search_gap < k_f_3)) && (k_f_0_5 < frame.search_gap)))))) {
          self_bot = &g_ai_bots[self_car];
          self_bot->target_valid = 1;
          self_bot->target_car = best_car;
          goto LAB_004186d3;
        }
      }
    }
  }
  /* target already held: keep it while the scan still picks it and it stays on its route */
  else {
    if (best_car == g_ai_bots[*(int *)&bot_index].target_car) {
      if (!target_is_behind) {
        g_ai_bots[*(int *)&bot_index].target_valid = 0;
      }
      car_current_matrix(&frame.matrix_addr,best_car);
      frame.direction.x = *(float *)(frame.matrix_addr + MATRIX_POS_X) - frame.self_pos_x;
      frame.direction.y = 0.0;
      frame.direction.z = *(float *)(frame.matrix_addr + MATRIX_POS_Z) - frame.self_pos_z;
      frame.search_gap = RwV3dLength(&frame.direction);
      RwV3dNormalize(&frame.unit_dir,&frame.direction);
      route_dir = (float *)player_route_get_record_ptr(best_car);
      if (k_f_0_75 <
          -((route_dir[2] * frame.unit_dir.z + frame.unit_dir.x * *route_dir + route_dir[1] * frame.unit_dir.y) * frame.search_gap)) {
        g_ai_bots[*(int *)&bot_index].target_valid = 0;
      }
      car_get_forward(&frame.direction,g_ai_bots[*(int *)&bot_index].target_car);
      route_dir = (float *)player_route_get_record_ptr(g_ai_bots[*(int *)&bot_index].target_car);
      frame.candidate_value = route_dir[2] * frame.direction.z + frame.direction.x * *route_dir + route_dir[1] * frame.direction.y;
      car_get_forward(&frame.direction,*(int *)&bot_index);
      route_dir = (float *)player_route_get_record_ptr(*(int *)&bot_index);
      if ((frame.candidate_value < k_f_0_2) ||
         (route_dir[2] * frame.direction.z + frame.direction.x * *route_dir + route_dir[1] * frame.direction.y < k_f_0_2)) {
        g_ai_bots[*(int *)&bot_index].target_valid = 0;
        return 0;
      }
      best_car = g_ai_bots[*(int *)&bot_index].target_car;
LAB_004186d3:
      car_current_matrix(&frame.matrix_addr,best_car);
      *target_pos = *(undefined4 *)(frame.matrix_addr + MATRIX_POS_X);
      target_pos[1] = *(undefined4 *)(frame.matrix_addr + MATRIX_POS_Z);
      return 1;
    }
    g_ai_bots[*(int *)&bot_index].target_valid = 0;
  }
  return 0;
}


