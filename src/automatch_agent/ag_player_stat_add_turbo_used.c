#include "mfl_tier2.h"
/*0040ff20*/ extern int race_setup_get_slot_type();
/*00481fd0*/ extern float __cdecl random_range(undefined4 p0, undefined4 p1);
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];


/* player_stat_add_turbo_used */

/* "(player, amount): g_player_stats_race[player].turbo_used += amount (called when the turbo charge
   is spent)" */

void __cdecl player_stat_add_turbo_used(int player,int requested_amount)

{
  int slot_type;
  int idx;
  int amount;
  
  idx = player;
  amount = requested_amount;
  slot_type = race_setup_get_slot_type(idx);
  if (slot_type != 1) {
    slot_type = (int)random_range(0,0x43960000);
    amount = amount - slot_type;
    if (amount < 0) {
      amount = 0;
    }
  }
  g_player_stats_race[idx].turbo_used = g_player_stats_race[idx].turbo_used + amount;
  return;
}
