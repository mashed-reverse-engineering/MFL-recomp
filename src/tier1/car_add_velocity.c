#include "mfl_tier2.h"
/*00492b50*/ extern undefined __cdecl car_add_velocity_impl(void *p0, undefined4 p1);
/*008b06e0*/ extern struct Car player_car[16];


/* car_add_velocity */

/* "(delta vec, car): adds to Car.velocity, or applies an impulse in RWP mode" */

undefined4 car_add_velocity(float *delta,uint car_index)
{
  if (car_index >= 16) {
    return 0;
  }
  if (player_car[car_index].rwp_object != 0) {
    car_add_velocity_impl(delta,car_index);
    return 1;
  }
  player_car[car_index].velocity[0] = player_car[car_index].velocity[0] + *delta;
  player_car[car_index].velocity[1] = delta[1] + player_car[car_index].velocity[1];
  player_car[car_index].velocity[2] = delta[2] + player_car[car_index].velocity[2];
  return 1;
}
