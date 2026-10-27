#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0047bc60*/ extern int car_get_field_8();
/*005dd328*/ extern float k_f_0_6;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd628*/ extern float k_f_0_25;
/*005ddc28*/ extern float k_f_0;
/*007ef7b8*/ extern undefined4 game_mode;
/*008c7e40*/ extern undefined4 distance_to_leader;
/*008c7e60*/ extern undefined4 g_distances_8c7e60;
/* camera_get_distance_factor */

/* "1.0 in modes 4 / 8 / 9, else based on the distance to the leader" */

/*
 * Returns the camera distance multiplier as 1.0 + factor * distance (double / x87 st0).
 * player_index: index of the car the camera follows.
 * unused_arg: not read by this function.
 * distance: camera distance input, scaled by a factor picked from the car's and the leader's field_8 (inferred).
 * Returns 1.0 in game modes 4, 8 and 9. Otherwise the factor is 0 unless distance_to_leader[player_index] == k_f_0 (inferred), and the result is 1.0.
 */
double camera_get_distance_factor(int player_index,char *unused_arg,float distance)

{
  int player_field_8;
  int leader_field_8;
  
  /* Modes 4, 8, 9: no distance scaling. */
  if (((game_mode == GAME_MODE_RACE) || (game_mode == GAME_MODE_CHASE_FUGITIVE)) || (game_mode == GAME_MODE_SURVIVAL)) {
    return (double)float_1_0;
  }
  get_player_count();
  /* Scale only when this car's distance_to_leader entry equals k_f_0 (inferred: 0.0 meaning "no gap set"). */
  if (*(float *)(((undefined *)&distance_to_leader) + player_index * 4) == k_f_0) {
    player_field_8 = car_get_field_8(player_index);
    leader_field_8 = car_get_field_8(g_distances_8c7e60);
    /* Factor table by (player_field_8, leader_field_8) (inferred: ordering of the two cars). */
    if (player_field_8 == 0) {
      if (leader_field_8 == 3) {
        return (double)k_f_0_6 * (double)distance + (double)float_1_0;
      }
      if (leader_field_8 == 2) {
LAB_0044c240:
        return (double)k_f_0_5 * (double)distance + (double)float_1_0;
      }
      if (leader_field_8 == 1) {
        return (double)k_f_0_25 * (double)distance + (double)float_1_0;
      }
    }
    else if (player_field_8 == 1) {
      if (leader_field_8 == 3) goto LAB_0044c240;
      if (leader_field_8 == 2) {
        return (double)k_f_0_25 * (double)distance + (double)float_1_0;
      }
    }
    else if ((player_field_8 == 2) && (leader_field_8 == 3)) {
      return (double)k_f_0_25 * (double)distance + (double)float_1_0;
    }
  }
  return (double)0.0 * (double)distance + (double)float_1_0;
}


