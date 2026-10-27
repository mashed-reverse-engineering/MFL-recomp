#include "mfl_tier2.h"
extern char s_Toast_Toast_Toast_Toast_Toast_To_006099b0[];
/*0040c2a0*/ extern int nop_a();
/*0042c2d0*/ extern int course_get_dat_69ba20();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c960*/ extern float ui_rt2d_text_width();
/*0042caa0*/ extern int ui_draw_pulsing_texture(int param_2, int param_3, float param_4);
/*0042cf10*/ extern int build_info_draw_page_0();
/*00475730*/ extern int get_viewport_camera();
/*004a41f0*/ extern int timer_ticks();
/*004b2a60*/ extern int sprintf();
/*004b3470*/ extern char * __cdecl _strrchr(char *p0, int p1);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*005de894*/ extern undefined g_build_info_5de894;
/*0069ba2c*/ extern undefined4 g_build_info_69ba2c;
/*0069bb30*/ extern undefined4 g_build_info_69bb30;
/*0069bb48*/ extern undefined4 g_build_info_69bb48;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define PULSE_TEXTURE_X (0x20) /* inferred: first ui_draw_pulsing_texture argument */
#define PULSE_TEXTURE_Y (-0x20) /* inferred: second ui_draw_pulsing_texture argument */
#define TOAST_SEARCH_CHAR ('T') /* character strrchr looks for in the toast string */
#define BUILD_INFO_TIMEOUT_TICKS (24000000) /* inferred: timer ticks before g_build_info_69bb30 is set */
#define BUILD_INFO_TEXT_ID (0x227) /* inferred: text id passed to ui_rt2d_text_draw */
#define BUILD_INFO_TEXT_X (0x44110000) /* float 580.0f (inferred: x position) */
#define BUILD_INFO_TEXT_Y (0x430e0000) /* float 142.0f (inferred: y position) */
#define BUILD_INFO_TEXT_SCALE (0x3f1eb852) /* float 0.62f (inferred) */
/* build_info_draw_page_0_0042d0a0 */
/* compile: /GS */
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* duplicate name: was build_info_draw_page_0 (first address 0042cf10) */
/*
 * Draws one frame of the build-info overlay into the viewport camera and shows it (inferred from the call sequence).
 * Both paths clear the camera to opaque black. If course_get_dat_69ba20() and nop_a() are both nonzero, the full page
 * is drawn (page 0 contents, pulsing texture, text BUILD_INFO_TEXT_ID); otherwise only the pulsing texture is drawn.
 * Parameters: unused_param_1 and unused_param_2 are not read by the body (inferred).
 * Returns nothing. `status` holds callee return values and, in the full path, the current timer value (inferred).
 */
void __cdecl build_info_draw_page_0_0042d0a0(char *unused_param_1,char *unused_param_2)
{
  int status;
  RwCamera *viewport_cam;
  undefined4 camera_handle;
  char *last_toast_t;
  RwRGBA *clear_color;
  void *raster;
  RwInt32 clear_flags;
  RwUInt32 show_flags;
  union {
    char gs_buf[136];
    struct {
      RwRGBA color_0;
      RwRGBA color_1;
      undefined1 text_buf [128];
    } m;
  } frame;

  /* Full page only when both gate calls return nonzero; otherwise fall through to the plain page below. */
  status = course_get_dat_69ba20();
  if (status != 0) {
    status = nop_a();
    if (status != 0) {
      /* Text colour is white (color_0), clear colour is black (color_1), both opaque. */
      frame.m.color_0.red = 0xff;
      frame.m.color_0.green = 0xff;
      frame.m.color_0.blue = 0xff;
      frame.m.color_0.alpha = 0xff;
      frame.m.color_1.alpha = 0xff;
      frame.m.color_1.red = '\0';
      frame.m.color_1.green = '\0';
      frame.m.color_1.blue = '\0';
      /* First call latches the start time in g_build_info_69ba2c (g_build_info_69bb48 marks it as latched). */
      if (g_build_info_69bb48 == 0) {
        g_build_info_69bb48 = 1;
        g_build_info_69ba2c = timer_ticks();
      }
      /* Once the page has been up longer than BUILD_INFO_TIMEOUT_TICKS, set the timeout flag g_build_info_69bb30 (inferred meaning). */
      status = timer_ticks();
      if (BUILD_INFO_TIMEOUT_TICKS < (uint)(status - g_build_info_69ba2c)) {
        g_build_info_69bb30 = 1;
      }
      clear_flags = (rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
      clear_color = &frame.m.color_1;
      RwCameraClear((RwCamera *)get_viewport_camera(0),clear_color,clear_flags);
      /* Format the build date/time stamp into the local buffer (text_buf); nothing later in this function reads it (inferred). */
      sprintf(frame.m.text_buf,&g_build_info_5de894,"Jun 30 2005","12:38:08");
      camera_handle = get_viewport_camera(0);
      status = RwCameraBeginUpdate(camera_handle);
      /* Draw only if the camera began updating. */
      if (status != 0) {
        /* camera_handle is reused as the float 1.0f bit pattern for the width call. */
        camera_handle = 0x3f800000;
        ui_rt2d_text_width(_strrchr(s_Toast_Toast_Toast_Toast_Toast_To_006099b0,TOAST_SEARCH_CHAR),camera_handle);
        RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
        build_info_draw_page_0();
        ui_draw_pulsing_texture(PULSE_TEXTURE_X,PULSE_TEXTURE_Y,1.0);
        ui_rt2d_text_draw(BUILD_INFO_TEXT_ID,BUILD_INFO_TEXT_X,BUILD_INFO_TEXT_Y,frame.m.color_0,BUILD_INFO_TEXT_SCALE,1);
        camera_handle = get_viewport_camera(0);
        RwCameraEndUpdate(camera_handle);
      }
      goto LAB_0042d249;
    }
  }
  /* Plain page: clear to opaque black and draw only the pulsing texture. */
  clear_flags = (rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
  clear_color = &frame.m.color_0;
  frame.m.color_0.red = '\0';
  frame.m.color_0.green = '\0';
  frame.m.color_0.blue = '\0';
  frame.m.color_0.alpha = 0xff;
  RwCameraClear((RwCamera *)get_viewport_camera(0),clear_color,clear_flags);
  camera_handle = get_viewport_camera(0);
  status = RwCameraBeginUpdate(camera_handle);
  if (status != 0) {
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
    ui_draw_pulsing_texture(PULSE_TEXTURE_X,PULSE_TEXTURE_Y,1.0);
    camera_handle = get_viewport_camera(0);
    RwCameraEndUpdate(camera_handle);
  }
LAB_0042d249:
  show_flags = rwRASTERFLIPWAITVSYNC;
  raster = (void *)0x0;
  RwCameraShowRaster((RwCamera *)get_viewport_camera(0),raster,show_flags);
  return;
}
