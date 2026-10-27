#include "mfl_tier2.h"
/*0047a5b0*/ extern int car_add_velocity();
/*0047b6e0*/ extern int car_get_velocity();


/* car_scale_velocity */

/* Scales the velocity of a car by the given factor. */

void __cdecl car_scale_velocity(int car, float factor)
{
  float velocity[3];
#define velocity_x velocity[0]
#define velocity_y velocity[1]
#define velocity_z velocity[2]
  
  /* Read current velocity */
  car_get_velocity(&velocity_x, car);
  velocity_x = velocity_x * factor;
  velocity_y = velocity_y * factor;
  velocity_z = velocity_z * factor;
  /* Write scaled velocity back */
  car_add_velocity(&velocity_x, car);
  return;
}

