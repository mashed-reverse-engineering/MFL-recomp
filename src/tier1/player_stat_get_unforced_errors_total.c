#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
#define EVENT_TYPE_NO_KILL_STATS (4)
/* player_stat_get_unforced_errors_total */
/* "total table unforced errors ('Unforced Errors Overall')" */
/*
 * Returns the 'Unforced Errors Overall' value (unforced_errors of the total table) for one player.
 * player_index: 0-3, index into g_player_stats_total / g_player_slots.
 * Returns 0 for event type EVENT_TYPE_NO_KILL_STATS (inferred: no such stats in that event).
 * Solo: that player's own total. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 */
int __cdecl player_stat_get_unforced_errors_total(int player_index)
{
  int slot_state;
  int total_unforced_errors;
  
  total_unforced_errors = get_event_type();
  if (total_unforced_errors == EVENT_TYPE_NO_KILL_STATS) {
    return 0;
  }
  if (is_team_mode() != 0) {
    slot_state = g_player_slots[player_index].state;
    total_unforced_errors = g_player_stats_total[player_index].unforced_errors;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[0].unforced_errors;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[1].unforced_errors;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[2].unforced_errors;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      total_unforced_errors = total_unforced_errors + g_player_stats_total[3].unforced_errors;
    }
    return total_unforced_errors;
  }
  return g_player_stats_total[player_index].unforced_errors;
}
