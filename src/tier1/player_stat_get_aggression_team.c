#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/* player_stat_get_aggression_team */
/* "result table aggression, summed over the team in team mode (kind 3, 'Aggression')" */
/*
 * Returns the 'Aggression' statistic from the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Solo: that player's own aggression. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
int player_stat_get_aggression_team(int player_index_in)
{
  int player_index = player_index_in;
  int slot_state;
  int aggression_team;

  /* is_team_mode() result first (0 = solo), then overwritten with the aggression value */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    aggression_team = g_player_stats_result[player_index].aggression;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      aggression_team = aggression_team + g_player_stats_result[0].aggression;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      aggression_team = aggression_team + g_player_stats_result[1].aggression;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      aggression_team = aggression_team + g_player_stats_result[2].aggression;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      aggression_team = aggression_team + g_player_stats_result[3].aggression;
    }
    return aggression_team;
  }
  return g_player_stats_result[player_index].aggression;
}
