#include "mfl_tier2.h"
/*00464c30*/ extern int powerup_place();
/*00464dd0*/ extern int powerup_set_respawn_time(undefined4 index, undefined4 respawn);
/* powerup_place_with_respawn */

/* "powerup_place, then powerup_set_respawn_time(index, param_3): the 3rd argument stays on the stack as 2nd argument" */

void __cdecl powerup_place_with_respawn(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 powerup_index;
  
  powerup_index = powerup_place(param_1,param_2,param_3);
  powerup_set_respawn_time(powerup_index,param_3);
  return;
}


