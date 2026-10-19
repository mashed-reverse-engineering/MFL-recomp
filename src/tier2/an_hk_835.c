#include "mfl_tier2.h"
/*00771a10*/ extern float g_video_get_771a10;
/*00771a14*/ extern float g_video_get_771a14;
/* video_get_window_size_a */

/* Writes the window size into out_size: out_size[0] gets the raw 32-bit pattern of
 * g_video_get_771a10 (width, inferred), out_size[1] gets g_video_get_771a14 as a float
 * (height, inferred). Returns nothing.
 * (was: "(out w, out h) from g_app_771a10 / 771a14" - g_app_ name is stale) */

void video_get_window_size_a(undefined4 *out_size)

{
  *out_size = *(int *)&g_video_get_771a10;
  *(float *)&out_size[1] = g_video_get_771a14;
  return;
}


