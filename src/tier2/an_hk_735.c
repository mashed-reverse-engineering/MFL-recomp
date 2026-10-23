#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*00434a10*/ extern int is_team_mode();
/*007ef7b8*/ extern undefined4 game_mode;
/*008d8b90*/ extern undefined4 g_player_8d8b90;
/*008d8ba0*/ extern undefined4 g_round_8d8ba0;
/*008d8bb0*/ extern undefined4 g_round_8d8bb0;
/*008d8bc0*/ extern undefined4 g_round_8d8bc0;
#define TEAM_COMPETITOR_COUNT (2)
#define COUNTER_ENTRY_BYTES (4)
/* get_competitor_count */

/* Adds one to the result counter for (player_index, placement), where placement is
   0-based (inferred: 0 = first place). The table depends on the competitor count, which
   is TEAM_COMPETITOR_COUNT (2) in team mode (unless game_mode is 2), else get_player_count().
   Tables by placement:
     2 competitors: g_round_8d8bc0, g_player_8d8b90
     3 competitors: g_round_8d8ba0, g_round_8d8bc0, g_player_8d8b90
     4 competitors: g_round_8d8bb0, g_round_8d8ba0, g_round_8d8bc0, g_player_8d8b90
   Other placements, and counts other than 2, 3 or 4, count nothing.
   Each counter is an int at COUNTER_ENTRY_BYTES per player_index. Returns nothing. */

void get_competitor_count(int player_index,int placement)

{
  int competitor_count;

  competitor_count = is_team_mode();
  if ((competitor_count == 0) || (game_mode == GAME_MODE_HARE_AND_HOUNDS)) {
    competitor_count = get_player_count();
  }
  else {
    competitor_count = TEAM_COMPETITOR_COUNT;
  }
  /* Not 2: only counts 3 and 4 have tables; any other count returns without counting. */
  if (competitor_count != 2) {
    if (competitor_count != 3) {
      if (competitor_count != 4) {
        return;
      }
      /* 4 competitors: each placement has its own table. */
      switch(placement) {
      case 0:
        *(int *)((char *)((undefined *)&g_round_8d8bb0) + player_index * COUNTER_ENTRY_BYTES) = *(int *)((char *)((undefined *)&g_round_8d8bb0) + player_index * COUNTER_ENTRY_BYTES) + 1;
        return;
      case 1:
        goto switchD_0040ccd2_caseD_1;
      case 2:
        goto switchD_0040ccd2_caseD_2;
      case 3:
        goto switchD_0040ccd2_caseD_3;
      default:
        return;
      }
    }
    /* 3 competitors: placement 0 has its own table; later placements shift down by one. */
    if (placement == 0) {
switchD_0040ccd2_caseD_1:
      *(int *)((char *)((undefined *)&g_round_8d8ba0) + player_index * COUNTER_ENTRY_BYTES) = *(int *)((char *)((undefined *)&g_round_8d8ba0) + player_index * COUNTER_ENTRY_BYTES) + 1;
      return;
    }
    placement = placement + -1;
  }
  /* 2 competitors, or 3 with placement already shifted down by one: 0 -> first table, 1 -> player table. */
  if (placement == 0) {
switchD_0040ccd2_caseD_2:
    *(int *)((char *)((undefined *)&g_round_8d8bc0) + player_index * COUNTER_ENTRY_BYTES) = *(int *)((char *)((undefined *)&g_round_8d8bc0) + player_index * COUNTER_ENTRY_BYTES) + 1;
  }
  else if (placement == 1) {
switchD_0040ccd2_caseD_3:
    (&g_player_8d8b90)[player_index] = (&g_player_8d8b90)[player_index] + 1;
    return;
  }
  return;
}


