#include "mfl_tier2.h"
/*00432da0*/ extern char *__cdecl ui_camera_create(RwInt32 p0, RwInt32 p1, int p2);
/*00434a40*/ extern int ui_camera_update_view_rect();
/*004d1bd0*/ extern struct RwCamera * RwCameraSetNearClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*0069d69c*/ extern undefined4 g_ui_69d69c;
#define CAMERA_NEAR_CLIP_PLANE (0.5) /* near clip distance passed to RwCameraSetNearClipPlane */
#define CAMERA_FAR_CLIP_PLANE (180.0) /* far clip distance passed to RwCameraSetFarClipPlane */
/* ui_camera_create_secondary */
/* Creates the secondary UI camera into g_ui_69d69c (inferred: the second UI camera, next to the primary one),
   sets its near/far clip planes, then refreshes the UI view rect. Takes no parameters, returns nothing. */
void ui_camera_create_secondary(void)
{
  g_ui_69d69c = (RwCamera *)ui_camera_create(0,0,1);
  RwCameraSetNearClipPlane(g_ui_69d69c,CAMERA_NEAR_CLIP_PLANE);
  RwCameraSetFarClipPlane(g_ui_69d69c,CAMERA_FAR_CLIP_PLANE);
  ui_camera_update_view_rect();
  return;
}
