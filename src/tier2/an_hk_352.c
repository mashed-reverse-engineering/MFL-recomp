#include "mfl_tier2.h"
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659d00*/ extern undefined4 g_game_659d00;
extern void vehicle_markers_render_parts_n(char *block);

/* vehicle_markers_render_if_loaded */

/* "game_fn_421720 when g_game_659d00 is set" */

/*
 * Renders the vehicle markers (inferred from the callee name) once the game is set up.
 * Does nothing while g_game_659d00 is 0. Otherwise passes the address of g_load_659b30
 * to vehicle_markers_render_parts (its natural-C body vehicle_markers_render_parts_n).
 * Takes no parameters and returns nothing.
 */
void vehicle_markers_render_if_loaded(void)

{
  /* Only draw when the game flag is set (inferred: game has loaded). */
  if (g_game_659d00 != 0) {
    vehicle_markers_render_parts_n((char *)(&g_load_659b30));
  }
  return;
}


