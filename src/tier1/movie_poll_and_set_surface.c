#include "mfl_tier2.h"
/*004a2d20*/ extern int rw_set_value_at_root_plus_947144();
/*004a34f0*/ extern int app_get_app_76e8a4();
/*004a37f0*/ extern int movie_renderer_poll();
/*0062ad50*/ extern undefined4 g_app_62ad50;
/*0076e89c*/ extern undefined4 g_d3dx_76e89c;
/*004a2d20*/ extern int rw_set_value_at_root_plus_947144();
/*004a34f0*/ extern int app_get_app_76e8a4();
/*004a37f0*/ extern int movie_renderer_poll();
/*0062ad50*/ extern undefined4 g_app_62ad50;
/*0076e89c*/ extern undefined4 g_d3dx_76e89c;
/* movie_poll_and_set_surface */

/* "movie_renderer_poll and sets the current surface via app_fn_4a2d20" */

undefined4 __cdecl movie_poll_and_set_surface(undefined4 unused_1,undefined4 unused_2)
{
  undefined4 raster;
  
  movie_renderer_poll();
  raster = app_get_app_76e8a4((&g_d3dx_76e89c)[g_app_62ad50]);
  rw_set_value_at_root_plus_947144(raster);
  return 1;
}
