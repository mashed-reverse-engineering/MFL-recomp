#include "mfl_tier2.h"
/*0042caa0*/ extern int ui_draw_pulsing_texture(int param_2, int param_3, float param_4);
/*00475730*/ extern char *get_viewport_camera();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*0069bb44*/ extern float g_course_69bb44;
#define OPAQUE_ALPHA (0xff)
#define CAMERA_CLEAR_IMAGE_AND_Z (rwCAMERACLEARIMAGE | rwCAMERACLEARZ)
#define RASTER_FLIP_WAIT_VSYNC (rwRASTERFLIPWAITVSYNC)
#define HALF_PI (1.5707964f)
/* screen_clear_black */
/* "RwCameraClear of viewport 0 with opaque black" */
/* Clears the viewport 0 camera to opaque black, draws a pulsing texture over it while the
   camera update is open (inferred), then presents the raster. Takes no parameters, always returns 1. */
undefined4 screen_clear_black(void)
{
  RwRGBA clear_colour;

  clear_colour.red = '\0';
  clear_colour.green = '\0';
  clear_colour.blue = '\0';
  clear_colour.alpha = OPAQUE_ALPHA;
  RwCameraClear((RwCamera *)get_viewport_camera(0),&clear_colour,CAMERA_CLEAR_IMAGE_AND_Z);
  /* only draw the overlay when RwCameraBeginUpdate succeeded (non-zero);
     also sets g_course_69bb44 to pi/2 (purpose of the global unknown) */
  if (RwCameraBeginUpdate(get_viewport_camera(0)) != 0) {
    g_course_69bb44 = HALF_PI;
    ui_draw_pulsing_texture(0x20,-0x20,1.0);
    RwCameraEndUpdate(get_viewport_camera(0));
  }
  RwCameraShowRaster((RwCamera *)get_viewport_camera(0),(void *)0x0,RASTER_FLIP_WAIT_VSYNC);
  return 1;
}
