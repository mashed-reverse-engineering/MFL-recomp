#include "mfl_tier2.h"
/*004a5ed0*/ extern int __cdecl video_raster_create(undefined4 p0);
/* video_raster_create_forward: forwards its first argument (raster_arg) to video_raster_create.
   unused_arg is never read (inferred). Returns nothing. */
/* "calls video_raster_create" */
int __cdecl video_raster_create_forward(undefined4 raster_arg,char *unused_arg)
{
  return video_raster_create(raster_arg);
  return;
}
