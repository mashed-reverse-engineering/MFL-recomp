#include "mfl_tier2.h"
/*00432da0*/ extern int __cdecl ui_camera_create(RwInt32 p0, RwInt32 p1, int p2);
/*00432e20*/ extern undefined ui_camera_set_viewport(void);
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*0069d698*/ extern undefined4 g_ui_69d698;

#define UI_CAMERA_NEAR_CLIP (0.5)
#define UI_CAMERA_FAR_CLIP (180.0)

/* ui_camera_create_main */

/* "creates g_ui_69d698 camera with near 0.5 / far 180" */

/* Creates the UI camera and stores it in g_ui_69d698, sets its near and far clip planes
 * (UI_CAMERA_NEAR_CLIP / UI_CAMERA_FAR_CLIP), then sets the viewport.
 * Takes no parameters. Returns 1 (inferred: success). */

undefined4 ui_camera_create_main(void)

{
  
  g_ui_69d698 = (undefined4)((RwCamera *)ui_camera_create(0,0,1));
  RwCameraSetNearClipPlane((struct RwCamera *)g_ui_69d698,UI_CAMERA_NEAR_CLIP);
  RwCameraSetFarClipPlane((struct RwCamera *)g_ui_69d698,UI_CAMERA_FAR_CLIP);
  ui_camera_set_viewport();
  return 1;
}


