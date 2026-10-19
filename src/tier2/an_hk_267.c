#include "mfl_tier2.h"
/*005dfd60*/ extern float k_f_160;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008b06e0*/ extern struct Car player_car[16];
#define MAX_CARS (16)
#define TURBO_CHARGE_MAX (3000)
#define TURBO_CHARGE_GAIN (2) /* turbo_charge gained per dt while accelerate is above 160 */
/* car_turbo_charge_update */

/* Advances the turbo charge of one car by one update step.
   car_index: index into player_car (0..MAX_CARS-1); out of range returns 0 and changes nothing.
   dt: step length (inferred: ms). Returns 1 when the car was updated.
   The charge is clamped to 0..TURBO_CHARGE_MAX. */

/* "(car, dt): turbo_charge += 2 * dt while the accelerate input is above 160, then -= dt, clamped
   to 0..3000" */

undefined4 __cdecl car_turbo_charge_update(int car_index,int dt)

{
  int *turbo_charge_ptr;
  int charge;

  /* reject car indices past the end of player_car */
  if (MAX_CARS - 1 < car_index) {
    return 0;
  }
  /* accelerate input above 160 gains charge at TURBO_CHARGE_GAIN * dt */
  if (k_f_160 < (float)g_player_inputs[g_player_slots[car_index].controller].accelerate) {
    player_car[car_index].turbo_charge = player_car[car_index].turbo_charge + dt * TURBO_CHARGE_GAIN;
  }
  /* the charge always drains by dt; store the result, then clamp it below */
  turbo_charge_ptr = &player_car[car_index].turbo_charge;
  charge = player_car[car_index].turbo_charge - dt;
  *turbo_charge_ptr = charge;
  /* clamp the charge to 0..TURBO_CHARGE_MAX */
  if (TURBO_CHARGE_MAX < charge) {
    *turbo_charge_ptr = TURBO_CHARGE_MAX;
  }
  if (*turbo_charge_ptr < 0) {
    *turbo_charge_ptr = 0;
  }
  return 1;
}


