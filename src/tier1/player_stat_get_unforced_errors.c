#include "mfl_tier2.h"
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/* event type whose race results carry no kill-type statistics (inferred from the kills family) */
#define EVENT_TYPE_NO_KILL_STATS (4)
/* player_stat_get_unforced_errors */
/* "result table unforced errors, summed over the team (kind 6, 'Unforced Errors')" */
/*
 * Returns the 'Unforced Errors' statistic shown in the race results for one player.
 * player_index: 0-3, index into g_player_stats_result / g_player_slots.
 * Returns 0 for event type EVENT_TYPE_NO_KILL_STATS (inferred: no such stats in that event).
 * Solo: that player's own unforced_errors. Team mode: summed over every player whose slot state
 * matches this player's (inferred: same team), this player counted once.
 */
int __cdecl player_stat_get_unforced_errors(int player_index)
{
  int slot_state;
  int result;
  
  /* event type EVENT_TYPE_NO_KILL_STATS: no kill-type stats (inferred) */
  result = get_event_type();
  if (result == EVENT_TYPE_NO_KILL_STATS) {
    return 0;
  }
  /* solo: this player's own value only; the is_team_mode() result is overwritten below */
  result = is_team_mode();
  if (result != 0) {
    slot_state = g_player_slots[player_index].state;
    result = g_player_stats_result[player_index].unforced_errors;
    if ((g_player_slots[0].state == slot_state) && (player_index != 0)) {
      result = result + g_player_stats_result[0].unforced_errors;
    }
    if ((g_player_slots[1].state == slot_state) && (player_index != 1)) {
      result = result + g_player_stats_result[1].unforced_errors;
    }
    if ((g_player_slots[2].state == slot_state) && (player_index != 2)) {
      result = result + g_player_stats_result[2].unforced_errors;
    }
    if ((g_player_slots[3].state == slot_state) && (player_index != 3)) {
      result = result + g_player_stats_result[3].unforced_errors;
    }
  }
  else {
    result = g_player_stats_result[player_index].unforced_errors;
  }
  return result;
}
