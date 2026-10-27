#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
/* player_stat_get_wall_hits_total_team */
/* "total table wall_hits ('Cleanest Drive Overall')" */
/*
 * Returns the 'Cleanest Drive Overall' value (wall_hits of the total table, lower is better) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Solo: that player's own total. Team mode: summed over every player whose slot_state
 * matches this player's (inferred: same team), this player counted once.
 */
int player_stat_get_wall_hits_total_team(int player_index)
{
  int slot_state;
  int wall_hits_total;

  /* is_team_mode() result first (0 = solo), then overwritten with the total */
  wall_hits_total = is_team_mode();
  if (wall_hits_total != 0) {
    slot_state = g_player_slots[player_index].state;
    wall_hits_total = g_player_stats_total[player_index].wall_hits;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[0].wall_hits;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[1].wall_hits;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[2].wall_hits;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      wall_hits_total = wall_hits_total + g_player_stats_total[3].wall_hits;
    }
  }
  else {
    wall_hits_total = g_player_stats_total[player_index].wall_hits;
  }
  return wall_hits_total;
}
