#include "mfl_tier2.h"
/*0042be50*/ extern undefined __cdecl ui_text_draw_a(undefined4 p0, undefined4 p1, undefined4 p2);
/*0042bfd0*/ extern undefined __cdecl ui_text_draw_boxed(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10);
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
/* message_box_draw */
/* compile: /GS */

/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "draws the message box (quads, table grid, title, body text by message type 1,2,7,9,0xB,0xC,
   button strings)" */

void message_box_draw(void)

{
  undefined4 camera;
  undefined1 *puVar1;
  int iVar2;
  undefined4 uVar3;
  union {
    char gs_buf[532];
    struct {
      undefined4 local_218;
      undefined4 local_214;
      int iStack_210;
      undefined4 uStack_20c;
      undefined4 uStack_208;
      undefined1 auStack_204 [512];
    } m;
  } frame;
  
  if (g_message_state == 0) {
    return;
  }
  if ((int)g_message_timer < 0x28) {
    return;
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  iVar2 = g_message_text_id;
  if (g_message_use_text_offset != 0) {
    iVar2 = g_message_text_offset + g_message_text_id;
  }
  text_width(iVar2,0x3f19999a);
  camera = g_menu_fade_alpha;
  g_menu_fade_alpha = (int)((byte)g_message_timer * 3) >> 2;
  uVar3 = 0;
  menu_fade_overlay_draw();
  frame.m.uStack_208 = CONCAT13((byte)g_message_timer,0x202020);
  frame.m.uStack_20c = CONCAT13((byte)g_message_timer,0xffffff);
  frame.m.iStack_210 = (byte)g_message_timer << 0x18;
  frame.m.local_218 = CONCAT13((byte)g_message_timer,0xdcdcdc);
  frame.m.local_214 = CONCAT13((byte)g_message_timer,0xffffff);
  g_menu_fade_alpha = camera;
  camera = get_viewport_camera(0xffffffff,uVar3);
  RwCameraEndUpdate(camera);
  camera_set_default_view_window();
  camera = get_viewport_camera(0xffffffff);
  RwCameraBeginUpdate(camera);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  ui_draw_quad_scaled(130.0,120.0,380.0,42.0,frame.m.iStack_210);
  ui_draw_quad_scaled(130.0,162.0,380.0,166.0,frame.m.uStack_208);
  ui_draw_quad_scaled(130.0,328.0,380.0,32.0,frame.m.iStack_210);
  ui_draw_table_grid(frame.m.uStack_20c);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  if (g_message_use_text_offset == 0) {
    camera = 0x42;
  }
  else {
    camera = 0xbb;
  }
  ui_rt2d_text_draw(camera,0x430c0000,0x430e0000,frame.m.local_214,0x3f4ccccd,0);
  camera = 0x3f19999a;
  if (g_message_text_id == 0x286) {
    puVar1 = frame.m.auStack_204;
    uVar3 = frame.m.local_218;
    ui_text_draw_wide_a(frame.m.auStack_204,frame.m.local_218,camera);
    ui_text_draw_boxed(0x287,0x43110000,0x43858000,0x43af0000,0x43160000,(undefined4)frame.m.local_218,0x3f0ccccd,0,                        (undefined4)puVar1,uVar3,camera);
  }
  else if (g_message_use_text_offset == 0) {
    ui_text_draw_a(g_message_text_id,frame.m.local_218,0x3f19999a);
  }
  else {
    ui_text_draw_a(g_message_text_offset + g_message_text_id,frame.m.local_218,0x3f19999a);
  }
  switch(g_message_buttons) {
  case 1:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
    break;
  case 2:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
    goto LAB_0043a0e6;
  case 7:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
    break;
  case 9:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
    goto LAB_0043a0e6;
  case 0xb:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
    break;
  case 0xc:
    ui_rt2d_text_draw(g_message_button_text_a,0x430c0000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
LAB_0043a0e6:
    ui_rt2d_text_draw(g_message_button_text_b,0x43870000,0x43ac8000,frame.m.local_218,0x3f0ccccd,0);
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


