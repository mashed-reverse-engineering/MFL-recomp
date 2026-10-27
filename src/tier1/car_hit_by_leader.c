#include "mfl_tier2.h"

#define CAR_MODE_NORMAL 0
#define TRAIL_PARAM_UNKNOWN_3 3 /* unknown trail parameter ID */
#define TRAIL_COUNT 4           /* number of trail slots */
#define LEADER_HIT_TIMEOUT 6000 /* unknown duration unit: likely ms (6s) or ticks */

/*004162f0*/ extern int trail_set_param();
/*007ef7bc*/ extern undefined4 g_cars_7ef7bc;
/*007ef7c4*/ extern undefined4 g_cars_7ef7c4;
/*008b06e0*/ extern struct Car player_car[16];


/* car_hit_by_leader */

/* Handles collision when the leader car touches another car in game mode 1. */

void __cdecl car_hit_by_leader(int car_index)
{
  int trail_index;
  
  /* Check if target car is active and in normal mode */
  if ((player_car[car_index].mode == CAR_MODE_NORMAL) && (player_car[car_index].active != 0)) {
    trail_index = 0;
    /* Reset trail effect parameters for all trails (unknown parameter 3) */
    do {
      trail_set_param(trail_index,TRAIL_PARAM_UNKNOWN_3,0);
      trail_index = trail_index + 1;
    } while (trail_index < TRAIL_COUNT);
    /* Record hit car index and set collision timeout (unknown: 6000 ms or ticks) */
    g_cars_7ef7bc = car_index;
    g_cars_7ef7c4 = LEADER_HIT_TIMEOUT;
  }
  return;
}

