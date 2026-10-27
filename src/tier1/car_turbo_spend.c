#include "mfl_tier2.h"
/*00426670*/ extern undefined __cdecl player_stat_add_turbo_used(undefined4 p0, undefined4 p1);
/*008b06e0*/ extern struct Car player_car[16];


/* car_turbo_spend */

/* "(car): if the turbo charge is above 1000 spends it (state 2), else converts the charge (state 1,
   +1000); adds the used charge to the player's turbo_used statistic" */

undefined4 car_turbo_spend(int car_index)
{
  int charge;
  
  if (car_index >= 16) {
    return 0;
  }
  charge = player_car[car_index].turbo_charge;
  if (1000 < charge) {
    player_car[car_index].turbo_state = 2;
    player_stat_add_turbo_used(car_index,1000 - charge);
    return 1;
  }
  if (0 < charge) {
    player_car[car_index].turbo_state = 1;
    player_car[car_index].turbo_charge = charge + 1000;
    player_stat_add_turbo_used(car_index,charge + 1000);
  }
  return 1;
}
