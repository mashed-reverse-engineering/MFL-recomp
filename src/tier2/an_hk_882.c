#include "mfl_tier2.h"
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*005ddacc*/ extern float k_f_360;
/* RwMatrix byte offsets of the up and at axis vectors (RwMatrixTag: right 0x00, up 0x10, at 0x20, pos 0x30) */
#define MATRIX_UP_X (0x10)
#define MATRIX_UP_Y (0x14)
#define MATRIX_UP_Z (0x18)
#define MATRIX_AT_X (0x20)
#define MATRIX_AT_Y (0x24)
#define MATRIX_AT_Z (0x28)
/* car_push_from_matrix */

/* "(car, a, b): velocity change along the car's matrix axes scaled by 360 (inferred)" */

/* Pushes a car: adds to its velocity at_amount along the car's at axis plus up_amount along its up axis.
   Each amount is multiplied by k_f_360 first (inferred: 360.0f scale). Parameters: car = car object (passed to
   car_current_matrix and car_add_velocity), at_amount / up_amount = push strength along the at / up axis (inferred).
   Returns nothing; the velocity change is applied by car_add_velocity. */
void __cdecl car_push_from_matrix(undefined4 car,float at_amount,float up_amount)

{
  double at_scale;
  double up_scale;
  double up_x_scaled;
  struct {
    char *matrix;
    float vel_x;
    float vel_y;
    float vel_z;
    undefined1 pad_c[4];
    float up_y_scaled;
    float up_z_scaled;
  } scratch;

  /* Fetch the car's matrix pointer (written into scratch.matrix), then scale both push amounts. */
  car_current_matrix(&scratch.matrix,car);
  at_scale = (double)at_amount * k_f_360;
  up_scale = (double)up_amount * k_f_360;
  /* Velocity delta = at axis * at_scale, then the up axis * up_scale added on top, component by component. */
  scratch.vel_x = (float)(*(float *)(scratch.matrix + MATRIX_AT_X) * at_scale);
  scratch.vel_y = (float)(*(float *)(scratch.matrix + MATRIX_AT_Y) * at_scale);
  scratch.vel_z = (float)(*(float *)(scratch.matrix + MATRIX_AT_Z) * at_scale);
  up_x_scaled = *(float *)(scratch.matrix + MATRIX_UP_X) * up_scale;
  scratch.up_y_scaled = (float)(*(float *)(scratch.matrix + MATRIX_UP_Y) * up_scale);
  scratch.up_z_scaled = (float)(*(float *)(scratch.matrix + MATRIX_UP_Z) * up_scale);
  scratch.vel_x = (float)(up_x_scaled + scratch.vel_x);
  scratch.vel_y = scratch.up_y_scaled + scratch.vel_y;
  scratch.vel_z = scratch.up_z_scaled + scratch.vel_z;
  car_add_velocity(&scratch.vel_x,car);
  return;
}


