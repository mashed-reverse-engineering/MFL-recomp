#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*00801b2c*/ extern float g_camera_fov_factor;
/* camera_fov_factor_set_wide */

/* offset in g_game_state (0xafc, inside the _gap_ae6 padding) where the wide flag is stored (inferred) */
#define WIDE_FLAG_OFFSET (2812)
/* g_camera_fov_factor when the wide flag is 0 (inferred: 4:3 view) */
#define FOV_FACTOR_NORMAL (0.75f)
/* g_camera_fov_factor when the wide flag is nonzero (inferred: 16:9 view, 0.75 * 0.75) */
#define FOV_FACTOR_WIDE (0.5625f)

/* "stores g_game_state+2812 and sets g_camera_fov_factor 0.75 or 0.5625" */

/* Sets the wide-screen flag in g_game_state and picks the matching camera FOV factor.
   is_wide: nonzero = wide (16:9), 0 = normal. Returns nothing. */
void __cdecl camera_fov_factor_set_wide(int is_wide)

{
  (*(uint *)((char *)&g_game_state + WIDE_FLAG_OFFSET)) = is_wide;
  g_camera_fov_factor = FOV_FACTOR_NORMAL;
  if (is_wide != 0) {
    g_camera_fov_factor = FOV_FACTOR_WIDE;
  }
  return;
}


