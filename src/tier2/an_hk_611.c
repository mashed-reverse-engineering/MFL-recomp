#include "mfl_tier2.h"
/*00403930*/ extern int hud_flash_text_overlay();
/*00404c90*/ extern int viewport_set_view_window_game_mode_9();
/*0040f810*/ extern undefined __cdecl camera_save(struct RwCamera *p0);
/*0040f900*/ extern undefined __cdecl camera_restore(struct RwCamera *p0);
/*0040f940*/ extern int race_phase_5_6_render_by_event();
/*0040f9c0*/ extern int hud_render_by_event_and_mode();
/*00412e00*/ extern int race_render_viewport();
/*0042a3c0*/ extern int course_race_start();
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *p0);
/*0042ab60*/ extern undefined __cdecl course_camera_remove_from_collision_world(struct RwCamera *p0);
/*0042fbf0*/ extern int menu_get_69d1e4();
/*00430ad0*/ extern int ui_state_is_0();
/*00430af0*/ extern int ui_state_is_5();
/*00430b30*/ extern int ui_get_menu_69d1ec();
/*00431350*/ extern int menu_draw_training_prompt();
/*00432c50*/ extern int ui_get_menu_69cc48();
/*00434a20*/ extern int get_second_camera();
/*00434a40*/ extern int ui_camera_update_view_rect();
/*00434bb0*/ extern int get_event_type();
/*00436550*/ extern int options_menu_draw();
/*00439d40*/ extern int message_box_draw();
/*00443f00*/ extern int frontend_draw();
/*00475730*/ extern int get_viewport_camera();
/*00475750*/ extern int car_get_car_6c2944();
/*0047fe30*/ extern int hud_labels_render_all();
/*00480e50*/ extern int gfx_frame_counters_rotate();
/*004a17e0*/ extern int framerate_sample_accumulate();
/*004a2210*/ extern int camera_set_default_view_window();
/*004a41f0*/ extern int timer_ticks();
/*004a8860*/ extern int app_get_app_7712f4();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1d70*/ extern struct RwCamera * RwCameraShowRaster(struct RwCamera *p0, void *p1, RwUInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*00563030*/ extern int Rt2dDeviceSetCamera();
/*005dd61c*/ extern float k_f_0_001;
/*0062ad2c*/ extern undefined g_game_62ad2c;
/*0076e7f4*/ extern undefined4 g_app_state;
/*0076e814*/ extern float g_app_state_76e814;
/*0076e818*/ extern undefined4 g_app_state_76e818;
/*0076e81c*/ extern undefined4 g_app_state_76e81c;
/*0076e824*/ extern undefined4 g_app_state_76e824;
/*0076e828*/ extern undefined4 g_app_state_76e828;
/*0076e82c*/ extern undefined4 g_app_state_76e82c;
/*0076e830*/ extern undefined4 g_app_state_76e830;
/*0076e838*/ extern undefined4 g_app_state_76e838;
/*0076e83c*/ extern undefined4 g_app_state_76e83c;
/*0076e854*/ extern undefined4 g_app_state_76e854;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*00883ea0*/ extern undefined g_app_state_883ea0[0x1000];
/*00885820*/ extern char g_app_885820[];

#define TIMER_TICKS_PER_STEP (3000) /* (inferred) timer ticks per step */
#define FPS_WINDOW_STEPS (1000) /* (inferred) length of the frame-count window, in steps */
#define MENU_ID_THRESHOLD (199) /* (inferred) menu id above which the overlay flag is set */
#define CLEAR_ALPHA_OPAQUE (0xff)
#define EVENT_TRAINING_PROMPT (0x11) /* (inferred) from menu_draw_training_prompt */
#define FRAMERATE_SAMPLE_STRIDE (0x44) /* bytes per framerate sample slot */
/* app_state_menu_render */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "camera clear, menu state render" */

/*
 * Per-frame render for the menu / race app state (g_app_state). Takes no parameters.
 * Updates the frame timing and the frames-per-window counter, clears the viewport
 * camera, draws the screen for the current state, flips the raster, then accumulates
 * the framerate samples. Always returns 1.
 */
undefined4 app_state_menu_render(void)

{
  int stopwatch_ticks;
  uint timer_now;
  int last_result;
  RwCamera *cam;
  int check_result;
  undefined4 viewport_cam;
  RwRGBA *clear_color_ptr;
  undefined *sample_ptr;
  void *raster;
  RwInt32 clear_mode;
  RwV2d *view_window;
  RwUInt32 show_flags;
  struct {
    RwRGBA clear_color;
    int frame_delta;
    RwV2d view_size;
  } frame;

  frame.clear_color.red = '\0';
  frame.clear_color.green = '\0';
  frame.clear_color.blue = '\0';
  frame.clear_color.alpha = CLEAR_ALPHA_OPAQUE;
  clear_color_ptr = (RwRGBA *)&g_game_62ad2c;
  /* Frame timing: current time in steps, and this frame's step delta. stopwatch_ticks is
     reused below as the start mark of the two render timers. */
  stopwatch_ticks = timer_ticks();
  timer_now = timer_ticks();
  g_app_state_76e82c = timer_now / TIMER_TICKS_PER_STEP;
  if (g_app_state_76e818 == 0) {
    g_app_state_76e824 = g_app_state_76e82c;
  }
  frame.frame_delta = g_app_state_76e82c - g_app_state_76e824;
  g_app_state_76e814 = (float)frame.frame_delta * k_f_0_001;
  /* Once per FPS_WINDOW_STEPS: latch the frames counted in the last window into
     g_app_state_76e81c (inferred: fps) and restart the count. */
  if ((int)(g_app_state_76e828 + FPS_WINDOW_STEPS) < (int)g_app_state_76e82c) {
    g_app_state_76e81c = g_app_state_76e830;
    g_app_state_76e830 = 0;
    g_app_state_76e828 = g_app_state_76e82c;
  }
  /* State 6: overlay flag is set while the menu id is above MENU_ID_THRESHOLD
     (inferred), and the flag also forces the menu clear colour below. */
  if (g_app_state == 6) {
    last_result = menu_get_69d1e4();
    g_app_state_76e854 = (uint)(MENU_ID_THRESHOLD < last_result);
  }
  if (((g_app_state == 2) || (g_app_state == 1)) || (g_app_state_76e854 != 0)) {
    clear_color_ptr = &frame.clear_color;
  }
  /* Clear the viewport camera (image and Z). Colour: opaque black for the menu states,
     otherwise the global colour at g_game_62ad2c. */
  clear_mode = rwCAMERACLEARIMAGE | rwCAMERACLEARZ;
  cam = (RwCamera *)get_viewport_camera(0xffffffff);
  RwCameraClear(cam,clear_color_ptr,clear_mode);
  /* Draw the screen for the current app state. */
  switch(g_app_state) {
  /* State 1: frontend menu. Draws into the viewport camera; when the menu id is 3 and
     the view rect is ready, also renders the second camera for race start. */
  case 1:
    last_result = ui_get_menu_69d1ec();
    if ((last_result == 1) &&
       ((check_result = ui_state_is_5(), check_result != 0 || (check_result = ui_state_is_0(), check_result != 0)))) {
      viewport_cam = get_viewport_camera(0xffffffff);
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result == 0) break;
      viewport_cam = 0xffffffff;
    }
    else {
      view_window = &frame.view_size;
      frame.view_size.x = 0.8;
      frame.view_size.y = 0.8;
      cam = (RwCamera *)get_viewport_camera(0xffffffff);
      RwCameraSetViewWindow(cam,view_window);
      camera_set_default_view_window();
      viewport_cam = get_viewport_camera(0xffffffff);
      check_result = RwCameraBeginUpdate(viewport_cam);
      if (check_result != 0) {
        frontend_draw();
        if (last_result == 0x21) {
          hud_flash_text_overlay();
        }
        viewport_cam = get_viewport_camera(0xffffffff);
        RwCameraEndUpdate(viewport_cam);
      }
      if (((last_result == 3) && (check_result = ui_camera_update_view_rect(), check_result != 0)) &&
         (check_result = ui_get_menu_69cc48(), check_result < 3)) {
        course_race_start();
        clear_color_ptr = (RwRGBA *)&g_game_62ad2c;
        cam = (RwCamera *)get_second_camera();
        RwCameraClear(cam,clear_color_ptr,last_result);
        cam = (RwCamera *)get_second_camera();
        course_camera_add_to_collision_world(cam);
        viewport_cam = get_second_camera();
        last_result = RwCameraBeginUpdate(viewport_cam);
        if (last_result != 0) {
          race_render_viewport();
          viewport_cam = get_second_camera();
          RwCameraEndUpdate(viewport_cam);
        }
        cam = (RwCamera *)get_second_camera();
        course_camera_remove_from_collision_world(cam);
      }
      viewport_cam = get_viewport_camera(0xffffffff);
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result == 0) break;
      message_box_draw();
      viewport_cam = 0xffffffff;
    }
    goto LAB_004a26d6;
  case 2:
    g_app_state_76e854 = 4;
    break;
  /* States 3 and 7: in-race render (course camera, race viewport, HUD, training prompt). */
  case 3:
  case 7:
    if (g_app_state_76e854 == 0) {
      course_race_start();
      cam = (RwCamera *)get_viewport_camera(0);
      course_camera_add_to_collision_world(cam);
      viewport_cam = get_viewport_camera(0);
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result != 0) {
        race_render_viewport();
        race_phase_5_6_render_by_event();
        viewport_set_view_window_game_mode_9();
        viewport_cam = get_viewport_camera(0);
        RwCameraEndUpdate(viewport_cam);
      }
      cam = (RwCamera *)car_get_car_6c2944();
      camera_save(cam);
      viewport_cam = car_get_car_6c2944();
      Rt2dDeviceSetCamera(viewport_cam);
      viewport_cam = car_get_car_6c2944();
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result != 0) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
        hud_render_by_event_and_mode();
        hud_labels_render_all();
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
        viewport_cam = car_get_car_6c2944();
        RwCameraEndUpdate(viewport_cam);
      }
      camera_set_default_view_window();
      viewport_cam = get_viewport_camera(0);
      Rt2dDeviceSetCamera(viewport_cam);
      cam = (RwCamera *)car_get_car_6c2944();
      camera_restore(cam);
      cam = (RwCamera *)get_viewport_camera(0);
      course_camera_remove_from_collision_world(cam);
      last_result = get_event_type();
      if (last_result == EVENT_TRAINING_PROMPT) {
        viewport_cam = get_viewport_camera(0);
        last_result = RwCameraBeginUpdate(viewport_cam);
        if (last_result != 0) {
          menu_draw_training_prompt();
          viewport_cam = get_viewport_camera(0);
          RwCameraEndUpdate(viewport_cam);
        }
      }
      if (g_app_state != 7) break;
    }
    else if (g_app_state != 7) {
      g_app_state_76e854 = g_app_state_76e854 - 1;
      break;
    }
    viewport_cam = get_viewport_camera(0);
    last_result = RwCameraBeginUpdate(viewport_cam);
    if (last_result == 0) break;
    goto LAB_004a26d0;
  /* State 4: frontend page, plus the options menu when the menu id is 0x20. */
  case 4:
    camera_set_default_view_window();
    viewport_cam = get_viewport_camera(0);
    last_result = RwCameraBeginUpdate(viewport_cam);
    if (last_result == 0) break;
    frontend_draw();
    last_result = ui_get_menu_69d1ec();
    if (last_result == 0x20) {
      options_menu_draw();
    }
LAB_004a26d0:
    message_box_draw();
    viewport_cam = 0;
LAB_004a26d6:
    viewport_cam = get_viewport_camera(viewport_cam);
    RwCameraEndUpdate(viewport_cam);
    break;
  /* States 5 and 6: in-race render with the frontend drawn on top (inferred). */
  case 5:
  case 6:
    if (g_app_state_76e854 == 0) {
      cam = (RwCamera *)get_viewport_camera(0);
      course_camera_add_to_collision_world(cam);
      viewport_cam = get_viewport_camera(0);
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result != 0) {
        race_render_viewport();
        viewport_cam = get_viewport_camera(0);
        RwCameraEndUpdate(viewport_cam);
      }
      cam = (RwCamera *)car_get_car_6c2944();
      camera_save(cam);
      viewport_cam = car_get_car_6c2944();
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result != 0) {
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
        hud_render_by_event_and_mode();
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
        viewport_cam = car_get_car_6c2944();
        RwCameraEndUpdate(viewport_cam);
      }
      cam = (RwCamera *)car_get_car_6c2944();
      camera_restore(cam);
      viewport_cam = get_viewport_camera(0);
      last_result = RwCameraBeginUpdate(viewport_cam);
      if (last_result != 0) {
        frontend_draw();
        viewport_cam = get_viewport_camera(0);
        RwCameraEndUpdate(viewport_cam);
      }
      cam = (RwCamera *)get_viewport_camera(0);
      course_camera_remove_from_collision_world(cam);
    }
  }
  g_app_state_76e838 = timer_ticks();
  g_app_state_76e838 = g_app_state_76e838 - stopwatch_ticks;
  stopwatch_ticks = timer_ticks();
  gfx_frame_counters_rotate();
  app_get_app_7712f4();
  /* Flip the raster to the screen, waiting for vsync. */
  show_flags = rwRASTERFLIPWAITVSYNC;
  raster = (void *)0x0;
  cam = (RwCamera *)get_viewport_camera(0);
  RwCameraShowRaster(cam,raster,show_flags);
  g_app_state_76e83c = timer_ticks();
  g_app_state_76e83c = g_app_state_76e83c - stopwatch_ticks;
  /* Accumulate every framerate sample slot, FRAMERATE_SAMPLE_STRIDE bytes apart,
     up to g_app_885820. */
  sample_ptr = g_app_state_883ea0;
  do {
    framerate_sample_accumulate(sample_ptr);
    sample_ptr = sample_ptr + FRAMERATE_SAMPLE_STRIDE;
  } while ((LONG_PTR)sample_ptr < ((LONG_PTR)&g_app_885820));
  /* Advance the frame counters for the next frame. */
  g_app_state_76e818 = g_app_state_76e818 + 1;
  g_app_state_76e824 = g_app_state_76e82c;
  g_app_state_76e830 = g_app_state_76e830 + 1;
  return 1;
}


