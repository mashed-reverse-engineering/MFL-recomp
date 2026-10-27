#include "mfl_tier2.h"
/*00467eb0*/ extern int __cdecl player_has_no_weapon(undefined4 p0);


/* player_has_weapon */

/* Check if the player (car index) already carries a weapon to determine if pick-up is refused. */

bool __cdecl player_has_weapon(int car_index)
{
  int has_no_weapon;
  
  has_no_weapon = player_has_no_weapon(car_index);
  return has_no_weapon == 0;
}
