#include "mfl_tier2.h"
extern char s_Toast_Toast_Toast_Toast_Toast_To_006099b0[];
/*0042c2d0*/ extern int course_get_dat_69ba20();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c960*/ extern float ui_rt2d_text_width();
/*0042caa0*/ extern int ui_draw_pulsing_texture(int param_2, int param_3, float param_4);
/*0042cdd0*/ extern int ui_draw_texture_640_default();
/*00475730*/ extern int get_viewport_camera();
/*004b2a60*/ extern int sprintf();
/*004b3470*/ extern char * __cdecl _strrchr(char *p0, int p1);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*005de894*/ extern undefined g_build_info_5de894;
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;
#define ALPHA_OPAQUE (0xff)
#define CAMERA_CLEAR_IMAGE_AND_Z (3) /* rwCAMERACLEARIMAGE | rwCAMERACLEARZ */
#define TOAST_SEARCH_CHAR (0x54) /* 'T' */
/* build_info_draw_page_1 */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "second build info page" */

/* Draws the second build info overlay page on the viewport camera and presents it.
 * Parameters: unused_1 and unused_2 are never read (inferred: the page takes no arguments).
 * Returns nothing. Draws only when course_get_dat_69ba20() returns nonzero (inferred: course data ready).
 */
void __cdecl build_info_draw_page_1(char *unused_1,char *unused_2)

{
  int result;
  RwCamera *viewport_cam;
  undefined4 camera;
  char *toast_last_word;
  RwRGBA *clear_colour;
  void *device_window;
  RwInt32 clear_mode;
  RwUInt32 show_flags;
  union {
    char raw_bytes[132];
    struct {
      RwRGBA clear_rgba;
      undefined1 text_buf [128];
    } m;
  } frame;

  result = course_get_dat_69ba20();
  if (result != 0) {
    /* clear the viewport to opaque black */
    frame.m.clear_rgba.alpha = ALPHA_OPAQUE;
    clear_mode = CAMERA_CLEAR_IMAGE_AND_Z;
    clear_colour = &frame.m.clear_rgba;
    frame.m.clear_rgba.red = '\0';
    frame.m.clear_rgba.green = '\0';
    frame.m.clear_rgba.blue = '\0';
    viewport_cam = (RwCamera *)get_viewport_camera(0);
    RwCameraClear(viewport_cam,clear_colour,clear_mode);
    /* format the build date/time into text_buf (never drawn below, inferred) */
    sprintf(frame.m.text_buf,&g_build_info_5de894,"Jun 30 2005","12:38:08");
    camera = get_viewport_camera(0);
    result = RwCameraBeginUpdate(camera);
    /* draw the page between BeginUpdate and EndUpdate */
    if (result != 0) {
      /* camera is reused here as the 1.0f (0x3f800000) scale argument */
      camera = 0x3f800000;
      /* width of the string tail from its last 'T'; the result is not used (inferred: measurement only) */
      toast_last_word = _strrchr(s_Toast_Toast_Toast_Toast_Toast_To_006099b0,TOAST_SEARCH_CHAR);
      ui_rt2d_text_width(toast_last_word,camera);
      ui_draw_texture_640_default(g_hud_76e7f0,0x43a00000,0x43820000,0x43f00000,0x43700000,2);
      ui_draw_pulsing_texture(0x20,-0x20,1.0);
      ui_rt2d_text_draw(0x227,0x44110000,0x430c0000,0xffffffff,0x3f400000,1);
      camera = get_viewport_camera(0);
      RwCameraEndUpdate(camera);
    }
    /* present the frame; NULL device window = default target (inferred) */
    show_flags = 1;
    device_window = (void *)0x0;
    viewport_cam = (RwCamera *)get_viewport_camera(0);
    RwCameraShowRaster(viewport_cam,device_window,show_flags);
  }
  return;
}
