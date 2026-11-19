#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*005ddb14*/ extern float float_7_0;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c7e40*/ extern undefined4 distance_to_leader;


/* player_distance_warning */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* returns 1 in race mode when distance_to_leader[p] > 7.0 (trainer warning threshold patch at
   0044c16b) */

undefined4 __cdecl player_distance_warning(int param_1)

{
  int race_phase;
  
  race_phase = race_get_phase();
  if ((((race_phase == 6) && (game_mode != GAME_MODE_RACE)) && (game_mode != GAME_MODE_CHASE_FUGITIVE)) &&
     ((game_mode != GAME_MODE_SURVIVAL && (((float *)&distance_to_leader)[param_1] > float_7_0)))) {
    return 1;
  }
  return 0;
}
