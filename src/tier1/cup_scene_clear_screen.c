#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/* cup_scene_clear_screen */
/* "RwCameraClear black for each of the 4 viewports, then RwCameraShowRaster" */
void __cdecl cup_scene_clear_screen(int param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8,char *param_9)
{
  int iVar2;
  RwRGBA local_4;
  
  local_4.red = '\0';
  local_4.green = '\0';
  local_4.blue = '\0';
  local_4.alpha = 0xff;
  if (0 < param_1) {
    do {
      RwCameraClear((RwCamera *)get_viewport_camera(0),&local_4,3);
      iVar2 = RwCameraBeginUpdate(get_viewport_camera(0));
      if (iVar2 != 0) {
        RwCameraEndUpdate(get_viewport_camera(0));
      }
      RwCameraShowRaster((RwCamera *)get_viewport_camera(0),(void *)0,1);
      param_1 = param_1 + -1;
    } while (param_1 != 0);
  }
  return;
}
