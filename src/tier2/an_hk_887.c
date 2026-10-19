#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff20*/ extern int race_setup_get_slot_type();
/* arg0 is a CAR INDEX (the callee passes it to car_get_forward); (a, b, c) are float pairs (x, z); out receives the projected point (x, z) */
/*00416e20*/ extern int __cdecl point_project_onto_line_2d(int car_index, float *a, float *b, float *c, float scale, float *out);
/*0042b0c0*/ extern int course_get_ai_polygon_count();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*0047b750*/ extern int car_get_speed();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd40c*/ extern float k_f_1_5;
/*005de22c*/ extern float k_f_1500;
/*005e64a4*/ extern float k_f_0;
#define NO_SLOT (-1)
/* RwMatrixTag.pos (x, z) */
#define MATRIX_POS_X_OFFSET (0x30)
#define MATRIX_POS_Z_OFFSET (0x38)
/* ai_find_target_c */

/* "inferred: third choice of target point" */

/*
 * Chooses a target for AI car car_index among the other race slots (inferred).
 * The candidate with the smallest rank_gap below 2.0 wins (see the loop). If there is one and the
 * speed gap to it is above k_f_1500 (or this car is below k_f_1500), this car's position is projected
 * onto the line from line_point_b to the target point, written to projected_out.
 * unused_arg is not read. line_point_b is a float pair (x, z) (inferred meaning: line end point).
 * Returns 1 if the projection succeeded (projected_out written), 2 if the target is closer than
 * k_f_1_5 and its slot type is not 1, otherwise 0.
 */
undefined4 __cdecl ai_find_target_c(char *unused_arg,undefined4 line_point_b,undefined4 projected_out,int car_index)

{
  float rank_gap;
  int slot_used;
  int slot_type;
  int slot_index;
  float10 rank_key;
  struct {
    int car_matrix;
    float target_speed;
    float best_rank_gap;
    float own_speed;
    float target_x;
    float target_z;
    RwReal target_distance;
    float own_x;
    float own_z;
    RwV3d offset_to_target;
    RwV3d own_forward;
  } frame;

  /* own position (x, z) from the car matrix, own speed; own_forward is not used later */
  car_current_matrix(&frame.car_matrix,car_index);
  frame.own_x = *(float *)(frame.car_matrix + MATRIX_POS_X_OFFSET);
  frame.own_z = *(float *)(frame.car_matrix + MATRIX_POS_Z_OFFSET);
  car_get_speed(&frame.own_speed,car_index);
  car_get_forward(&frame.own_forward,car_index);
  frame.own_forward.y = 0;
  /* target_x is a scratch slot holding the own rank key until the loop ends (name reused below) */
  rank_key = (float10)player_rank_key(car_index);
  frame.target_x = (float)rank_key;
  /* slot_type: best candidate slot, NO_SLOT if none; after the loop it holds that slot's race setup type */
  slot_type = NO_SLOT;
  frame.best_rank_gap = 2.0;
  /* loop over race slots 0..3, skipping this car and unused slots */
  slot_index = 0;
  do {
    if (slot_index != car_index) {
      slot_used = race_setup_slot_used(slot_index);
      if (slot_used != 0) {
        rank_key = (float10)player_rank_key(slot_index);
        /* target_speed is a scratch slot here: candidate rank key, overwritten by the speed below */
        frame.target_speed = (float)rank_key;
        rank_gap = (float)(rank_key - (float10)frame.target_x);
        /* candidate at or below own rank (inferred): skip it when rank_gap >= 5 - polygon count.
           target_distance is a scratch slot holding that threshold here (name reused below) */
        if (rank_key - (float10)frame.target_x <= k_f_0) {
          slot_used = course_get_ai_polygon_count();
          frame.target_distance = (RwReal)(5 - slot_used);
          if ((float)(int)frame.target_distance <= rank_gap) goto LAB_0041768b;
        }
        /* keep the closest candidate so far */
        if (rank_gap < frame.best_rank_gap) {
          slot_type = slot_index;
          frame.best_rank_gap = rank_gap;
        }
      }
    }
LAB_0041768b:
    slot_index = slot_index + 1;
    if (3 < slot_index) {
      if (slot_type != NO_SLOT) {
        /* target position (x, z), offset from this car, and distance to it */
        car_current_matrix(&frame.car_matrix,slot_type);
        frame.target_x = *(float *)(frame.car_matrix + MATRIX_POS_X_OFFSET);
        frame.target_z = *(float *)(frame.car_matrix + MATRIX_POS_Z_OFFSET);
        frame.offset_to_target.x = frame.target_x - frame.own_x;
        frame.offset_to_target.y = 0.0;
        frame.offset_to_target.z = frame.target_z - frame.own_z;
        frame.target_distance = RwV3dLength(&frame.offset_to_target);
        /* target speed; the speed gap decides whether to project this car onto the line */
        car_get_speed(&frame.target_speed,slot_type);
        if ((k_f_1500 < frame.own_speed - frame.target_speed) || (frame.own_speed < k_f_1500)) {
          /* slot_index is reused here to hold the projection result (name reused) */
          slot_index = point_project_onto_line_2d(car_index, &frame.own_x, (float *)line_point_b, &frame.target_x, 1.0f, (float *)projected_out);
          if (slot_index != 0) {
            return 1;
          }
        }
        /* close target whose slot type is not 1: report 2 */
        slot_type = race_setup_get_slot_type(slot_type);
        if ((slot_type != 1) && (frame.target_distance < k_f_1_5)) {
          return 2;
        }
      }
      return 0;
    }
  } while( true );
}


