#include "mfl_tier2.h"
/*004a3860*/ extern int movie_renderer_release();
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*0076e890*/ extern undefined4 g_app_76e890;
/*0076e8a4*/ extern undefined4 g_app_76e8a4;
/*0076e8ac*/ extern undefined4 *g_app_76e8ac;
/* movie_stop */

/* "stops the movie, releases the renderer and destroys the raster" */

typedef void (__stdcall *vtbl_fn_ii)(undefined4 *, int);

void __cdecl movie_stop(undefined4 param_1)

{
  (*(vtbl_fn_ii *)(*g_app_76e8ac + 0x1c))(g_app_76e8ac,0xffffec78);
  (*(vtbl_fn_ii *)(*g_app_76e8ac + 0x1c))(g_app_76e8ac,0xffffd8f0);
  movie_renderer_release();
  RwRasterDestroy((struct RwRaster *)g_app_76e8a4);
  g_app_76e8a4 = (undefined4)(RwRaster *)0x0;
  g_app_76e890 = 0;
  return;
}


