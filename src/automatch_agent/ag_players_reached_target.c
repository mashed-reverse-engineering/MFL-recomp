#include "mfl_tier2.h"
/*008d8b40*/ extern undefined4 player_points;
void players_reached_target(undefined4 *reached_flag)

{
  int target;
  int players_left;
  char *points_offset;
  
  points_offset = (char *)&player_points - (int)reached_flag;
  players_left = 4;
  do {
    *reached_flag = 0;
    target = score_target_b();
    if (*(int *)(points_offset + (int)reached_flag) >= target) {
      *reached_flag = 1;
    }
    reached_flag = reached_flag + 1;
    players_left = players_left + -1;
  } while (players_left != 0);
  return;
}
