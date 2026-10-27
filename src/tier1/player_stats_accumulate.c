#include "mfl_tier2.h"
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];


/* player_stats_accumulate */

/* "adds the per race values to the running totals (0x8C9540 / 0x8C9594 ...)" */

void player_stats_accumulate(void)
{
  struct PlayerStatRecord *p_3;
  struct PlayerStatRecord *p_2;
  struct PlayerStatRecord *p_1;
  struct PlayerStatRecord *p_0;
  p_0 = &g_player_stats_total[0];
  p_0->aggression =
       p_0->aggression + g_player_stats_result[0].aggression;
  p_0->crowns = p_0->crowns + g_player_stats_result[0].crowns;
  p_0->kills = p_0->kills + g_player_stats_result[0].kills;
  p_0->pants = p_0->pants + g_player_stats_result[0].pants;
  p_0->unforced_errors =
       p_0->unforced_errors + g_player_stats_result[0].unforced_errors;
  p_0->wall_hits =
       p_0->wall_hits + g_player_stats_result[0].wall_hits;
  p_0->turbo_used =
       p_0->turbo_used + g_player_stats_result[0].turbo_used;
  p_0->place = p_0->place + g_player_stats_result[0].place;
  p_1 = &g_player_stats_total[1];
  p_1->aggression =
       p_1->aggression + g_player_stats_result[1].aggression;
  p_1->crowns = p_1->crowns + g_player_stats_result[1].crowns;
  p_1->kills = p_1->kills + g_player_stats_result[1].kills;
  p_1->pants = p_1->pants + g_player_stats_result[1].pants;
  p_1->unforced_errors =
       p_1->unforced_errors + g_player_stats_result[1].unforced_errors;
  p_1->wall_hits =
       p_1->wall_hits + g_player_stats_result[1].wall_hits;
  p_1->turbo_used =
       p_1->turbo_used + g_player_stats_result[1].turbo_used;
  p_1->place = p_1->place + g_player_stats_result[1].place;
  p_2 = &g_player_stats_total[2];
  p_2->aggression =
       p_2->aggression + g_player_stats_result[2].aggression;
  p_2->crowns = p_2->crowns + g_player_stats_result[2].crowns;
  p_2->kills = p_2->kills + g_player_stats_result[2].kills;
  p_2->pants = p_2->pants + g_player_stats_result[2].pants;
  p_2->unforced_errors =
       p_2->unforced_errors + g_player_stats_result[2].unforced_errors;
  p_2->wall_hits =
       p_2->wall_hits + g_player_stats_result[2].wall_hits;
  p_2->turbo_used =
       p_2->turbo_used + g_player_stats_result[2].turbo_used;
  p_2->place = p_2->place + g_player_stats_result[2].place;
  p_3 = &g_player_stats_total[3];
  p_3->aggression =
       p_3->aggression + g_player_stats_result[3].aggression;
  p_3->crowns = p_3->crowns + g_player_stats_result[3].crowns;
  p_3->kills = p_3->kills + g_player_stats_result[3].kills;
  p_3->pants = p_3->pants + g_player_stats_result[3].pants;
  p_3->unforced_errors =
       p_3->unforced_errors + g_player_stats_result[3].unforced_errors;
  p_3->wall_hits =
       p_3->wall_hits + g_player_stats_result[3].wall_hits;
  p_3->turbo_used =
       p_3->turbo_used + g_player_stats_result[3].turbo_used;
  p_3->place = p_3->place + g_player_stats_result[3].place;
  return;
}
