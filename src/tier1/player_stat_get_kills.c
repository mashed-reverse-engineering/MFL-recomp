#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/* player_stat_get_kills */
/* "result table kills, summed over the team in team mode (kind 4, 'Kills')" */
/*
 * Returns the kills statistic shown in the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Returns 0 for event type 4 (inferred: no kill stats in that event).
 * In team mode, adds the kills of every other player whose slot state
 * matches this player's (inferred: same team).
 */
int __cdecl player_stat_get_kills(int player_index)
{
  int slot_state;
  int total_kills;

  total_kills = get_event_type();
  if (total_kills == EVENT_CUP_4) {
    return 0;
  }
  total_kills = is_team_mode();
  if (total_kills != 0) {
    slot_state = g_player_slots[player_index].state;
    total_kills = g_player_stats_result[player_index].kills;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      total_kills = total_kills + g_player_stats_result[0].kills;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      total_kills = total_kills + g_player_stats_result[1].kills;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      total_kills = total_kills + g_player_stats_result[2].kills;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      total_kills = total_kills + g_player_stats_result[3].kills;
    }
  }
  else {
    total_kills = g_player_stats_result[player_index].kills;
  }
  return total_kills;
}
