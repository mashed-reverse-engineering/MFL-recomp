#include "mfl_tier2.h"
/*00475730*/ extern char *get_viewport_camera();
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);


/* camera_set_default_view_window */

/* "view window 0.8 x 0.8 on the viewport camera -1" */

void camera_set_default_view_window(void)

{
  RwV2d local_8;
  
  local_8.x = 0.8;
  local_8.y = 0.8;
  RwCameraSetViewWindow((RwCamera *)get_viewport_camera(0xffffffff),&local_8);
  return;
}
