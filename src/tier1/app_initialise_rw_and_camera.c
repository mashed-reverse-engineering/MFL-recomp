#include "mfl_tier2.h"

enum {
  RW_ENGINE_INIT_PARAM_UNKNOWN = 0,
  RW_ENGINE_INIT_FAILURE = 0,
  APP_INIT_FAILURE = 0,
  APP_INIT_SUCCESS = 1
};

/*0042cbf0*/ extern int screen_clear_black();
/*004a14f0*/ extern int video_init_main_camera();
/*004a2b50*/ extern int __cdecl rw_engine_init(undefined4 p0);


/* app_initialise_rw_and_camera */

/* "rw_engine_init, video_init_main_camera, screen_clear_black" */

/* Initialize RenderWare engine, setup main camera, and clear screen to black. */
undefined4 app_initialise_rw_and_camera(void)
{
  int rw_engine_init_success;
  
  /* Initialize RenderWare engine; return failure on error */
  rw_engine_init_success = rw_engine_init(RW_ENGINE_INIT_PARAM_UNKNOWN);
  if (rw_engine_init_success == RW_ENGINE_INIT_FAILURE) {
    return APP_INIT_FAILURE;
  }
  /* Initialize main camera and clear initial display buffer */
  video_init_main_camera();
  screen_clear_black();
  return APP_INIT_SUCCESS;
}

