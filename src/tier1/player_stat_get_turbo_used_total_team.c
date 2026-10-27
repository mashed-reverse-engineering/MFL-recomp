#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
/* player_stat_get_turbo_used_total_team */
/* "total table +0xFC (team sum, 'Turbo Skills Overall')" */
/*
 * Returns the 'Turbo Skills Overall' value (turbo_used of the total table) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Solo: that player's own total. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
int player_stat_get_turbo_used_total_team(int player_index)
{
  int slot_state;
  int turbo_used_total;

  /* is_team_mode() result first (0 = solo), then overwritten with the total */
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    turbo_used_total = g_player_stats_total[player_index].turbo_used;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[0].turbo_used;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[1].turbo_used;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[2].turbo_used;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      turbo_used_total = turbo_used_total + g_player_stats_total[3].turbo_used;
    }
    return turbo_used_total;
  }
  return g_player_stats_total[player_index].turbo_used;
}
