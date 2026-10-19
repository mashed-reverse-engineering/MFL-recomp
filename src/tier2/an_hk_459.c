#include "mfl_tier2.h"
/*0047a5b0*/ extern int car_add_velocity();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *p0);
/*005ddafc*/ extern float k_f_0_05;
/*005ddc28*/ extern float k_f_0;
/* Byte offsets of the first float of two row vectors in the RwMatrix returned by car_current_matrix (inferred RwMatrix layout: right, up, at, pos) */
#define RWMATRIX_UP_OFFSET (0x10)
#define RWMATRIX_AT_OFFSET (0x20)
/* car_apply_speed_boost_along_forward */

/* "adds a velocity along the car's forward vector scaled by 0.05 * horizontal speed" (was: header; the offsets say up, see below) */
/*
 * Speed boost: adds a velocity to the car, proportional to its horizontal speed.
 * car: the car index, passed to car_get_velocity, car_current_matrix and car_add_velocity.
 * unused_arg: never read by this function.
 * Added vector = 0.05 * horizontal speed * matrix row at RWMATRIX_UP_OFFSET, plus k_f_0 * matrix row at RWMATRIX_AT_OFFSET.
 * Returns nothing.
 */
void __cdecl car_apply_speed_boost_along_forward(undefined4 car,char *unused_arg)

{
  float boost_scale;
  struct {
    int matrix;
    float horizontal_speed;
    RwV2d horizontal_vel;
    RwReal vel_x;
    float vel_y;
    RwReal vel_z;
    undefined1 pad_c[4];
    float boost_y;
    float boost_z;
  } scratch;

  /* read the current velocity; the horizontal part is x and z (y is vertical) */
  car_get_velocity(&scratch.vel_x,car);
  scratch.horizontal_vel.x = scratch.vel_x;
  scratch.horizontal_vel.y = scratch.vel_z;
  scratch.horizontal_speed = RwV2dLength(&scratch.horizontal_vel);
  /* scale the boost by the horizontal speed */
  car_current_matrix(&scratch.matrix,car);
  boost_scale = scratch.horizontal_speed * k_f_0_05;
  scratch.boost_y = *(float *)(scratch.matrix + RWMATRIX_UP_OFFSET + 4) * boost_scale;
  scratch.boost_z = *(float *)(scratch.matrix + RWMATRIX_UP_OFFSET + 8) * boost_scale;
  /* add the boost along the up row and the k_f_0 term along the at row, then write the velocity back */
  scratch.vel_x = *(float *)(scratch.matrix + RWMATRIX_UP_OFFSET) * boost_scale + *(float *)(scratch.matrix + RWMATRIX_AT_OFFSET) * k_f_0;
  scratch.vel_y = scratch.boost_y + *(float *)(scratch.matrix + RWMATRIX_AT_OFFSET + 4) * k_f_0;
  scratch.vel_z = scratch.boost_z + *(float *)(scratch.matrix + RWMATRIX_AT_OFFSET + 8) * k_f_0;
  car_add_velocity(&scratch.vel_x,car);
  return;
}


