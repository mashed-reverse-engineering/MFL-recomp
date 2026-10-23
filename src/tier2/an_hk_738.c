#include "mfl_tier2.h"
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*008d8b14*/ extern char *g_race_setup;
#define RACE_SETUP_SLOT_FIRST (0xb8)
#define RACE_SETUP_SLOT_END (200)
#define RACE_SETUP_SLOT_STRIDE (4)
#define NO_CAR (-1)
#define MATRIX_POS_X_OFFSET (0x30)
#define MATRIX_POS_Y_OFFSET (0x34)
#define MATRIX_POS_Z_OFFSET (0x38)
/* race_get_focus_player */

/* Picks the focus pair for the camera / AI (inferred): among cars that are active and in mode 0
   (car_get_mode), find the pair (ref car, other car) whose positions are farthest apart.
   out_player = the other car of that pair, out_ref_car = the ref car. If no pair qualifies,
   both outputs are 0.
   (was: "(out player, out ?): active car with mode 0 selected by position, used for camera / AI focus
   (inferred)") */

void race_get_focus_player(int *out_player,int *out_ref_car)

{
  int other_car;
  int ref_car;
  int best_car;
  int ref_slot;
  int focus_ref_car; /* ref car of the best pair so far; also reused as the active flag of other_car in the inner loop (inferred) */
  int other_slot_offset;
  RwReal distance;
  struct {
    int best_ref_car;
    int car_mode;
    char *car_matrix;
    RwReal best_distance;
    int ref_slot_offset;
    undefined1 car_mode_extra [4];
    float ref_pos_x;
    float ref_pos_y;
    float ref_pos_z;
    RwV3d delta;
    undefined1 pad_c[8];
    float other_pos_z;
  } frame;

  focus_ref_car = NO_CAR;
  best_car = NO_CAR;
  frame.best_distance = 0.0;
  frame.best_ref_car = NO_CAR;
  ref_car = 0;
  frame.ref_slot_offset = RACE_SETUP_SLOT_FIRST;
  /* Outer loop: each race setup slot in turn is a candidate reference car (ref_car). */
  do {
    ref_slot = frame.ref_slot_offset;
    if (((*(int *)(frame.ref_slot_offset + g_race_setup) != 0) &&
        (other_car = car_is_active(ref_car), other_car == 1)) &&
       (car_get_mode(ref_car,&frame.car_mode,frame.car_mode_extra), frame.car_mode == 0)) {
      car_current_matrix(&frame.car_matrix,ref_car);
      frame.ref_pos_x = *(float *)(frame.car_matrix + MATRIX_POS_X_OFFSET);
      frame.ref_pos_y = *(float *)(frame.car_matrix + MATRIX_POS_Y_OFFSET);
      frame.ref_pos_z = *(float *)(frame.car_matrix + MATRIX_POS_Z_OFFSET);
      other_car = 0;
      other_slot_offset = RACE_SETUP_SLOT_FIRST;
      /* Inner loop: measure ref_car against every other qualifying car and keep the farthest. */
      do {
        if (((*(int *)(other_slot_offset + g_race_setup) != 0) &&
            (focus_ref_car = car_is_active(other_car), focus_ref_car == 1)) &&
           (car_get_mode(other_car,&frame.car_mode,frame.car_mode_extra), frame.car_mode == 0)) {
          car_current_matrix(&frame.car_matrix,other_car);
          frame.other_pos_z = *(float *)(frame.car_matrix + MATRIX_POS_Z_OFFSET);
          frame.delta.x = frame.ref_pos_x - *(float *)(frame.car_matrix + MATRIX_POS_X_OFFSET);
          frame.delta.y = frame.ref_pos_y - *(float *)(frame.car_matrix + MATRIX_POS_Y_OFFSET);
          frame.delta.z = frame.ref_pos_z - frame.other_pos_z;
          distance = RwV3dLength(&frame.delta);
          if (frame.best_distance <= distance) {
            best_car = other_car;
            frame.best_ref_car = ref_car;
            frame.best_distance = distance;
          }
        }
        other_slot_offset = other_slot_offset + RACE_SETUP_SLOT_STRIDE;
        other_car = other_car + 1;
        ref_slot = frame.ref_slot_offset;
        focus_ref_car = frame.best_ref_car;
      } while (other_slot_offset < RACE_SETUP_SLOT_END);
    }
    frame.ref_slot_offset = ref_slot + RACE_SETUP_SLOT_STRIDE;
    ref_car = ref_car + 1;
  } while (frame.ref_slot_offset < RACE_SETUP_SLOT_END);
  /* Fallbacks: a missing best car takes focus_ref_car, a missing focus_ref_car takes best_car;
     if still none, car 0. */
  if (best_car == NO_CAR) {
    best_car = focus_ref_car;
  }
  if (focus_ref_car == NO_CAR) {
    focus_ref_car = best_car;
  }
  if (best_car == NO_CAR) {
    best_car = 0;
  }
  if (focus_ref_car != NO_CAR) {
    *out_player = best_car;
    *out_ref_car = focus_ref_car;
    return;
  }
  *out_player = best_car;
  *out_ref_car = 0;
  return;
}
