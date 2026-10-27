#include "mfl_tier2.h"
/*008d8b90*/ extern undefined4 g_player_8d8b90;


/* player_get_stat_8b90 */

/* "0x8D8B90[player]" */

/* Retrieves an unknown player statistic by player index. */
undefined4 player_get_stat_8b90(int player_index)
{
  return (&g_player_8d8b90)[player_index];
}
