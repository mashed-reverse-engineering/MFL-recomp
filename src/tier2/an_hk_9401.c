#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*00491ab0*/ extern int car_enter_rwp_mode();
/*00492b20*/ extern int car_apply_rwp_impulse();
/*0049d060*/ extern int fx_spawn_burst_default_scale();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4620*/ extern void FUN_004c4620(void *p0, void *p1, undefined4 p2);
/*005dd340*/ extern float k_f_0_5;
/*005deb34*/ extern float k_f_300;
extern double fabs(double);
/* course_highway_hit_spawn_fx */

/* "on |value|>0.5 spawns a burst at the object position and offsets by the car's matrix" */

void course_highway_hit_spawn_fx(char *obj)

{
  double rand_x;
  struct {
    char *car_matrix;
    float impulse_x;
    float impulse_y;
    float impulse_z;
    undefined4 burst_x;
    undefined4 burst_y;
    undefined4 burst_z;
  } frame;
  
  frame.car_matrix = 0;
  frame.burst_x = 0;
  frame.burst_y = 0;
  frame.burst_z = 0;
  frame.impulse_x = 0.0;
  frame.impulse_y = 0.0;
  frame.impulse_z = 0.0;
  FUN_004c4620(&frame.burst_x,obj,*(undefined4 *)(obj + 0x44));
  if (k_f_0_5 < fabs(*(float *)(obj + 0x38))) {
    fx_spawn_burst_default_scale(&frame.burst_x,10,0,0,0,0x40000000);
    car_current_matrix(&frame.car_matrix,*(undefined4 *)(obj + 0x3c));
    frame.impulse_x = *(float *)(frame.car_matrix + 0x20) * k_f_300;
    frame.impulse_y = *(float *)(frame.car_matrix + 0x24) * k_f_300;
    frame.impulse_z = *(float *)(frame.car_matrix + 0x28) * k_f_300;
    rand_x = (double)rp_random_float_range(0xc2480000,0x42480000);
    frame.impulse_x = (float)rand_x;
    car_enter_rwp_mode(*(undefined4 *)(obj + 0x3c));
    car_apply_rwp_impulse(*(undefined4 *)(obj + 0x3c),&frame.impulse_x,&frame.burst_x);
  }
  return;
}


