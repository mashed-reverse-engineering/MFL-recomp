#include "mfl_tier2.h"
/*0047a830*/ extern int car_is_active();
/*004b4ba7*/ extern int entry();
/*008d8b40*/ extern undefined4 player_points;
#define PLAYER_SLOTS (4)
/* players_max_points */

/*
 * Sets winner_flags[0..PLAYER_SLOTS-1] to 1 for every player slot whose player_points
 * equals the highest value, and to 0 for the others.
 * active_only: 0 = consider all slots; nonzero = only slots where car_is_active() is true
 * (inferred; the old comment said "only human").
 * Returns nothing.
 */

void players_max_points(undefined4 *winner_flags,int active_only)

{
  int car_active;
  int car_index;
  int max_points;

  max_points = 0;
  car_index = 0;
  /* Pass 1: find the highest player_points among the considered slots. */
  do {
    if (active_only == 0) {
LAB_0040cf61:
      if (max_points < (int)(&player_points)[car_index]) {
        max_points = (&player_points)[car_index];
      }
    }
    else {
      car_active = car_is_active(car_index);
      if (car_active != 0) goto LAB_0040cf61;
    }
    car_index = car_index + 1;
    if ((PLAYER_SLOTS - 1) < car_index) {
      /* Pass 2: car_index becomes the byte distance from winner_flags to player_points
         (inferred), so *(int *)(car_index + winner_flags) reads player_points[k]. */
      car_index = (int)&player_points - (int)winner_flags;
      car_active = PLAYER_SLOTS;
      do {
        *winner_flags = 0;
        if (*(int *)(car_index + (LONG_PTR)winner_flags) == max_points) {
          *winner_flags = 1;
        }
        winner_flags = winner_flags + 1;
        car_active = car_active + -1;
      } while (car_active != 0);
      return;
    }
  } while( true );
}
