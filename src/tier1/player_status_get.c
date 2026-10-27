#include "mfl_tier2.h"

/* Size in bytes of one g_player_status entry. */
#define PLAYER_STATUS_ENTRY_SIZE 4

/*006aac18*/ extern undefined g_player_status;


/* player_status_get */

/* Status word of `player` from g_player_status. */
/* "(player): g_player_status[player]" */

undefined4 __cdecl player_status_get(int player)
{
  return *(undefined4 *)(&g_player_status + player * PLAYER_STATUS_ENTRY_SIZE);
}
