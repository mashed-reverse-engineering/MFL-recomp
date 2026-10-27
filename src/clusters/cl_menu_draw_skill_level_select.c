// cluster-entries: menu_draw_skill_level_select
/* Installed cluster menu_draw_skill_level_select (tools/cluster_install.py from output/cluster_pilot_j1/menu_draw_skill_level_select.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042c540*/ extern int text_draw_plain();
/*0042c8e0*/ extern float text_width();
/*00430eb0*/ extern undefined __cdecl ui_keyboard_icon_find(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*00435040*/ extern int team_color_names();
/*004832d0*/ extern int draw_sprite_uv(int *param_3, float param_4, float param_5, float param_6, float param_7, uint param_8, RwReal param_9, RwReal param_10, RwReal param_11, RwReal param_12, int param_13, int param_14);
/*004b2b48*/ extern int _ftol();
/*005dd634*/ extern float k_f_2;
/*005ddb0c*/ extern float k_f_15;
/*005ddb50*/ extern float k_f_8;
/*005ddf74*/ extern float k_f_64;
/*005de3e4*/ extern float k_f_40;
/*005decd4*/ extern float k_f_65;
/*0069c9ac*/ extern undefined1 g_menu_69c9ac;
/*0069ca28*/ extern undefined4 g_menu_controller_vehicle;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7d0*/ extern undefined4 g_menu_7ef7d0;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
static int ui_draw_framed_rect_r(int reg_eax, float stk0, float stk1, float stk2, float stk3);
extern void ui_draw_framed_rect(void);
__declspec(naked) int ui_draw_framed_rect_r(int reg_eax, float stk0, float stk1, float stk2, float stk3)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 20]
        call ui_draw_framed_rect
        add esp, 16
        ret
    }
}

/* menu_draw_skill_level_select */

/* "skill levels Rookie / Amateur / Pro / Elite (texts 215, 214, 213, 230)" */

void menu_draw_skill_level_select(void)

{
  float fbits1;
  float fbits2;
  byte alpha;
  uint title_color_copy;
  undefined4 rect_result;
  int *team_texture;
  int label_width;
  float row_y;
  int *slot_team_ptr;
  int *ctrl_vehicle_cur;
  struct {
    char number_label [4];
    int controller_index;
    int shadow_color;
    undefined1 team_col5_r;
    undefined1 team_col5_g;
    undefined1 team_col5_b;
    byte team_col5_a;
    undefined1 team_col4_r;
    undefined1 team_col4_g;
    undefined1 team_col4_b;
    byte team_col4_a;
    undefined1 team_col3_r;
    undefined1 team_col3_g;
    undefined1 team_col3_b;
    byte team_col3_a;
    undefined1 team_col2_r;
    undefined1 team_col2_g;
    undefined1 team_col2_b;
    byte team_col2_a;
    undefined1 team_col1_r;
    undefined1 team_col1_g;
    undefined1 team_col1_b;
    byte team_col1_a;
    undefined1 team_col0_r;
    undefined1 team_col0_g;
    undefined1 team_col0_b;
    byte team_col0_a;
    undefined4 white_tint;
    undefined4 title_color;
    float label_off;
    int *ctrl_vehicle_ptr;
    float slot_x;
    float slot_y;
    float box_x;
    float box_y;
    float text_x;
    float icon_y;
    float box_w;
    float box_h;
    float icon_w;
    float icon_h;
    undefined1 *team_color_table [4];
    undefined1 *team_col4_ptr;
    undefined1 *team_col5_ptr;
  } frame;
  
  alpha = g_menu_69c9ac;
  frame.team_col4_g = 0xa7;
  frame.team_col4_b = 0xa7;
  /* per team colour table: 6 RGBA entries; boxes at x = 65/195/325/455 */
  frame.team_color_table[1] = &frame.team_col1_r;
  frame.box_h = 28.0;
  row_y = 28.0;
  frame.team_color_table[0] = &frame.team_col0_r;
  frame.box_w = 120.0;
  frame.team_color_table[3] = &frame.team_col3_r;
  frame.team_col4_ptr = &frame.team_col4_r;
  frame.team_color_table[2] = &frame.team_col2_r;
  frame.box_y = 140.0;
  frame.box_x = 65.0;
  frame.team_col5_ptr = &frame.team_col5_r;
  frame.team_col0_r = 0x98;
  frame.team_col0_g = 0x3a;
  frame.team_col0_b = 0x3d;
  frame.team_col1_r = 0x4e;
  frame.team_col1_g = 0x89;
  frame.team_col1_b = 0xae;
  frame.team_col2_r = 0x61;
  frame.team_col2_g = 0x76;
  frame.team_col2_b = 0x56;
  frame.team_col3_r = 0xdb;
  frame.team_col3_g = 0xc3;
  frame.team_col3_b = 0x62;
  frame.team_col4_r = 0xeb;
  frame.team_col5_r = 0xff;
  frame.team_col5_g = 0xff;
  frame.team_col5_b = 0xff;
  frame.team_col5_a = g_menu_69c9ac;
  frame.team_col4_a = g_menu_69c9ac;
  frame.team_col3_a = g_menu_69c9ac;
  frame.team_col2_a = g_menu_69c9ac;
  frame.team_col1_a = g_menu_69c9ac;
  frame.team_col0_a = g_menu_69c9ac;
  frame.title_color = CONCAT13(g_menu_69c9ac,(undefined3)frame.title_color);
  rect_result = ui_draw_framed_rect_r(CONCAT31((int3)((uint)frame.team_color_table[3] >> 8),g_menu_69c9ac),65.0,140.0,
                              120.0,28.0);
  frame.box_x = 195.0;
  ui_draw_framed_rect_r(CONCAT31((int3)((uint)rect_result >> 8),alpha),195.0,140.0,120.0,28.0);
  frame.box_x = 325.0;
  rect_result = ui_draw_framed_rect_r(CONCAT31(0x43a280,alpha),325.0,140.0,120.0,28.0);
  frame.box_x = 455.0;
  ui_draw_framed_rect_r(CONCAT31((int3)((uint)rect_result >> 8),alpha),455.0,140.0,120.0,28.0);
  frame.box_x = 60.0;
  frame.box_y = 200.0;
  frame.box_w = 523.0;
  frame.icon_w = 48.0;
  frame.icon_h = 48.0;
  frame.white_tint = CONCAT13(alpha,0xffffff);
  frame.shadow_color = (uint)alpha << 0x18;
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x0);
  frame.controller_index = 0;
  frame.ctrl_vehicle_ptr = &g_menu_controller_vehicle;
  do {
    /* for every controller draw one icon + digit + team sprite per player slot using it */
    slot_team_ptr = &g_player_slots[0].team;
    ctrl_vehicle_cur = frame.ctrl_vehicle_ptr;
    do {
      if (((PlayerSlot *)(slot_team_ptr + -2))->controller == frame.controller_index) {
        ui_draw_framed_rect_r(CONCAT31((int3)(*(uint *)&frame.box_y >> 8),alpha),frame.box_x,frame.box_y,frame.box_w,
                            row_y);
        team_texture = (int *)team_color_names(*slot_team_ptr);
        frame.slot_x = (float)(*ctrl_vehicle_cur * 0x82) + k_f_64;
        frame.slot_y = frame.box_y - k_f_2;
        row_y = frame.slot_y + k_f_15;
        frame.label_off = row_y;
        frame.text_x = frame.slot_x;
        ui_keyboard_icon_find
                  (((PlayerSlot *)(slot_team_ptr + -2))->controller,(fbits1 = frame.slot_x + k_f_65, *(int *)&fbits1),*(int *)&row_y,
                   *(undefined4 *)frame.team_color_table[*slot_team_ptr],(uint)(*slot_team_ptr == 5));
        label_width = (int)text_width(0x37,0x3f666666);
        frame.label_off = (float)(label_width / 2);
        frame.number_label[0] = (char)frame.controller_index;
        if ((g_menu_7ef7d0 != 0) && ((int)&g_menu_controller_vehicle + 0x47 < (int)frame.ctrl_vehicle_ptr)) {
          frame.number_label[0] = (char)frame.controller_index + -6;
        }
        frame.number_label[0] = frame.number_label[0] + '1';
        frame.number_label[1] = 0;
        text_draw_plain(frame.number_label,(fbits2 = (float)(int)frame.label_off + frame.text_x + k_f_65, *(int *)&fbits2),*(int *)&row_y,frame.shadow_color,
                        0x3f19999a,0);
        frame.icon_y = frame.slot_y - k_f_8;
        frame.text_x = frame.slot_x;
        draw_sprite_uv(team_texture,frame.slot_x,frame.icon_y,frame.icon_w,frame.icon_h,frame.white_tint,0.0
                       ,1.0,0.0,1.0,1,1);
        frame.box_y = frame.box_y + k_f_40;
        row_y = frame.box_h;
        ctrl_vehicle_cur = frame.ctrl_vehicle_ptr;
      }
      slot_team_ptr = slot_team_ptr + 4;
    } while ((int)slot_team_ptr < (int)&g_player_slots + 0x48);
    frame.ctrl_vehicle_ptr = ctrl_vehicle_cur + 3;
    frame.controller_index = frame.controller_index + 1;
  } while ((int)frame.ctrl_vehicle_ptr < (int)&g_menu_controller_vehicle + 0x90);
  /* headings: texts 230, 213, 214, 215 over the four skill boxes */
  frame.title_color = frame.title_color & 0xff000000;
  title_color_copy = frame.title_color;
  ui_rt2d_text_draw(0xe6,0x42fa0000,0x431c0000,frame.title_color,0x3f400000,2);
  ui_rt2d_text_draw(0xd5,0x437f0000,0x431c0000,title_color_copy,0x3f400000,2);
  ui_rt2d_text_draw(0xd6,0x43c08000,0x431c0000,title_color_copy,0x3f400000,2);
  ui_rt2d_text_draw(0xd7,0x4400c000,0x431c0000,title_color_copy,0x3f400000,2);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  return;
}

