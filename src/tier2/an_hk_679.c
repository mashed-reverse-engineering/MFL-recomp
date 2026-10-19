#include "mfl_tier2.h"
/*00402830*/ extern undefined __cdecl cup_scene_render_with_overlay(undefined4 p0);
/*00410090*/ extern int race_get_course_id();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042caa0*/ extern int ui_draw_pulsing_texture(int param_2, int param_3, float param_4);
/*0042cdd0*/ extern int ui_draw_texture_640_default();
/*00434580*/ extern int __cdecl career_row_title_string_id(undefined4 p0);
/*00434bb0*/ extern int get_event_type();
/*0043cda0*/ extern int menu_texture_dict_load();
/*004408c0*/ extern int menu_draw_event_descriptions();
/*00442580*/ extern int menu_draw_event_icons();
/*00475730*/ extern int get_viewport_camera();
/*004824a0*/ extern int draw_rect();
/*004827a0*/ extern int gfx_draw_rect_2d_flat_a();
/*00482920*/ extern int gfx_draw_rect_2d_flat_b();
/*00482aa0*/ extern int gfx_draw_rect_2d_gradient_a();
/*00483d90*/ extern int build_info_display_draw_wave_grid();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005de8c4*/ extern float k_f_0_016666668;
/*0069ba28*/ extern undefined4 g_build_info_69ba28;
/*0076e7f0*/ extern undefined4 g_hud_76e7f0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b4*/ extern undefined4 g_hud_7ef7b4;
/*007ef7f8*/ extern float g_menu_wave_time;

#define BUILD_INFO_COLOUR_WHITE (0xffffffff)
#define EVENT_TYPE_NO_COURSE_OVERLAY (0x11) /* inferred: event where the course scene is skipped and text is dimmed */
#define FRAME_LINE_WIDTH (0x3f800000) /* 1.0f: width passed to draw_rect for the border lines */
#define TEXT_FACTOR_FULL (0x3f800000) /* 1.0f: last ui_rt2d_text_draw arg (alpha or scale, inferred) */
#define TEXT_FACTOR_DIMMED (0x3f333333) /* 0.7f: same arg, used during EVENT_TYPE_NO_COURSE_OVERLAY */
#define TEXT_FACTOR_SMALL (0x3f4ccccd) /* 0.8f: same arg, for the lower text lines */

/* build_info_display_draw_frame */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws the build info/loading screen: backdrop, gradients, borders and the 640x480 frame lines"
    */

/*
 * Draws one frame of the build info / loading screen in 640x480 layout.
 * Bumps the frame counter g_build_info_69ba28 and advances g_menu_wave_time.
 * The scene is drawn between RwCameraBeginUpdate and RwCameraEndUpdate, but the
 * camera is only cleared and given a 0.8 x 0.8 view window before that.
 * Finally the raster is shown and g_hud_7ef7b4 is reset to 0.
 * Parameters: unused_arg_1 and unused_arg_2 are not read by this function.
 * Returns nothing.
 */
void __cdecl build_info_display_draw_frame(char *unused_arg_1,char *unused_arg_2)

{
  undefined4 scratch_handle;
  RwCamera *viewport_camera;
  undefined4 scratch_value;
  int event_type_or_result;
  undefined4 title_string_id;
  undefined4 text_x;
  undefined4 text_y;
  RwRGBA *colour;
  void *window_handle;
  RwV2d *viewWindow;
  RwInt32 clear_mode;
  RwUInt32 show_flags;
  /* stack-passed rect arguments for the draw helpers (inferred); uStack_c/10/14/18 are set before each call */
  struct {
    undefined4 uStack_24;
    RwRGBA local_20;
    undefined4 local_1c;
    undefined4 uStack_18;
    undefined4 uStack_14;
    undefined4 uStack_10;
    undefined4 uStack_c;
    RwV2d local_8;
  } frame;

  /* frame counter */
  g_build_info_69ba28 = g_build_info_69ba28 + 1;
  frame.local_20.red = '\0';
  frame.local_20.green = '\0';
  frame.local_20.blue = '\0';
  frame.local_20.alpha = 0xff;
  frame.local_1c = BUILD_INFO_COLOUR_WHITE;
  scratch_handle = menu_texture_dict_load(0);
  menu_texture_dict_load(1);
  /* 0.8 x 0.8 view window */
  viewWindow = &frame.local_8;
  frame.local_8.x = 0.8;
  frame.local_8.y = 0.8;
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  RwCameraSetViewWindow(viewport_camera,viewWindow);
  /* clear image and Z (rwCAMERACLEARIMAGE | rwCAMERACLEARZ) to black */
  clear_mode = (rwCAMERACLEARIMAGE | rwCAMERACLEARZ);
  colour = &frame.local_20;
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  RwCameraClear(viewport_camera,colour,clear_mode);
  scratch_value = get_viewport_camera(0);
  event_type_or_result = RwCameraBeginUpdate(scratch_value);
  /* everything below is drawn only if the camera update started */
  if (event_type_or_result != 0) {
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    ui_draw_texture_640_default(scratch_handle,0,0x42800000,0x43b00000,0x43b00000,0);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    /* two gradient panels: x 0..256 and x 256..352 (inferred), both 480 high */
    frame.uStack_c = 0x43f00000;
    frame.uStack_10 = 0x43800000;
    frame.uStack_18 = 0;
    frame.uStack_14 = 0;
    gfx_draw_rect_2d_gradient_a(0,0,0x43800000,0x43f00000,0x80000000,0xc0000000);
    frame.uStack_10 = 0x42c00000;
    frame.uStack_18 = 0x43800000;
    gfx_draw_rect_2d_gradient_a(0x43800000,0,0x42c00000,0x43f00000,0xc0000000,0xff000000);
    /* course scene overlay, skipped for EVENT_TYPE_NO_COURSE_OVERLAY */
    event_type_or_result = get_event_type();
    if (event_type_or_result != EVENT_TYPE_NO_COURSE_OVERLAY) {
      scratch_handle = race_get_course_id();
      cup_scene_render_with_overlay(scratch_handle);
    }
    if (g_hud_76e7f0 != 0) {
      ui_draw_texture_640_default(g_hud_76e7f0,0x42800000,0x42700000,0x43480000,0x42c80000,0);
    }
    /* wave grid: 512 x 512 region (inferred) */
    frame.uStack_c = 0x44000000;
    frame.uStack_10 = 0x44000000;
    frame.uStack_14 = 0;
    frame.uStack_18 = 0;
    build_info_display_draw_wave_grid(0,0,0x44000000,0x44000000,0,0,0x40840000,0,0x407e0000,0,0xff);
    /* translucent flat bars along the top (x 0..640, y 0..64, inferred) and bottom (y 416..480) */
    frame.uStack_c = 0x42800000;
    frame.uStack_10 = 0x44200000;
    frame.uStack_14 = 0;
    frame.uStack_18 = 0;
    gfx_draw_rect_2d_flat_a(0,0,0x44200000,0x42800000,0xa0000000);
    frame.uStack_c = 0x42800000;
    frame.uStack_14 = 0x43d00000;
    frame.uStack_18 = 0;
    gfx_draw_rect_2d_flat_b(0,0x43d00000,0x44200000,0x42800000,0xa0000000);
    /* 1-pixel border lines around the frame */
    frame.uStack_c = 0x3f800000;
    frame.uStack_10 = 0x44200000;
    frame.uStack_18 = 0;
    draw_rect(0,0x43d00000,0x44200000,FRAME_LINE_WIDTH,BUILD_INFO_COLOUR_WHITE);
    frame.uStack_14 = 0x42800000;
    draw_rect(0,0x42800000,frame.uStack_10,FRAME_LINE_WIDTH,BUILD_INFO_COLOUR_WHITE);
    frame.uStack_18 = 0x44100000;
    frame.uStack_14 = 0;
    frame.uStack_10 = 0x3f800000;
    frame.uStack_c = 0x43848000;
    draw_rect(0x44100000,0,FRAME_LINE_WIDTH,0x43848000,BUILD_INFO_COLOUR_WHITE);
    frame.uStack_c = 0x3f800000;
    frame.uStack_10 = 0x43800000;
    frame.uStack_14 = 0x43848000;
    frame.uStack_18 = 0x43a00000;
    draw_rect(0x43a00000,0x43848000,0x43800000,FRAME_LINE_WIDTH,BUILD_INFO_COLOUR_WHITE);
    frame.uStack_10 = 0x44200000;
    frame.uStack_14 = 0x43180000;
    frame.uStack_18 = 0;
    draw_rect(0,0x43180000,0x44200000,FRAME_LINE_WIDTH,BUILD_INFO_COLOUR_WHITE);
    if (g_hud_7ef7b4 == 0) {
      ui_draw_pulsing_texture(-0xd8,0x24,0.4);
    }
    frame.uStack_c = 0x43848000;
    frame.uStack_10 = 0x3f800000;
    frame.uStack_14 = 0;
    frame.uStack_18 = 0x43a00000;
    draw_rect(0x43a00000,0,FRAME_LINE_WIDTH,0x43848000,BUILD_INFO_COLOUR_WHITE);
    /* title line: string from career_row_title_string_id(-1), at x = 310.0 (0x439b0000), y = 168.0 (0x43280000) (x/y inferred) */
    frame.uStack_24 = BUILD_INFO_COLOUR_WHITE;
    event_type_or_result = get_event_type();
    scratch_handle = 1;
    if (event_type_or_result == EVENT_TYPE_NO_COURSE_OVERLAY) {
      scratch_value = TEXT_FACTOR_DIMMED;
    }
    else {
      scratch_value = TEXT_FACTOR_FULL;
    }
    text_y = 0x43280000;
    text_x = 0x439b0000;
    title_string_id = career_row_title_string_id(0xffffffff);
    ui_rt2d_text_draw(title_string_id,text_x,text_y,frame.uStack_24,scratch_value,scratch_handle);
    g_menu_wave_time = g_menu_wave_time + k_f_0_016666668;
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    /* hint text at (320.0, 432.0) (0x43a00000, 0x43d80000), only while g_hud_7ef7b4 is 0 */
    if (g_hud_7ef7b4 == 0) {
      ui_rt2d_text_draw(0x227,0x43a00000,0x43d80000,BUILD_INFO_COLOUR_WHITE,TEXT_FACTOR_SMALL,2);
    }
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
    /* event descriptions and icons on top, with depth test/write back on */
    menu_draw_event_descriptions(1);
    menu_draw_event_icons(1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    /* extra text at (320.0, 432.0), only when g_hud_7ef7b4 is set and bit 0x20 of g_build_info_69ba28 is set (inferred) */
    if ((g_hud_7ef7b4 != 0) && ((g_build_info_69ba28 & 0x20) != 0)) {
      ui_rt2d_text_draw(0x1e9,0x43a00000,0x43d80000,frame.local_1c,TEXT_FACTOR_SMALL,2);
    }
    scratch_handle = get_viewport_camera(0);
    RwCameraEndUpdate(scratch_handle);
  }
  /* present the frame */
  show_flags = rwRASTERFLIPWAITVSYNC;
  window_handle = (void *)0x0;
  viewport_camera = (RwCamera *)get_viewport_camera(0);
  RwCameraShowRaster(viewport_camera,window_handle,show_flags);
  g_hud_7ef7b4 = 0;
  return;
}


