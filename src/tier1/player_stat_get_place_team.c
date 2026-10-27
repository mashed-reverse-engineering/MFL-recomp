#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];


/* player_stat_get_place_team */

/* "result table place (team sum over controllers in use)" */

/* Returns the player's place from g_player_stats_result; in team mode it adds the places of
   the other slots that share the player's slot state and have a controller (controller != -1). */
int player_stat_get_place_team(int player_index)
{
  int team_key;                        /* slot state of this player in team mode, then reused as a flag */
  int place_sum;

  team_key = is_team_mode();
  if (team_key != 0) {
    team_key = g_player_slots[player_index].state;
    place_sum = g_player_stats_result[player_index].place;
    if (((g_player_slots[0].state == team_key) && (player_index != 0)) &&
       (g_player_slots[0].controller != -1)) {
      place_sum = place_sum + g_player_stats_result[0].place;
    }
    if (((g_player_slots[1].state == team_key) && (player_index != 1)) &&
       (g_player_slots[1].controller != -1)) {
      place_sum = place_sum + g_player_stats_result[1].place;
    }
    if (((g_player_slots[2].state == team_key) && (player_index != 2)) &&
       (g_player_slots[2].controller != -1)) {
      place_sum = place_sum + g_player_stats_result[2].place;
    }
    if (((g_player_slots[3].state == team_key) && (player_index != 3)) &&
       (g_player_slots[3].controller != -1)) {
      place_sum = place_sum + g_player_stats_result[3].place;
    }
    return place_sum;
  }
  return g_player_stats_result[player_index].place;
}
