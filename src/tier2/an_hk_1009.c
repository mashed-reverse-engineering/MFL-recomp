#include "mfl_tier2.h"
/*0042be50*/ extern undefined __cdecl ui_text_draw_a(undefined4 p0, undefined4 p1, undefined4 p2);
/*0042bfd0*/ extern undefined __cdecl ui_text_draw_boxed(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, void *p8, undefined4 p9, undefined4 p10);
/*0042c220*/ extern int ui_text_draw_wide_a(undefined1 *text, undefined4 colour, undefined4 scale);
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c8e0*/ extern float text_width();
/*0042fce0*/ extern int menu_fade_overlay_draw(void);
/*00431210*/ extern int ui_draw_quad_scaled(float param_3, float param_4, float param_5, float param_6, undefined4 param_7);
/*00431290*/ extern int ui_draw_table_grid();
/*00475730*/ extern int get_viewport_camera();
/*004a2210*/ extern int camera_set_default_view_window();
/*0069cfd0*/ extern undefined4 g_message_state;
/*0069cfd4*/ extern undefined4 g_message_text_id;
/*0069cfe4*/ extern int g_message_timer;
/*0069cff4*/ extern undefined4 g_message_buttons;
/*0069cffc*/ extern undefined4 g_message_button_text_a;
/*0069d000*/ extern undefined4 g_message_button_text_b;
/*0069d008*/ extern undefined4 g_message_use_text_offset;
/*0069d010*/ extern undefined4 g_message_text_offset;
/*0069d1e4*/ extern undefined4 g_menu_fade_alpha;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
#define MESSAGE_MIN_TIMER (0x28) /* box is not drawn until the timer reaches this (inferred) */
#define MESSAGE_TEXT_SCALE (0x3f19999a) /* float 0.6 */
#define MESSAGE_BUTTON_TEXT_SCALE (0x3f0ccccd) /* float 0.55 */
#define MESSAGE_BUTTON_A_X (0x430c0000) /* float 140.0 */
#define MESSAGE_BUTTON_B_X (0x43870000) /* float 270.0 */
#define MESSAGE_BUTTON_Y (0x43ac8000) /* float 345.0 */
#define MESSAGE_TEXT_ID_WITH_BOX (0x286) /* text id that also draws text 0x287 in a box (inferred) */
/* message_box_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* "draws the message box (quads, table grid, title, body text by message type 1,2,7,9,0xB,0xC,
   button strings)" */

/* Draws the modal message box (header bar, body panel, footer bar, grid, title, message text, button labels by g_message_buttons); colours fade in with g_message_timer, nothing is drawn when no message is active (inferred). */
void message_box_draw(void)

{
  undefined4 scratch;
  undefined1 *text_buf_ptr;
  int text_id;
  undefined4 scratch_arg;
  union {
    char gs_buf[532];
    struct {
      undefined4 text_colour;
      undefined4 title_colour;
      int bar_colour;
      undefined4 grid_colour;
      undefined4 body_colour;
      undefined1 text_buf [512];
    } m;
  } frame;
  
  if (g_message_state == 0) {
    return;
  }
  if ((int)g_message_timer < MESSAGE_MIN_TIMER) {
    return;
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  text_id = g_message_text_id;
  if (g_message_use_text_offset != 0) {
    text_id = g_message_text_offset + g_message_text_id;
  }
  text_width(text_id,MESSAGE_TEXT_SCALE);
  scratch = g_menu_fade_alpha;
  g_menu_fade_alpha = (int)((byte)g_message_timer * 3) >> 2;
  scratch_arg = 0;
  menu_fade_overlay_draw();
  frame.m.body_colour = CONCAT13((byte)g_message_timer,0x202020);
  frame.m.grid_colour = CONCAT13((byte)g_message_timer,0xffffff);
  frame.m.bar_colour = (byte)g_message_timer << 0x18;
  frame.m.text_colour = CONCAT13((byte)g_message_timer,0xdcdcdc);
  frame.m.title_colour = CONCAT13((byte)g_message_timer,0xffffff);
  g_menu_fade_alpha = scratch;
  scratch = get_viewport_camera(0xffffffff,scratch_arg);
  RwCameraEndUpdate(scratch);
  camera_set_default_view_window();
  scratch = get_viewport_camera(0xffffffff);
  RwCameraBeginUpdate(scratch);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  ui_draw_quad_scaled(130.0,120.0,380.0,42.0,frame.m.bar_colour);
  ui_draw_quad_scaled(130.0,162.0,380.0,166.0,frame.m.body_colour);
  ui_draw_quad_scaled(130.0,328.0,380.0,32.0,frame.m.bar_colour);
  ui_draw_table_grid(frame.m.grid_colour);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  if (g_message_use_text_offset == 0) {
    scratch = 0x42;
  }
  else {
    scratch = 0xbb;
  }
  ui_rt2d_text_draw(scratch,0x430c0000,0x430e0000,frame.m.title_colour,0x3f4ccccd,0);
  scratch = MESSAGE_TEXT_SCALE;
  if (g_message_text_id == MESSAGE_TEXT_ID_WITH_BOX) {
    text_buf_ptr = frame.m.text_buf;
    scratch_arg = frame.m.text_colour;
    ui_text_draw_wide_a(frame.m.text_buf,frame.m.text_colour,scratch);
    ui_text_draw_boxed(0x287,0x43110000,0x43858000,0x43af0000,0x43160000,(undefined4)frame.m.text_colour,0x3f0ccccd,0,                        text_buf_ptr,scratch_arg,scratch);
  }
  else if (g_message_use_text_offset == 0) {
    ui_text_draw_a(g_message_text_id,frame.m.text_colour,MESSAGE_TEXT_SCALE);
  }
  else {
    ui_text_draw_a(g_message_text_offset + g_message_text_id,frame.m.text_colour,MESSAGE_TEXT_SCALE);
  }
  switch(g_message_buttons) {
  case 1:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
    break;
  case 2:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
    goto LAB_0043a0e6;
  case 7:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
    break;
  case 9:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
    goto LAB_0043a0e6;
  case 0xb:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
    break;
  case 0xc:
    ui_rt2d_text_draw(g_message_button_text_a,MESSAGE_BUTTON_A_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
LAB_0043a0e6:
    ui_rt2d_text_draw(g_message_button_text_b,MESSAGE_BUTTON_B_X,MESSAGE_BUTTON_Y,frame.m.text_colour,MESSAGE_BUTTON_TEXT_SCALE,0);
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


