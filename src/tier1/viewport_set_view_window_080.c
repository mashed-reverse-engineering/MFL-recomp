#include "mfl_tier2.h"
/*00403890*/ extern int hud_flash_text();
/*0042cdd0*/ extern int ui_draw_texture_640_default();
/*00475730*/ extern char *get_viewport_camera();
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;
/*007d2128*/ extern char *RwEngineInstance;
/* viewport_set_view_window_080 */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "camera view window 0.8 x 0.8 for the viewport (saves the old window)" */
void viewport_set_view_window_080(void)
{
  RwCamera *camera;
  int began;
  union {
    RwV2d window[2];       /* [0] the 0.8 window, [1] the saved old window */
    char gs_buffer[16];    /* only makes /GS put the stack cookie here, as in the original */
  } vwu;
  RwV2d *vw = vwu.window;
  
  vw[0].x = 0.8;
  vw[0].y = 0.8;
  camera = (RwCamera *)get_viewport_camera(0);
  RwCameraEndUpdate(camera);
  vw[1].x = RwCamera_viewWindow(camera).x;
  vw[1].y = RwCamera_viewWindow(camera).y;
  RwCameraSetViewWindow(camera,&vw[0]);
  began = RwCameraBeginUpdate(camera);
  if (began != 0) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    hud_flash_text();
    if (g_hud_76e7f0 != 0) {
      ui_draw_texture_640_default(g_hud_76e7f0,0x42480000,0x41f00000,0x43700000,0x42f00000,0);
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwCameraEndUpdate(camera);
  }
  RwCameraSetViewWindow(camera,&vw[1]);
  RwCameraBeginUpdate(camera);
  return;
}
