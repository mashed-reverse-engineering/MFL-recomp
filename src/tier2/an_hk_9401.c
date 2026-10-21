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

void course_highway_hit_spawn_fx(char *param_1)

{
  double fVar1;
  struct {
    char *local_1c;
    float local_18;
    float local_14;
    float local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame;
  
  frame.local_1c = 0;
  frame.local_c = 0;
  frame.local_8 = 0;
  frame.local_4 = 0;
  frame.local_18 = 0.0;
  frame.local_14 = 0.0;
  frame.local_10 = 0.0;
  FUN_004c4620(&frame.local_c,param_1,*(undefined4 *)(param_1 + 0x44));
  if (k_f_0_5 < fabs(*(float *)(param_1 + 0x38))) {
    fx_spawn_burst_default_scale(&frame.local_c,10,0,0,0,0x40000000);
    car_current_matrix(&frame.local_1c,*(undefined4 *)(param_1 + 0x3c));
    frame.local_18 = *(float *)(frame.local_1c + 0x20) * k_f_300;
    frame.local_14 = *(float *)(frame.local_1c + 0x24) * k_f_300;
    frame.local_10 = *(float *)(frame.local_1c + 0x28) * k_f_300;
    fVar1 = (double)rp_random_float_range(0xc2480000,0x42480000);
    frame.local_18 = (float)fVar1;
    car_enter_rwp_mode(*(undefined4 *)(param_1 + 0x3c));
    car_apply_rwp_impulse(*(undefined4 *)(param_1 + 0x3c),&frame.local_18,&frame.local_c);
  }
  return;
}


