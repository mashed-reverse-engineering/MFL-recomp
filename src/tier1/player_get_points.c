#include "mfl_tier2.h"
/*008d8b40*/ extern undefined4 player_points;


/* player_get_points */

/* "player_points[i]" */

/* Returns the points of the specified player. */
undefined4 player_get_points(int player_index)
{
  return (&player_points)[player_index];
}
