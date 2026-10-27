#include "mfl_tier2.h"
/*00652dec*/ extern undefined4 g_race_652dec;


/* race_set_view_player */

/* Selects the player whose view the camera shows. */
/* "0x652DEC = player index shown by the camera" */

void race_set_view_player(undefined4 player)
{
  g_race_652dec = player;
  return;
}
