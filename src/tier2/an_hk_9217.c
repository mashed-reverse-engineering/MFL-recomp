#include "mfl_tier2.h"
/*0040d0f0*/ extern int __cdecl find_texture_dict_c(RwChar *p0);
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c8e0*/ extern float text_width();
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00483130*/ extern int draw_sprite();
/*004832d0*/ extern int draw_sprite_uv(int *param_3, float param_4, float param_5, float param_6, float param_7, uint param_8, RwReal param_9, RwReal param_10, RwReal param_11, RwReal param_12, int param_13, int param_14);
/*004b2b48*/ extern int _ftol();
/*005dd334*/ extern float k_f_0_033333335;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd7f0*/ extern float k_f_30;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb50*/ extern float k_f_8;
/*005de7c0*/ extern float k_f_0_0015625;
/*005deb24*/ extern float k_f_382;
/*005deb28*/ extern float k_f_0_55625;
/*0060a5b0*/ extern undefined g_menu_page_33_bonus_features;
/*0069ca00*/ extern undefined4 g_menu_panel_state_69ca00;
/*0069ca04*/ extern undefined4 g_menu_69ca04;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*007d2128*/ extern char *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
static int menu_item_y_position_r(uint row_count, int reg_ecx);
extern void menu_item_y_position(void);
__declspec(naked) int menu_item_y_position_r(uint row_count, int reg_ecx)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}

/* menu_panel_draw_69ca00 */

/* "panel with state flag g_menu_panel_state_69ca00 (fade counter 69ca04)" */

void menu_panel_draw_69ca00(void)

{
  short screen_dim;
  undefined *item_list;
  int rows_or_flag;
  int *arrow_dict;
  undefined4 arrow_tex;
  uint *option_flag_ptr;
  int row_count;
  float arrow_x;
  float arg_y;
  float arg_w;
  float arg_h;
  uint arg_color;
  RwReal arg_u0;
  RwReal arg_v0;
  float arrow_y;
  RwReal arg_u1;
  float arrow_w;
  RwReal arg_v1;
  float arrow_h;
  int fade_counter;
  undefined4 arg_flag;
  struct {
    uint fade_color;
    float row_y;
    int rows_left;
  } frame;
  
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  fade_counter = g_menu_69ca04;
  if ((g_menu_panel_state_69ca00 == 0) && ((int)g_menu_69ca04 < 0x60)) {
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    return;
  }
  /* rows: 4, or 2 on the bonus features page; the list is taken from the current or the next menu record by panel state */
  row_count = 4;
  if (g_menu_panel_state_69ca00 == 1) {
    item_list = (undefined *)g_menu_records[g_menu_current].item_list;
  }
  else {
    item_list = (undefined *)g_menu_records[g_menu_current + 1].item_list;
  }
  if (item_list == &g_menu_page_33_bonus_features) {
    row_count = 2;
  }
  rows_or_flag = menu_item_y_position_r(row_count,0x1e);
  frame.row_y = (float)rows_or_flag;
  frame.fade_color = fade_counter << 0x18;
  if (row_count != 0) {
    option_flag_ptr = g_game_state.option_flags + (item_list == &g_menu_page_33_bonus_features) + 1;
    frame.rows_left = row_count;
    do {
      screen_dim = screen_width();
      /* per row: arrow sprite, option text (on/off string 0x5c/0x5d) and a second arrow after the text */
      arrow_x = (float)(int)screen_dim * k_f_0_55625;
      screen_dim = screen_height();
      arrow_y = (frame.row_y - k_f_8) * (float)(int)screen_dim * k_f_0_0020833334;
      screen_dim = screen_width();
      arrow_w = (float)(int)screen_dim * k_f_0_025;
      screen_dim = screen_height();
      rows_or_flag = 0;
      fade_counter = 0;
      arg_v1 = 1.0;
      arg_u1 = 0.0;
      arg_v0 = 0.0;
      arg_u0 = 0.9999;
      arrow_h = (float)(int)screen_dim * k_f_0_033333335;
      arg_y = arrow_y;
      arg_w = arrow_w;
      arg_h = arrow_h;
      arg_color = frame.fade_color;
      arrow_dict = (int *)find_texture_dict_c("Arrow");
      draw_sprite_uv(arrow_dict,arrow_x,arg_y,arg_w,arg_h,arg_color,arg_u0,arg_v0,
                     arg_u1,arg_v1,fade_counter,rows_or_flag);
      row_count = (*option_flag_ptr != 0) + 0x5c;
      ui_rt2d_text_draw(row_count,0x43bc0000,*(int *)&frame.row_y,frame.fade_color,0x3f333333,0);
      fade_counter = (int)text_width(row_count,0x3f333333);
      screen_dim = screen_width();
      arg_flag = 1;
      arrow_x = ((float)fade_counter + k_f_382) * (float)(int)screen_dim * k_f_0_0015625;
      arg_color = frame.fade_color;
      arrow_tex = find_texture_dict_c("Arrow");
      draw_sprite(arrow_tex,*(int *)&arrow_x,*(int *)&arrow_y,*(int *)&arrow_w,*(int *)&arrow_h,arg_color,arg_flag);
      frame.row_y = frame.row_y + k_f_30;
      option_flag_ptr = option_flag_ptr + 1;
      frame.rows_left = frame.rows_left + -1;
    } while (frame.rows_left != 0);
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}


