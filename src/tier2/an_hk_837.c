#include "mfl_tier2.h"
/*0042b920*/ extern int ui_draw_line(float param_2, float param_3, float param_4, float param_5, undefined1 *param_6);
/*0042bf10*/ extern int ui_text_draw_b();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c8e0*/ extern float text_width();
/*00431210*/ extern int ui_draw_quad_scaled(float param_3, float param_4, float param_5, float param_6, undefined4 param_7);
/*00475730*/ extern int get_viewport_camera();
/*004a2210*/ extern int camera_set_default_view_window();
/*004b2b48*/ extern int _ftol();
/*005ddc28*/ extern float k_f_0;
/*0069cfd4*/ extern undefined4 g_message_text_id;
/*0069d2d8*/ extern undefined4 g_car_69d2d8;
/*0069d2dc*/ extern undefined4 g_ui_69d2dc;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008c8078*/ extern undefined4 g_training_lesson;

/* render state values passed to fpRenderStateSet (rwRENDERSTATEZTESTENABLE / ZWRITEENABLE) */
#define RENDER_STATE_OFF (0x0)
#define RENDER_STATE_ON (0x1)
/* text ids: 0x2b1 = 689 title, 0x2b0 = 688 prompt (from header comment); 0x2e = 46 (inferred) */
#define TEXT_TRAINING_TITLE (0x2b1)
#define TEXT_TRAINING_PROMPT (0x2b0)
#define TEXT_PROMPT_FIRST_SELECTED (0x2e)
#define TEXT_LESSON_NAME_BASE (699)
/* 0xdcdcdc: RGB of the text colour; alpha is in the top byte (inferred) */
#define TEXT_COLOUR_RGB (0xdcdcdc)
/* panel colours, RGB in the low 3 bytes, alpha in the top byte (inferred) */
#define BAR_COLOUR_RGB (0x804000)
#define PANEL_COLOUR_RGB (0x201000)
/* panel edges, same literal types as the original calls (inferred layout) */
#define PANEL_LEFT_X (60.0)
#define PANEL_RIGHT_X (580.0)
#define PANEL_TITLE_TOP_Y (90.0)
#define PANEL_BODY_TOP_Y (122.0)
#define PANEL_FOOTER_TOP_Y (218.0)
#define PANEL_BOTTOM_Y (250.0)

/* menu_draw_training_prompt */

/*
 * Draws the Training page overlay: a dark blue panel (title bar, body, footer bar and outline lines),
 * the "Training" title, the lesson name and the prompt text (texts 689, 688 per the header comment).
 * Takes no parameters and returns nothing. Only draws when g_car_69d2d8 is set and entry_enabled[0]
 * of menu record 0 is not zero (inferred: k_f_0 is 0.0).
 * (was: "'Training' title and Continue / Back prompt (texts 689, 688)")
 */

void menu_draw_training_prompt(void)

{
  byte fade_alpha;
  undefined1 dim_alpha;
  /* viewport camera, later reused as a colour word */
  undefined4 camera_or_colour;
  struct {
    undefined4 text_colour;
    undefined1 line_colour_r;
    undefined1 line_colour_g;
    undefined1 line_colour_b;
    byte line_colour_a;
    undefined4 white_colour;
    undefined4 bar_colour;
    undefined4 panel_colour;
    float fade_level;
  } colours;
  
  colours.white_colour = 0xffffffff;
  /* gate: game car present and menu record 0 entry 0 enabled; fade_level holds entry_enabled[0] as a float */
  if ((g_car_69d2d8 != 0) && (colours.fade_level = (float)g_menu_records[0].entry_enabled[0], colours.fade_level != k_f_0)
     ) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)RENDER_STATE_OFF);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RENDER_STATE_OFF);
    text_width(g_message_text_id,0x3f19999a);
    /* colours: alpha = fade_alpha (inferred: brightness from entry_enabled[0]); dim_alpha is 75% of it, for the bars and panel */
    colours.bar_colour = CONCAT13((*(byte *)((char *)&colours.bar_colour + 3)),BAR_COLOUR_RGB);
    colours.panel_colour = CONCAT13((*(byte *)((char *)&colours.panel_colour + 3)),PANEL_COLOUR_RGB);
    colours.line_colour_r = 0xff;
    colours.line_colour_g = 0xff;
    colours.line_colour_b = 0xff;
    fade_alpha = ((int)(colours.fade_level));
    colours.fade_level = (float)(uint)fade_alpha;
    dim_alpha = ((int)((float)colours.fade_level * 0.75f));
    colours.panel_colour = CONCAT13(dim_alpha,(undefined3)colours.panel_colour);
    colours.bar_colour = CONCAT13(dim_alpha,(undefined3)colours.bar_colour);
    colours.text_colour = CONCAT13(fade_alpha,TEXT_COLOUR_RGB);
    colours.white_colour = CONCAT13(fade_alpha,(undefined3)colours.white_colour);
    colours.line_colour_a = fade_alpha;
    /* end the camera update, reset the default view window, and begin it again for the 2D draw */
    camera_or_colour = get_viewport_camera(0xffffffff);
    RwCameraEndUpdate(camera_or_colour);
    camera_set_default_view_window();
    camera_or_colour = get_viewport_camera(0xffffffff);
    RwCameraBeginUpdate(camera_or_colour);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)RENDER_STATE_OFF);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RENDER_STATE_OFF);
    /* panel: title bar, body, footer bar, then the outline lines (white with fade_alpha) */
    camera_or_colour = colours.bar_colour;
    ui_draw_quad_scaled(PANEL_LEFT_X,PANEL_TITLE_TOP_Y,520.0,32.0,colours.bar_colour);
    ui_draw_quad_scaled(PANEL_LEFT_X,PANEL_BODY_TOP_Y,520.0,96.0,colours.panel_colour);
    ui_draw_quad_scaled(PANEL_LEFT_X,PANEL_FOOTER_TOP_Y,520.0,32.0,camera_or_colour);
    ui_draw_line(PANEL_LEFT_X,PANEL_TITLE_TOP_Y,PANEL_RIGHT_X,PANEL_TITLE_TOP_Y,&colours.line_colour_r);
    ui_draw_line(PANEL_LEFT_X,PANEL_BODY_TOP_Y,PANEL_RIGHT_X,PANEL_BODY_TOP_Y,&colours.line_colour_r);
    ui_draw_line(PANEL_LEFT_X,PANEL_FOOTER_TOP_Y,PANEL_RIGHT_X,PANEL_FOOTER_TOP_Y,&colours.line_colour_r);
    ui_draw_line(PANEL_LEFT_X,PANEL_BOTTOM_Y,PANEL_RIGHT_X,PANEL_BOTTOM_Y,&colours.line_colour_r);
    ui_draw_line(PANEL_LEFT_X,PANEL_TITLE_TOP_Y,PANEL_LEFT_X,PANEL_BOTTOM_Y,&colours.line_colour_r);
    ui_draw_line(PANEL_RIGHT_X,PANEL_TITLE_TOP_Y,PANEL_RIGHT_X,PANEL_BOTTOM_Y,&colours.line_colour_r);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)RENDER_STATE_OFF);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RENDER_STATE_OFF);
    /* text: title (white), lesson name, then the prompt line, which depends on whether entry 0 is selected */
    camera_or_colour = colours.white_colour;
    ui_rt2d_text_draw(TEXT_TRAINING_TITLE,0x428c0000,0x42d40000,colours.white_colour,0x3f4ccccd,0);
    ui_rt2d_text_draw(g_training_lesson + TEXT_LESSON_NAME_BASE,0x440e8000,0x42d40000,camera_or_colour,0x3f19999a,1);
    if (g_menu_records[0].selected == 0) {
      ui_rt2d_text_draw(TEXT_PROMPT_FIRST_SELECTED,0x428c0000,0x436a0000,colours.text_colour,0x3f0ccccd,0);
    }
    else {
      ui_rt2d_text_draw(TEXT_TRAINING_PROMPT,0x428c0000,0x436a0000,colours.text_colour,0x3f0ccccd,0);
    }
    /* text colour alpha from entry_enabled[1] for g_ui_69d2dc, from entry_enabled[2] for item_list, then the prompt again */
    colours.text_colour = CONCAT13((undefined1)g_menu_records[0].entry_enabled[1],TEXT_COLOUR_RGB);
    ui_text_draw_b(g_ui_69d2dc,colours.text_colour,0x3f19999a);
    colours.text_colour = CONCAT13((undefined1)g_menu_records[0].entry_enabled[2],TEXT_COLOUR_RGB);
    ui_text_draw_b(g_menu_records[0].item_list,colours.text_colour,0x3f19999a);
    ui_rt2d_text_draw(TEXT_TRAINING_PROMPT,0x428c0000,0x436a0000,colours.text_colour,0x3f0ccccd,0);
    /* restore depth test and depth write */
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)RENDER_STATE_ON);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)RENDER_STATE_ON);
  }
  return;
}


