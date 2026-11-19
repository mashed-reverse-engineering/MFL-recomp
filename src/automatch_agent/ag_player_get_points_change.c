#include "mfl_tier2.h"
/*008d8b80*/ extern undefined4 g_round_8d8b80;


/* player_get_points_change */

/* "0x8D8B80[player]" */

undefined4 player_get_points_change(int param_1)

{
  return (&g_round_8d8b80)[param_1];
}
