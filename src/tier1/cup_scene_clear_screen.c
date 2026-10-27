#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/* cup_scene_clear_screen */
/* "RwCameraClear black for each of the 4 viewports, then RwCameraShowRaster" */
void __cdecl cup_scene_clear_screen(int pass_count,char *unused_2,char *unused_3,char *unused_4,char *unused_5, char *unused_6,char *unused_7,char *unused_8,char *unused_9)
{
  int began;
  RwRGBA black;
  
  black.red = '\0';
  black.green = '\0';
  black.blue = '\0';
  black.alpha = 0xff;
  if (0 < pass_count) {
    do {
      RwCameraClear((RwCamera *)get_viewport_camera(0),&black,(rwCAMERACLEARIMAGE | rwCAMERACLEARZ));
      began = RwCameraBeginUpdate(get_viewport_camera(0));
      if (began != 0) {
        RwCameraEndUpdate(get_viewport_camera(0));
      }
      RwCameraShowRaster((RwCamera *)get_viewport_camera(0),(void *)0,rwRASTERFLIPWAITVSYNC);
      pass_count = pass_count + -1;
    } while (pass_count != 0);
  }
  return;
}
