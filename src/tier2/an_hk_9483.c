#include "mfl_tier2.h"
/*00406cf0*/ extern int fx_objects_all_exploded();
/*0040f700*/ extern float intro_banner_get_x(void);
/*0040fdc0*/ extern int race_get_phase();
/*0040ff60*/ extern int race_get_phase_timer();
/*0041a380*/ extern int ai_ftol_noop_wrapper();
/*00421210*/ extern int game_get_game_659d00();
/*0042c440*/ extern int __cdecl ui_rt2d_text_draw(int id, float x, float y, int colour, float scale, int flags);
/*0042e6c0*/ extern int race_lose_banner_draw();
/*0042e780*/ extern int intro_banner_draw_team_text();
/*0042ed70*/ extern int race_winner_banner_draw();
/*00434bb0*/ extern int get_event_type();
/*0044c540*/ extern int camera_get_car_8c747c();
/*00475730*/ extern char *get_viewport_camera();
/*004824a0*/ extern int draw_rect();
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*005dd32c*/ extern float k_f_3;
/*005dd7f4*/ extern float k_f_80;
/*005ddb20*/ extern float k_f_0_9;
/*005e64a4*/ extern float k_f_0;
/*005ddf74*/ extern float k_f_64;
/*005de290*/ extern float float_50_0;
/*005de8fc*/ extern float k_f_68;
/*005de900*/ extern float k_f_52;
/*005de904*/ extern float k_f_480;
/*007d2128*/ extern char *RwEngineInstance;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7fc*/ extern undefined4 g_race_7ef7fc;
/*008c8790*/ extern undefined4 g_intro_banner_text_id;
/*008c8794*/ extern float g_intro_banner_slide_step;
/*008c8798*/ extern undefined4 g_intro_banner_offset;
/*0041a370*/ extern float ai_get_counter();
/* race_status_messages_draw */

/* "'Car is wrecked', 'Time Trial Abandoned', 'Fugitive has lapped you!' (texts 598, 599, 534 ...)"
    */

void race_status_messages_draw(void)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  undefined4 shadow_colour;
  float text_colour;
  RwCamera *camera;
  int scratch_int;
  RwCamera *clear_camera;
  undefined4 draw_team_text;
  double counter_b;
  RwRGBA *clear_colour_ptr;
  RwInt32 clear_flags;
  struct {
    undefined4 colour_white;
    RwRGBA clear_colour;
    undefined4 colour_black;
    undefined4 rect_height_bits;
    float text_colour_f;
    float rect_y;
  } frame;
  RwV2d view_window_new;
  RwV2d view_window_saved;
  
  frame.clear_colour.red = '\0';
  frame.clear_colour.green = '\0';
  frame.clear_colour.blue = '\0';
  frame.clear_colour.alpha = '\0';
  frame.colour_black = 0xff000000;
  frame.text_colour_f = -2.5554925e+38;
  frame.colour_white = 0xffffffff;
  camera = (RwCamera *)get_viewport_camera(0);
  scratch_int = get_event_type();
  text_colour = frame.text_colour_f;
  shadow_colour = frame.colour_black;
  if ((((scratch_int == 0) || (scratch_int = get_event_type(), scratch_int == 1)) ||
      (scratch_int = get_event_type(), scratch_int == 2)) &&
     ((scratch_int = race_get_phase(), scratch_int == 7 && (scratch_int = game_get_game_659d00(), scratch_int == 0)))) {
    RwCameraEndUpdate(camera);
    view_window_saved.x = RwCamera_viewWindow(camera).x;
    view_window_saved.y = RwCamera_viewWindow(camera).y;
    view_window_new.x = 0.800000012f;
    view_window_new.y = 0.800000012f;
    RwCameraSetViewWindow(camera,&view_window_new);
    clear_flags = 2;
    clear_colour_ptr = &frame.clear_colour;
    clear_camera = (RwCamera *)get_viewport_camera(0);
    RwCameraClear(clear_camera,clear_colour_ptr,clear_flags);
    RwCameraBeginUpdate(camera);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
    /* Case 1: event types 0/1/2 in race phase 7: shrunk view window (0.8), text ids 0x256 (598) and 599 each drawn twice (shadow at +3,+3, then main), then the camera is restored (inferred) */
    ui_rt2d_text_draw(0x256, 323.0f, 98.0f, shadow_colour, 1.0f, 2);
    ui_rt2d_text_draw(0x256, 320.0f, 95.0f, *(int *)&text_colour, 1.0f, 2);
    ui_rt2d_text_draw(599, 323.0f, 128.0f, shadow_colour, 1.0f, 2);
    ui_rt2d_text_draw(599, 320.0f, 125.0f, *(int *)&text_colour, 1.0f, 2);
    RwCameraEndUpdate(camera);
    RwCameraSetViewWindow(camera,&view_window_saved);
    RwCameraBeginUpdate(camera);
  }
  /* Case 2: camera car 8c747c != 1 draws the intro banner (g_intro_banner_text_id): black bars sliding by g_intro_banner_slide_step, text ids 0x2e / 0x42 / banner id, plus text 0xd4 blinking once the slide ended (inferred) */
  scratch_int = camera_get_car_8c747c();
  if (scratch_int != 1) {
    if (g_intro_banner_text_id != 0) {
      RwCameraEndUpdate(camera);
      view_window_saved.x = RwCamera_viewWindow(camera).x;
      view_window_saved.y = RwCamera_viewWindow(camera).y;
      view_window_new.x = 0.800000012f;
      view_window_new.y = 0.800000012f;
      RwCameraSetViewWindow(camera,&view_window_new);
      clear_flags = 2;
      clear_colour_ptr = &frame.clear_colour;
      clear_camera = (RwCamera *)get_viewport_camera(0);
      RwCameraClear(clear_camera,clear_colour_ptr,clear_flags);
      RwCameraBeginUpdate(camera);
      frame.colour_white = 0xff000000;
      frame.rect_height_bits = 0x42a00000;
      frame.colour_black = 0x44200000;
      frame.text_colour_f = 0.0;
      frame.rect_y = g_intro_banner_slide_step;
      draw_rect(0,*(int *)&g_intro_banner_slide_step,0x44200000,0x42a00000,0xff000000);
      frame.rect_y = (k_f_480 - g_intro_banner_slide_step) - k_f_64;
      draw_rect(0,*(int *)&frame.rect_y,0x44200000,frame.rect_height_bits,frame.colour_white);
      frame.colour_white = 0xffffffff;
      frame.rect_height_bits = 0x3f800000;
      draw_rect(0,*(int *)&frame.rect_y,0x44200000,0x3f800000,0xffffffff);
      frame.rect_y = g_intro_banner_slide_step + k_f_80;
      draw_rect(0,*(int *)&frame.rect_y,0x44200000,frame.rect_height_bits,0xffffffff);
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
      scratch_int = race_get_phase_timer();
      if (199 < scratch_int) {
        ui_rt2d_text_draw(0x2e, 64.0f, (k_f_480 - g_intro_banner_slide_step) - k_f_52, 0xffffffff, 0.600000024f, 0);
      }
      ui_rt2d_text_draw(0x42, 64.0f, g_intro_banner_slide_step + float_50_0, 0xffffffff, 0.800000012f, 0);
      ui_rt2d_text_draw(g_intro_banner_text_id, 64.0f, g_intro_banner_slide_step + k_f_68, 0xffffffff, 0.600000024f, 0);
      if (g_intro_banner_slide_step == k_f_0) {
        if (((g_race_7ef7fc != 0) && (((byte)g_intro_banner_offset & 0x3f) < 0x34)) &&
           (scratch_int = get_event_type(), scratch_int != 0)) {
          ui_rt2d_text_draw(0xd4, 320.0f, 400.0f, 0xffffffff, 1.0f, 2);
        }
      }
      else {
        g_intro_banner_offset = 0;
      }
      RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
      RwCameraEndUpdate(camera);
      RwCameraSetViewWindow(camera,&view_window_saved);
      RwCameraBeginUpdate(camera);
    }
    return;
  }
  scratch_int = race_get_phase();
  if (scratch_int != 0xb) {
    return;
  }
  scratch_int = get_event_type();
  if ((((scratch_int == EVENT_CUP_3) || (scratch_int = get_event_type(), scratch_int == 4)) ||
      (scratch_int = get_event_type(), scratch_int == 5)) && (g_car_7ef7b0 != 0)) {
    draw_team_text = 1;
  }
  else {
    draw_team_text = 0;
  }
  RwCameraEndUpdate(camera);
  view_window_saved.x = RwCamera_viewWindow(camera).x;
  view_window_saved.y = RwCamera_viewWindow(camera).y;
  view_window_new.x = 0.800000012f;
  view_window_new.y = 0.800000012f;
  RwCameraSetViewWindow(camera,&view_window_new);
  clear_flags = 2;
  clear_colour_ptr = &frame.clear_colour;
  clear_camera = (RwCamera *)get_viewport_camera(0);
  RwCameraClear(clear_camera,clear_colour_ptr,clear_flags);
  RwCameraBeginUpdate(camera);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  /* Case 3: race phase 0xb (end of race): per game_mode draw the winner or lose banner, optional team text (draw_team_text) and mode specific captions (0x216, 0x215, 0x231) */
  switch(game_mode) {
  case 5:
    scratch_int = fx_objects_all_exploded();
    if (scratch_int == 0) {
LAB_0042f22a:
      race_lose_banner_draw();
      goto LAB_0042f393;
    }
    race_winner_banner_draw(draw_team_text);
    draw_team_text = 1;
    break;
  default:
    race_winner_banner_draw(draw_team_text);
    break;
  case 8:
    scratch_int = ai_ftol_noop_wrapper(0);
    if (scratch_int == -1) goto LAB_0042f22a;
    goto LAB_0042f370;
  case 9:
    if (g_car_7ef7b0 == 0) {
      race_lose_banner_draw();
      counter_b = (double)ai_get_counter(1);
      frame.text_colour_f = (float)counter_b;
      counter_b = (double)ai_get_counter(0);
      if ((double)k_f_0_9 <= (double)frame.text_colour_f - counter_b) {
        ui_rt2d_text_draw(0x216, intro_banner_get_x() + k_f_3, 98.0f, shadow_colour, 1.0f, 2);
        ui_rt2d_text_draw(0x216, intro_banner_get_x(), 95.0f, *(int *)&text_colour, 1.0f, 2);
      }
    }
    else {
      race_winner_banner_draw(draw_team_text);
      intro_banner_draw_team_text(&frame.colour_white,1);
      ui_rt2d_text_draw(0x215, 320.0f, 98.0f, shadow_colour, 1.0f, 2);
      ui_rt2d_text_draw(0x215, 320.0f, 95.0f, frame.colour_white, 1.0f, 2);
    }
    goto LAB_0042f393;
  case 10:
    if (g_car_7ef7b0 == 0) {
      race_lose_banner_draw();
      ui_rt2d_text_draw(0x231, intro_banner_get_x() + k_f_3, 98.0f, shadow_colour, 1.0f, 2);
      ui_rt2d_text_draw(0x231, intro_banner_get_x(), 95.0f, *(int *)&text_colour, 1.0f, 2);
      goto LAB_0042f393;
    }
LAB_0042f370:
    race_winner_banner_draw(draw_team_text);
    draw_team_text = 1;
  }
  intro_banner_draw_team_text(&frame.colour_white,draw_team_text);
LAB_0042f393:
  RwCameraEndUpdate(camera);
  RwCameraSetViewWindow(camera,&view_window_saved);
  RwCameraBeginUpdate(camera);
  return;
}


