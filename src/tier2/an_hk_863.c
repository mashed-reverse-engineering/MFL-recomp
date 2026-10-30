#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0044c920*/ extern float ai_route_closest_point_search(undefined4 p0, void *p1, undefined4 p2, void *p3, undefined4 p4);
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*0047b750*/ extern int car_get_speed();
/*004b3284*/ extern double crt_acos_body(double p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd330*/ extern float float_1_0;
/*005dd364*/ extern float k_f_n1;
/*005e64a4*/ extern float k_f_0;
/*005dd378*/ extern float k_f_0_1;
/*005dd7f0*/ extern float k_f_30;
/*005ddacc*/ extern float k_f_360;
/*005ddad0*/ extern float k_f_180;
/*005ddb14*/ extern float float_7_0;
/*005ddb58*/ extern float k_f_1000;
/*005ddea0*/ extern float k_f_20;
/*005de21c*/ extern float k_f_57_2958;
/*005de220*/ extern float k_f_2500;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];

#define ROUTE_SEARCH_LIMIT_BITS (0x40f00000) /* float 7.5 bit pattern passed raw as undefined4 (inferred: route search radius) */
#define MAX_CARS (4)
#define RANK_WINDOW_BASE (5) /* rank window is RANK_WINDOW_BASE - ai_polygon_count (inferred) */
#define NO_TARGET_CAR (-1)
#define RANK_GAP_SENTINEL (2.0)

/* ai_find_target_a */

/* "inferred: candidate target point for a bot (first choice in the ai_drive_* variants)" */
/* (inferred) Aim helper for a bot: finds the human car it should chase and the aim point next to it,
 * then reports whether the bot is already roughly facing that point.
 * Returns 1 when a target was found and the bot's forward vector is within 30 degrees (k_f_30) of the
 * direction to the aim point; otherwise returns 0.
 * Parameters:
 *   route_id      route handle for ai_route_closest_point_search (inferred)
 *   origin_xz     x, z reference point; the aim offset is measured from here
 *   out_target_xz receives the aim point (x, z). It is written before the final facing test,
 *                 so it can change even when the result is 0.
 *   bot_index     car index of the bot; also indexes g_ai_bots
 *   param_5..7    unused
 */

undefined4 __cdecl ai_find_target_a(undefined4 route_id,float *origin_xz,float *out_target_xz,int bot_index,undefined4 param_5,
                undefined4 unused_6,undefined4 unused_7)

{
  float offset_x;
  float offset_scale;
  float offset_z;
  int ai_polygon_count;
  int status;
  int target_car;
  int car_index;
  double scratch_d; /* rank difference (candidate minus bot), later the acos result */
  struct {
    float scratch_f; /* reused: candidate rank key, then target speed, then the facing cosine */
    char *matrix; /* RwMatrix from car_current_matrix. The struct overlays later members, so read pos right after the call (layout inferred) */
    float route_angle; /* angle to the route direction, degrees 0..180 (inferred) */
    float best_rank_gap; /* smallest rank gap so far, starts at RANK_GAP_SENTINEL */
    float own_speed; /* bot speed */
    int unused_30; /* not read by this function */
    float own_rank_key; /* player_rank_key of the bot */
    RwReal distance; /* bot-to-target distance; reused as a temp for the aim point z */
    int car_mode; /* car_get_mode output (0 = eligible); also holds RANK_WINDOW_BASE - ai_polygon_count */
    float own_x; /* bot position x, copied from the matrix */
    float own_z; /* bot position z, copied from the matrix */
    RwV3d to_target; /* unit vector from the bot to the target (y = 0) */
    RwV3d own_forward; /* bot forward vector, y flattened and normalized */
  } frame;

  /* own car: position, speed and forward vector */
  car_current_matrix(&frame.matrix,bot_index);
  frame.own_x = ((RwMatrix *)frame.matrix)->pos.x;
  frame.own_z = ((RwMatrix *)frame.matrix)->pos.z;
  /* angle between the route direction at the bot's position and its heading, folded to 0..180;
     give up when more than k_f_20 degrees off */
  ai_route_closest_point_search(route_id, &frame.own_x, ROUTE_SEARCH_LIMIT_BITS, &frame.route_angle, 0x0);
  if (k_f_180 < frame.route_angle) {
    frame.route_angle = k_f_360 - frame.route_angle;
  }
  if (k_f_20 < frame.route_angle) {
    return 0;
  }
  car_get_speed(&frame.own_speed, bot_index);
  car_get_forward(&frame.own_forward,bot_index);
  frame.own_forward.y = 0.0;
  RwV3dNormalize(&frame.own_forward,&frame.own_forward);
  ai_polygon_count = course_get_ai_polygon_count();
  scratch_d = (double)player_rank_key(bot_index);
  frame.own_rank_key = (float)scratch_d;
  target_car = NO_TARGET_CAR;
  frame.best_rank_gap = RANK_GAP_SENTINEL;
  /* pick the human car (slot type 1, mode 0) that is ahead of the bot in rank, or inside the rank window,
     with the smallest rank gap */
  car_index = 0;
  do {
    if ((((car_index != bot_index) && (status = car_is_active(car_index), status != 0)) &&
        (status = race_setup_get_slot_type(car_index), status == 1)) &&
       (car_get_mode(car_index,&frame.car_mode,&frame.to_target), frame.car_mode == 0)) {
      scratch_d = (double)player_rank_key(car_index);
      frame.scratch_f = (float)scratch_d;
      scratch_d = scratch_d - (double)frame.own_rank_key;
      if (((k_f_0 < scratch_d) ||
          (frame.car_mode = RANK_WINDOW_BASE - ai_polygon_count, scratch_d < (double)frame.car_mode)) &&
         (scratch_d < (double)frame.best_rank_gap)) {
        frame.best_rank_gap = (float)scratch_d;
        target_car = car_index;
      }
    }
    car_index = car_index + 1;
  } while (car_index < MAX_CARS);
  if (target_car != NO_TARGET_CAR) {
    /* target position and distance to it */
    car_current_matrix(&frame.matrix, target_car);
    frame.to_target.x = ((RwMatrix *)frame.matrix)->pos.x - frame.own_x;
    frame.to_target.y = 0.0;
    frame.to_target.z = ((RwMatrix *)frame.matrix)->pos.z - frame.own_z;
    frame.distance = RwV3dLength(&frame.to_target);
    RwV3dNormalize(&frame.to_target,&frame.to_target);
    car_get_speed(&frame.scratch_f,target_car);
    /* target must be fast (above k_f_2500), speed-matched (gap below k_f_1000) and close (below float_7_0) */
    if (((frame.own_speed - frame.scratch_f < k_f_1000) && (k_f_2500 < frame.own_speed)) &&
       ((k_f_2500 < frame.scratch_f && (frame.distance < float_7_0)))) {
      /* aim point = target position, with the offset from origin_xz scaled by base_value * 0.1
         (inferred) unless game_mode == GAME_MODE_SURVIVAL */
      car_current_matrix(&frame.matrix, target_car);
      *out_target_xz = ((RwMatrix *)frame.matrix)->pos.x;
      frame.distance = ((RwMatrix *)frame.matrix)->pos.z;
      out_target_xz[1] = frame.distance;
      offset_x = *out_target_xz - *origin_xz;
      offset_z = frame.distance - origin_xz[1];
      if (game_mode != GAME_MODE_SURVIVAL) {
        offset_scale = (float)g_ai_bots[bot_index].base_value * k_f_0_1;
        offset_x = offset_scale * offset_x;
        offset_z = offset_scale * offset_z;
      }
      *out_target_xz = offset_x + *origin_xz;
      out_target_xz[1] = offset_z + origin_xz[1];
      frame.to_target.x = *out_target_xz - frame.own_x;
      frame.to_target.z = *(float *)&out_target_xz[1] - frame.own_z;      /* reload: the original does fstp + fld here (the rounded float), the compiler reused the unrounded st(0) */
      RwV3dNormalize(&frame.to_target,&frame.to_target);
      /* facing test: cosine between own forward and the direction to the aim point, clamped to -1..1 */
      frame.scratch_f = frame.own_forward.z * frame.to_target.z + frame.to_target.y * frame.own_forward.y + frame.own_forward.x * frame.to_target.x;
      if (frame.scratch_f < k_f_n1) {
        frame.scratch_f = -1.0;
      }
      else {
        if (frame.scratch_f > float_1_0) {
          frame.scratch_f = 1.0;
        }
      }
      /* angle in degrees (crt_acos_body taken as acos, inferred; k_f_57_2958 = 180/pi) must be below k_f_30 */
      scratch_d = (double)crt_acos_body((double)frame.scratch_f);
      if (scratch_d * (double)k_f_57_2958 < (double)k_f_30) {
        return 1;
      }
    }
  }
  return 0;
}


