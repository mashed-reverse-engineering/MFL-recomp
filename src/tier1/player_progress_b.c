#include "mfl_tier2.h"
/*008d8d4c*/ extern undefined g_player_8d8d4c;

#define PLAYER_RECORD_STRIDE 0x30c /* bytes per player record */


/* player_progress_b */

/* Returns the progress value (float) of the given player record. */

double player_progress_b(int player_index)
{
  return (double)*(float *)(&g_player_8d8d4c + player_index * PLAYER_RECORD_STRIDE);
}
