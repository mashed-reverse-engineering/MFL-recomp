#include "mfl_tier2.h"
/*00492b50*/ extern undefined __cdecl car_add_velocity_impl(void *p0, undefined4 p1);
/*008b06e0*/ extern struct Car player_car[16];


/* car_add_velocity */

/* "(delta vec, car): adds to Car.velocity, or applies an impulse in RWP mode" */

undefined4 car_add_velocity(float *param_1,uint param_2)
{
  if (param_2 >= 16) {
    return 0;
  }
  if (player_car[param_2].rwp_object != 0) {
    car_add_velocity_impl(param_1,param_2);
    return 1;
  }
  player_car[param_2].velocity[0] = player_car[param_2].velocity[0] + *param_1;
  player_car[param_2].velocity[1] = param_1[1] + player_car[param_2].velocity[1];
  player_car[param_2].velocity[2] = param_1[2] + player_car[param_2].velocity[2];
  return 1;
}
