#include "mfl_tier2.h"
/*0047a100*/ extern int car_reset_state();
/*00491d70*/ extern int car_reset_physics();

#define CAR_RESET_SUCCESS 1

/* Fully resets a car by resetting its physics and state. */
undefined4 __cdecl car_reset_full(undefined4 car)
{
  car_reset_physics(car);
  car_reset_state(car);
  return CAR_RESET_SUCCESS;
}
