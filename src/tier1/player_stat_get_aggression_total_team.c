#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
/* player_stat_get_aggression_total_team */
/* "total table aggression (team sum, 'Aggression Overall')" */
/* (player_index): returns the 'Aggression Overall' value shown for one player in the total table.
   Outside team mode it is that player's own total aggression. In team mode it is the sum over the
   player and every other player slot with the same team state (slot_state). */
int player_stat_get_aggression_total_team(int player_index)
{
  int slot_state;
  int aggression_total;

  aggression_total = is_team_mode();
  if (aggression_total != 0) {
    /* team mode: start with this player's own total, then add each teammate
       (same slot state, not this player). All four slots are tested in order. */
    slot_state = g_player_slots[player_index].state;
    aggression_total = g_player_stats_total[player_index].aggression;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      aggression_total = aggression_total + g_player_stats_total[0].aggression;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      aggression_total = aggression_total + g_player_stats_total[1].aggression;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      aggression_total = aggression_total + g_player_stats_total[2].aggression;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      aggression_total = aggression_total + g_player_stats_total[3].aggression;
    }
  }
  else {
    aggression_total = g_player_stats_total[player_index].aggression;
  }
  return aggression_total;
}
