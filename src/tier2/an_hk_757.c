#include "mfl_tier2.h"
/*0040fbf0*/ extern int race_get_focus_player();
/*0047b520*/ extern int car_current_matrix();
/*005dd340*/ extern float k_f_0_5;
#define CAMERA_OVERRIDE_ENABLED_OFFSET (0x1c)
#define CAMERA_OVERRIDE_PLAYER_OFFSET (0x20)
#define CAR_MATRIX_POS_X_OFFSET (0x30)
#define CAR_MATRIX_POS_Y_OFFSET (0x34)
#define CAR_MATRIX_POS_Z_OFFSET (0x38)
/* camera_get_target_position */

/* Writes the camera target point to out_pos[0..2]: the midpoint (inferred) of the focus player's car position
   and the target player's car position. camera+0x1C is an override flag; when set, camera+0x20 (inferred)
   is the player id used for both ends, so the result is that car's position.
   (was: "position of the followed car (or the override at +0x1C)") */

void camera_get_target_position(float *out_pos,char *camera)

{
  volatile float half_z_delta;
  float target_y;
  float target_z;
  float focus_x;
  struct {
    char *car_matrix;
    undefined4 focus_player;
    undefined4 target_player;
    float focus_y;
    float focus_z;
  } frame;

  /* Who is followed (focus_player) and the second player (target_player, inferred). */
  race_get_focus_player(&frame.focus_player,&frame.target_player);
  /* Override active: both ends use the override player. */
  if (*(int *)(camera + CAMERA_OVERRIDE_ENABLED_OFFSET) != 0) {
    frame.focus_player = *(undefined4 *)(camera + CAMERA_OVERRIDE_PLAYER_OFFSET);
    frame.target_player = frame.focus_player;
  }
  /* Focus car position (x kept in a local, y and z in the frame). */
  car_current_matrix(&frame.car_matrix,frame.focus_player);
  focus_x = *(float *)(frame.car_matrix + CAR_MATRIX_POS_X_OFFSET);
  frame.focus_y = *(float *)(frame.car_matrix + CAR_MATRIX_POS_Y_OFFSET);
  frame.focus_z = *(float *)(frame.car_matrix + CAR_MATRIX_POS_Z_OFFSET);
  /* Target car position. */
  car_current_matrix(&frame.car_matrix,frame.target_player);
  target_y = *(float *)(frame.car_matrix + CAR_MATRIX_POS_Y_OFFSET);
  target_z = *(float *)(frame.car_matrix + CAR_MATRIX_POS_Z_OFFSET);
  /* Midpoint per axis: focus - (focus - target) * 0.5. */
  *out_pos = focus_x - (focus_x - *(float *)(frame.car_matrix + CAR_MATRIX_POS_X_OFFSET)) * k_f_0_5;
  out_pos[1] = frame.focus_y - (frame.focus_y - target_y) * k_f_0_5;
  half_z_delta = (frame.focus_z - target_z) * k_f_0_5;
  out_pos[2] = frame.focus_z - half_z_delta;
  return;
}


