#include "mfl_tier2.h"
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];


/* game_get_game_8c8ae0 */

/* "return g_game_8c8ae0;" */

int game_get_game_8c8ae0(void)
{
  return g_player_stats_race[0].place;
}
