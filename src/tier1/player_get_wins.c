#include "mfl_tier2.h"
/*008d8b50*/ extern undefined4 g_player_8d8b50;


/* player_get_wins */

/* "0x8D8B50[player]" */

/* Returns the number of wins for the specified player. */
undefined4 player_get_wins(int player_index)
{
  return (&g_player_8d8b50)[player_index];
}
