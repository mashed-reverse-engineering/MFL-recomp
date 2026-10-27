#include "mfl_tier2.h"
/*004a5f00*/ extern int video_raster_destroy(void);
/* Destroys the video raster (tail-calls video_raster_destroy). */
int raster_destroy_771038(void)
{
  return video_raster_destroy();
}
