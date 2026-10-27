#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/* player_stat_get_wall_hits_team */
/* "result table wall_hits, summed over the team (kind 5, 'Cleanest Drive', lower is better)" */
/*
 * Returns the 'Cleanest Drive' statistic (wall_hits of the result table) for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own wall_hits. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 * Lower is better.
 */
int player_stat_get_wall_hits_team(int player_index)
{
  int slot_state;
  int team_wall_hits;
  
  /* is_team_mode() result first (0 = solo), then overwritten with the wall_hits value */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    team_wall_hits = g_player_stats_result[player_index].wall_hits;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[0].wall_hits;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[1].wall_hits;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[2].wall_hits;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      team_wall_hits = team_wall_hits + g_player_stats_result[3].wall_hits;
    }
    return team_wall_hits;
  }
  return g_player_stats_result[player_index].wall_hits;
}
