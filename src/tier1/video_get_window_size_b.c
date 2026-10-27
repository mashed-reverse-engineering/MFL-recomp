#include "mfl_tier2.h"
/*00771a24*/ extern undefined4 g_video_get_771a24;
/*00771a28*/ extern undefined4 g_video_get_771a28;


/* Retrieves the current video window width and height from global state. */
void video_get_window_size_b(int *out_width, int *out_height)
{
  *out_width = g_video_get_771a24;
  *out_height = g_video_get_771a28;
  return;
}
