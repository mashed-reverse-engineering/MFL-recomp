// cluster-entries: results_screen_draw
/* Installed cluster results_screen_draw (tools/cluster_install.py from output/cluster_pilot_j1/results_screen_draw.c); entries are patched at their original addresses by build_overlay.py --clusters. */
/* compile: /GS */
#include "mfl_tier2.h"
#define EVENT_IS_TIME_TRIAL(t) ((t) == EVENT_TIME_TRIAL_0 || (t) == EVENT_TIME_TRIAL_1 || (t) == EVENT_TIME_TRIAL_2)
/* layout of the results panels (pixels on the 640x480 menu screen): rows are 26 high, the first starts at y = 100, the next one 32 below */
#define RESULTS_ROW_HEIGHT 26.0f
#define RESULTS_ROW0_TOP 100.0f
#define RESULTS_ROW0_TEXT_Y 113.0f /* text baseline inside the first row */
#define RESULTS_ROW1_TOP 132.0f
#define RESULTS_ROW2_TOP 164.0f
#define RESULTS_ROW3_TOP 196.0f
#pragma auto_inline(off)      /* the verified (diffemu, /Ob0) and the shipped (/O2) code must have the same call structure */
#pragma intrinsic(sin, cos)
/*0040c9f0*/ extern int __cdecl players_get_wins(int *order);
/*0040cad0*/ extern int __cdecl players_get_points(int *order);
/*0040cbb0*/ extern int __cdecl players_get_stat_8b90(int *order);
/*0040cc40*/ extern int __cdecl player_get_stat_8b90(int player);
/*0040cc50*/ extern int __cdecl player_get_wins(int player);
/*0040cc60*/ extern int __cdecl player_get_points(int player);
/*0040cd40*/ extern int round_get_round_652dbc(void);
/*0040cd50*/ extern int __cdecl player_get_stat(int player, int index);
/*0040d0f0*/ extern int __cdecl find_texture_dict_c(char *name);
/*0040fdb0*/ extern int get_player_count(void);
/*0040fe50*/ extern void __cdecl team_color_rgba(int team, unsigned char *rgba);
/*0042bd00*/ extern short * __cdecl dat_get_string(int id);
/*0042c440*/ extern void __cdecl ui_rt2d_text_draw(int id, float x, float y, unsigned int color, float scale, int align);
/*0042c540*/ extern void __cdecl text_draw_plain(char *text, float x, float y, unsigned int color, float scale, int align);
/*0042c8e0*/ extern double __cdecl text_width(int id, float scale);
/*0042c960*/ extern double __cdecl ui_rt2d_text_width(char *text, float scale);
/*0042e990*/ extern int time_is_better_than_best(void);
/*0042eb70*/ extern void best_time_store_previous(void);
/*0042ebd0*/ extern double __cdecl time_get_fraction(int which);
/*0042ebe0*/ extern int __cdecl time_get_minutes(int which);
/*0042ebf0*/ extern int __cdecl time_get_seconds(int which);
/*00430ab0*/ extern short screen_width(void);
/*00430ac0*/ extern short screen_height(void);
/*00432b50*/ extern void __cdecl ui_format_number_string(int value, float centi, char *out_a, char *out_b);
/*00432bc0*/ extern void __cdecl ui_compare_sign(int a, int b, int *sign, int *v1, int *v2, int *v3);
/*00435040*/ extern int __cdecl team_color_names(int team);
/*004824a0*/ extern void __cdecl draw_rect(float x, float y, float w, float h, unsigned int color);
/*00482f70*/ extern void __cdecl gfx_draw_rect_2d_four_alpha(float x, float y, float w, float h, unsigned int color, unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3);
/*00483130*/ extern void __cdecl draw_sprite(int texture, float x, float y, float w, float h, unsigned int color, int flag);
/*004832d0*/ extern void __cdecl draw_sprite_uv(int texture, float x, float y, float w, float h, unsigned int color, float u0, float v0, float u1, float v1, int flag_a, int flag_b);
/*00484760*/ extern double __cdecl degrees_to_radians(float degrees);
/*004b2a60*/ extern int __cdecl sprintf(char *buffer, const char *format, ...);
/*004b2b48*/ extern int _ftol();
/*005dd334*/ extern float k_f_0_033333335;
/*005dd624*/ extern float k_f_0_0020833334;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd41c*/ extern float float_5_0;
/*005dd418*/ extern float k_f_12;
/*005dd620*/ extern float float_10_0;
/*005dd62c*/ extern float k_f_100;
/*005dd634*/ extern float k_f_2;
/*005dd7e8*/ extern float k_f_60;
/*005dda24*/ extern float k_f_0_75;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb0c*/ extern float k_f_15;
/*005ddb14*/ extern float float_7_0;
/*005ddb50*/ extern float k_f_8;
/*005ddb58*/ extern float k_f_1000;
/*005ddc20*/ extern float k_f_45;
/*005ddc28*/ extern float k_f_0;
/*005de1fc*/ extern float k_f_111;
/*005de254*/ extern float k_f_359;
/*005de290*/ extern float float_50_0;
/*005de7c0*/ extern float k_f_0_0015625;
/*005de838*/ extern float k_f_640;
/*005de9a4*/ extern float k_f_48;
/*005de9b4*/ extern char g_frontend_5de9b4[];
/*005deb18*/ extern float k_f_13;
/*005deb54*/ extern float k_f_77;
/*005debc0*/ extern float k_f_82;
/*005debc4*/ extern float k_f_580;
/*005debc8*/ extern float k_f_1600;
/*005debcc*/ extern float k_f_n1500;
/*005debd0*/ extern char g_frontend_5debd0[];
/*005debd8*/ extern double k_f_1_4660155e13;
/*005debe0*/ extern float k_f_96;
/*005debe4*/ extern float k_f_0_022222223;
/*005debe8*/ extern float k_f_0_075;
/*005debec*/ extern float k_f_109;
/*005debf0*/ extern float k_f_79;
/*005debf4*/ extern char g_results_screen_5debf4[];
/*005debf8*/ extern char s__s_d_s_s_s_s_005debf8[];
/*005dec08*/ extern char g_results_screen_5dec08[];
/*005dec0c*/ extern float k_f_47;
/*005dec10*/ extern char g_frontend_5dec10[];
/*005dec1c*/ extern char g_frontend_5dec1c[];
/*005dec20*/ extern float k_f_14;
/*005dec24*/ extern float k_f_140;
/*005dec28*/ extern float k_f_220;
/*005dec2c*/ extern char PTR_s_ppearance___edge__normal_normsca_00646e30_2_005dec2c[];
/*005dec30*/ extern float k_f_19;
/*005dec34*/ extern float k_f_435;
/*005dec38*/ extern float k_f_365;
/*005dec3c*/ extern float k_f_295;
/*005dec40*/ extern float k_f_225;
/*005dec44*/ extern float k_f_155;
/*005dd59c*/ extern char g_frontend_5dd59c[];
/*0060ad5c*/ extern int g_menu_60ad5c;
/*0060ad60*/ extern int g_menu_60ad60;
/*0069c99c*/ extern unsigned char g_frontend_69c99c;
/*0069cbd4*/ extern int g_event_type;
/*0069cbdc*/ extern float g_results_screen_69cbdc;
/*0069cbe0*/ extern int g_tutorial_page_index;
/*0069cc40*/ extern int g_team_mode;
/*0069cc48*/ extern int g_menu_69cc48;
/*0069d100*/ extern int g_tutorial_line_count;
/*0069d104*/ extern int g_tutorial_display_ticks;
/*0069d108*/ extern int g_tutorial_line_entries[];
/*0069d1f8*/ extern int g_results_screen_69d1f8[4];
/*0069d208*/ extern int g_results_screen_69d208;
/*0069d20c*/ extern int g_results_screen_69d20c;
/*0069d210*/ extern int g_results_screen_69d210;
/*0069d214*/ extern int g_results_screen_69d214;
/*0069d21c*/ extern int g_menu_multiplayer_kind;
/*0069d288*/ extern int g_results_screen_69d288;
/*0069d2a8*/ extern int g_tutorial_69d2a8;
/*0069d2c8*/ extern int g_tutorial_69d2c8;
/*0069d2d0*/ extern int g_menu_career_race_active;
/*0069d31c*/ extern float g_frontend_69d31c;
/*0069d674*/ extern int g_results_screen_69d674;
/*0069d678*/ extern int g_results_screen_69d678;
/*0069d67c*/ extern float g_tutorial_69d67c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b0*/ extern int g_car_7ef7b0;
/*007ef7b8*/ extern int game_mode;
/*007ef7ec*/ extern float g_frame_seconds;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* results_screen_draw */

/* "large draw function with team colours, points, wins and stats ('%s%d%s%s%s%s')" */

static void ui_draw_framed_rect_r(unsigned int p0, float p1, float p2, float p3, float p4);
extern void ui_draw_framed_rect(void);
static void ui_get_triple_r(int p0, int p1, float * p2, int * p3, int * p4);
extern void ui_get_triple(void);
/* Draws the end-of-race results screen (no parameters). Depending on the event type it draws either the four
   stacked panels of a time trial / cup event (lap and total time from time_get_minutes/seconds/fraction,
   points via player_get_points, with a sign comparison against the previous value) or the per-player / per-
   team score table in the multiplayer events (team colour names, points, wins, stats formatted through
   sprintf into ui_rt2d_text_draw). Inferred from the callees and format strings; the meaning of individual
   columns is not fully confirmed. */
void results_screen_draw(void)
{
  unsigned char team_rgba[4];
  unsigned int white;
  unsigned int tint;
  unsigned int alpha;
  int show_tutorial;
  int nplayers;
  int rank0;
  int rank1;
  int rank2;
  int panel_x_int;
  float px;
  float py;
  float pw;
  float top;
  int index;
  int rows;
  int label;
  int prev_team;
  int team_run;
  int pidx;
  int tex;
  int value;
  float off;
  float y_a;
  float y_b;
  float x_a;
  float w_a;
  float w_b;
  int kind;
  int star_flag;
  float star_off;
  float star_radius;
  int star_count;
  int star_index;
  float star_x;
  float star_y;
  float star_sx;
  float star_sy;
  float angle;
  float star_w;
  float star_h;
  struct { int sign; int v1; int v2; float fv3; } cmp;
  struct { float centi; int sec; int min; } tr[4];
  int sign;
  int diff;
  int v1;
  int v2;
  int a;
  int b;
  char rank_text[4];
  char text[64];
  char small_text[32];
  char sec_text[32];
  char centi_text[32];
  int stat_x;
  int stat_i;
  int stat_rows;
  float scroll;
  float line_end;
  int line;
  int id;
  short *dat;
  int len;
  char clip[128];
  int pos;
  int j;
  float clip_w;
  float title_w;
  float page_w;
  float rect_w;
  int page_id;
  int bar_alpha;
  double cy_term;
  float fv3;
  unsigned int bar_color;
  unsigned int slot_color;
  unsigned int bar_color_unused;
  char *sign_text;
  float left;
  float radius;
  float cur_cx;
  float cur_cy;
  unsigned int shadow;
  unsigned int blue;

  team_rgba[0] = 0xff;
  team_rgba[3] = 0xff;
  white = 0xffffffff;
  team_rgba[1] = 0;
  team_rgba[2] = 0x30;
  tint = 0;
  if (game_mode == 2) {
    g_team_mode = 0;
  }
  team_color_rgba(0, team_rgba);
  show_tutorial = 1;
  if ((g_event_type == EVENT_CUP_3 || g_event_type == EVENT_CUP_4 || g_event_type == EVENT_CUP_5) &&
      (game_mode == 10 || game_mode == 9 || game_mode == 8 || game_mode == 7 || game_mode == 5)) {
    show_tutorial = 0;
  }
  switch (g_menu_69cc48) {
  case 0:
  case 1:
  case 2:
    players_get_points(g_results_screen_69d1f8);
    team_rgba[3] = (unsigned char)(int)g_tutorial_69d67c;
    break;
  case 3:
  case 4:
    if (g_menu_multiplayer_kind == 2) {
      players_get_stat_8b90(g_results_screen_69d1f8);
    }
    else {
      players_get_wins(g_results_screen_69d1f8);
    }
    /* fall through */
  case 5:
    if (g_menu_69cc48 == 5) {
      team_rgba[3] = g_frontend_69c99c;
    }
    else {
      team_rgba[3] = (unsigned char)(int)g_tutorial_69d67c;
    }
    break;
  }
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)0);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)0);
  nplayers = get_player_count();
  rank0 = 3;
  rank1 = 2;
  rank2 = 1;
  if (nplayers == 3) {
    rank0 = 2;
    rank1 = 1;
    rank2 = 0;
  }
  else if (nplayers == 2) {
    rank0 = 1;
    rank1 = 0;
  }
  if (g_team_mode != 0) {
    rank0 = 1;
    rank1 = 0;
  }
  panel_x_int = g_tutorial_69d2c8;
  if (panel_x_int < 60) {
    panel_x_int = 60;
  }
  px = (float)panel_x_int;
  pw = (float)(600 - panel_x_int);
  alpha = g_frontend_69c99c;
  tint = alpha << 24;
  slot_color = (alpha << 24) | 0x146ef0;
  /* top: set only by the four stacked panels below (100.0); every other path reads it uninitialised, as the original does (frame residue) */
  py = RESULTS_ROW0_TOP;
  if (!EVENT_IS_TIME_TRIAL(g_event_type)) {
    ui_draw_framed_rect_r(alpha, px, RESULTS_ROW0_TOP, pw, RESULTS_ROW_HEIGHT);
  }
  if (g_menu_69cc48 < 3) {
    ui_rt2d_text_draw(g_team_mode == 0 ? 0xa4 : 0x6e, px + float_10_0, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 0);
    goto common;
  }
  if (g_event_type != EVENT_MULTIPLAYER_6 && g_event_type != EVENT_MULTIPLAYER_7 && g_event_type != EVENT_MULTIPLAYER_8 &&
      !EVENT_IS_TIME_TRIAL(g_event_type)) {
    goto common;
  }
  if (g_team_mode != 0) {
    ui_rt2d_text_draw(0x6e, px + float_10_0, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 0);
  }
  else if (!EVENT_IS_TIME_TRIAL(g_event_type)) {
    ui_rt2d_text_draw(0xa4, px + float_10_0, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 0);
  }
  if (!EVENT_IS_TIME_TRIAL(g_event_type)) {
    x_a = px + k_f_155;
    ui_rt2d_text_draw(0x3e, x_a, 107.0f, tint, 0.5f, 2);
    ui_rt2d_text_draw(0x75, x_a, 119.0f, tint, 0.5f, 2);
    ui_rt2d_text_draw(0x70, px + k_f_225, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 2);
    ui_rt2d_text_draw(0x71, px + k_f_295, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 2);
    if (g_team_mode == 0) {
      if (nplayers > 2) {
        ui_rt2d_text_draw(0x72, px + k_f_365, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 2);
        if (g_team_mode != 0) {
          goto common;
        }
      }
      if (nplayers > 3) {
        ui_rt2d_text_draw(0x73, px + k_f_435, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 2);
      }
    }
    goto common;
  }
  /* time trial / cup events: four stacked panels */
  ui_draw_framed_rect_r(alpha, px, RESULTS_ROW0_TOP, pw, RESULTS_ROW_HEIGHT);
  ui_rt2d_text_draw(0x151, px + float_10_0, RESULTS_ROW0_TEXT_Y, tint, 0.5f, 0);
  ui_draw_framed_rect_r(alpha, px, RESULTS_ROW1_TOP, pw, RESULTS_ROW_HEIGHT);
  ui_draw_framed_rect_r(alpha, px, RESULTS_ROW2_TOP, pw, RESULTS_ROW_HEIGHT);
  ui_draw_framed_rect_r(alpha, px, RESULTS_ROW3_TOP, pw, RESULTS_ROW_HEIGHT);
  top = 100.0f;
common:
  kind = g_menu_multiplayer_kind;
  if (kind != 0 && g_menu_69cc48 >= 3) {
    if (kind == 1) {
      ui_rt2d_text_draw(0xa9, 595.0f, 107.0f, tint, 0.5f, 1);
      ui_rt2d_text_draw(0xab, 595.0f, 119.0f, tint, 0.5f, 1);
    }
    if (kind == 2) {
      ui_rt2d_text_draw(0xaa, 595.0f, 107.0f, tint, 0.5f, 1);
      ui_rt2d_text_draw(0xab, 595.0f, 119.0f, tint, 0.5f, 1);
    }
  }
  else if (g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_TIME_TRIAL_2) {
    y_a = top + float_7_0;
    ui_rt2d_text_draw(0xc1, 270.0f, y_a, tint, 0.5f, 2);
    y_b = top + k_f_19;
    ui_rt2d_text_draw(0x152, 270.0f, y_b, tint, 0.5f, 2);
    ui_rt2d_text_draw(0xa4, 400.0f, y_a, tint, 0.5f, 2);
    ui_rt2d_text_draw(0x152, 400.0f, y_b, tint, 0.5f, 2);
    ui_rt2d_text_draw(0x153, 590.0f, top + k_f_12, tint, 0.5f, 1);
  }
  else if (g_event_type == EVENT_MULTIPLAYER_6 || g_event_type == EVENT_MULTIPLAYER_7 || g_event_type == EVENT_MULTIPLAYER_8) {
    ui_rt2d_text_draw(0x6f, 590.0f, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 1);
  }
  else {
    ui_rt2d_text_draw(0x253, 590.0f, RESULTS_ROW0_TEXT_Y, tint, 0.7f, 1);
  }
  rows = 4;
  prev_team = -1;
  py = 148.0f;
  team_run = 1;
  if (game_mode == 8) {
    rows = 1;
  }
  index = 0;
  if (rows > 0) {
    do {
      if (index != 0 && g_menu_career_race_active != 0) {
        goto next_row;
      }
      pidx = g_results_screen_69d1f8[index];
      if (g_player_slots[pidx].controller <= -1) {
        goto next_row;
      }
      if (g_team_mode != 0) {
        if (g_player_slots[pidx].state == prev_team) {
          py = py - float_50_0;
          tex = team_color_names(g_player_slots[pidx].team);
          off = (float)(team_run * 0x24 + 8);
          px = off + px;
          draw_sprite_uv(tex, px, py, 32.0f, 32.0f, white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
          px = px - off;
          team_run = team_run + 1;
          py = py + float_50_0;
          goto next_row;
        }
        prev_team = g_player_slots[pidx].state;
        team_run = 1;
      }
      slot_color = (alpha << 24) | 0x92312e;
      if (!EVENT_IS_TIME_TRIAL(g_event_type)) {
        ui_draw_framed_rect_r(alpha, px, py, pw, 26.0f);
      }
      py = py + k_f_2;
      tex = team_color_names(g_player_slots[g_results_screen_69d1f8[index]].team);
      if (game_mode == 8) {
        tex = team_color_names(g_player_slots[0].team);
      }
      px = px + k_f_8;
      py = py - k_f_4;
      if (!EVENT_IS_TIME_TRIAL(g_event_type) && g_results_screen_69d1f8[index] != -1) {
        draw_sprite_uv(tex, px, py, 32.0f, 32.0f, white, 0.0f, 1.0f, 0.0f, 1.0f, 1, 1);
      }
      px = px - k_f_8;
      py = py + k_f_2;
      switch (g_menu_69cc48) {
      case 0:
      case 1:
        if (index == 0) {
          value = rank0;
          label = 0x70;
        }
        else if (index == 1) {
          value = rank1;
          label = 0x71;
        }
        else if (index == 2) {
          a = player_get_points(g_results_screen_69d1f8[1]);
          b = player_get_points(g_results_screen_69d1f8[2]);
          if (b == a) {
            value = rank1;
            label = 0x71;
          }
          else {
            value = rank2;
            label = 0x72;
          }
        }
        else if (index == 3) {
          a = player_get_points(g_results_screen_69d1f8[3]);
          b = player_get_points(g_results_screen_69d1f8[1]);
          if (a == b) {
            value = rank1;
            label = 0x71;
          }
          else {
            a = player_get_points(g_results_screen_69d1f8[3]);
            b = player_get_points(g_results_screen_69d1f8[2]);
            if (a == b) {
              value = rank2;
              label = 0x72;
            }
            else {
              value = 0;
              label = 0x73;
            }
          }
        }
        if (index <= 3) {
          sprintf(rank_text, g_frontend_5de9b4, value);
        }
        if (game_mode == 4 || game_mode == 7) {
          label = index + 0x70;
        }
        if (g_team_mode != 0 && index != 0) {
          sprintf(text, PTR_s_ppearance___edge__normal_normsca_00646e30_2_005dec2c);
          label = 0x71;
          sprintf(rank_text, g_frontend_5de9b4, rank1);
        }
        if (g_event_type == EVENT_MULTIPLAYER_6 || g_event_type == EVENT_MULTIPLAYER_7 || g_event_type == EVENT_MULTIPLAYER_8) {
          y_a = py + k_f_15;
          ui_rt2d_text_draw(label, px + k_f_140, y_a, tint, 0.6f, 1);
          text_draw_plain(rank_text, px + k_f_220, y_a, tint, 0.6f, 2);
        }
        else {
          if (game_mode == 10 || game_mode == 8 || game_mode == 5) {
            label = (g_car_7ef7b0 != 0) ? 0x70 : 0x226;
          }
          ui_rt2d_text_draw(label, px + k_f_220, py + k_f_15, tint, 0.6f, 1);
        }
        break;
      case 3:
      case 4:
      case 5:
        if (g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_TIME_TRIAL_2) {
          g_results_screen_69d288 = 0;
          if (time_is_better_than_best() != 0 && g_menu_career_race_active == 0) {
            best_time_store_previous();
            g_results_screen_69d288 = 1;
          }
          g_results_screen_69d210 = time_get_minutes(0);
          g_results_screen_69d208 = time_get_seconds(0);
          g_results_screen_69d674 = (int)(time_get_fraction(0) * k_f_100);
          g_results_screen_69d214 = time_get_minutes(1);
          g_results_screen_69d20c = time_get_seconds(1);
          g_results_screen_69d678 = (int)(time_get_fraction(1) * k_f_100);
          goto time_table;
        }
        if (g_event_type != EVENT_MULTIPLAYER_6 && g_event_type != EVENT_MULTIPLAYER_7 && g_event_type != EVENT_MULTIPLAYER_8) {
          break;
        }
        if (g_menu_multiplayer_kind == 2) {
          value = player_get_stat_8b90(g_results_screen_69d1f8[index]);
        }
        else {
          value = player_get_wins(g_results_screen_69d1f8[index]);
        }
        sprintf(text, g_frontend_5de9b4, value);
        if (g_menu_multiplayer_kind == 0) {
          text_draw_plain(text, (pw + px) - k_f_14, py + k_f_15, tint, 0.6f, 1);
          goto stat_columns;
        }
        if (g_menu_multiplayer_kind == 1) {
          value = g_menu_60ad5c;
        }
        else if (g_menu_multiplayer_kind == 2) {
          value = g_menu_60ad60;
        }
        sprintf(small_text, g_frontend_5de9b4, value);
        x_a = (pw + px) - float_5_0;
        text_draw_plain(small_text, x_a, py + k_f_19, tint, 0.5f, 1);
        w_a = (float)ui_rt2d_text_width(small_text, 0.5f);
        text_draw_plain(g_frontend_5dd59c, x_a - w_a, py + k_f_13, tint, 0.5f, 1);
        w_b = (float)ui_rt2d_text_width(g_frontend_5dd59c, 0.5f);
        star_off = (float)(ui_rt2d_text_width(g_frontend_5dd59c, 0.5f) * k_f_0_5 + w_a);
        text_draw_plain(text, (x_a - (float)(w_b + w_a)) - k_f_2, py + k_f_12, tint, 0.5f, 1);
        star_flag = 0;
        if (g_tutorial_69d2a8 == g_results_screen_69d1f8[index] + 1) {
          goto star_update;
        }
        goto stat_columns;
      }
      goto row_done;
time_table:
      sprintf(text, g_frontend_5de9b4, 1);
      text_draw_plain(text, px + float_10_0, top + k_f_45, tint, 0.7f, 0);
      ui_get_triple_r(1, 0, &tr[0].centi, &tr[0].sec, &tr[0].min);
      ui_format_number_string(tr[0].sec, tr[0].centi * k_f_100, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, tr[0].min, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      y_a = top + k_f_47;
      text_draw_plain(text, 270.0f, y_a, tint, 0.6f, 2);
      ui_get_triple_r(0, 0, &tr[1].centi, &tr[1].sec, &tr[1].min);
      ui_format_number_string(tr[1].sec, tr[1].centi * k_f_100, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, tr[1].min, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      text_draw_plain(text, 400.0f, y_a, tint, 0.6f, 2);
      ui_compare_sign((int)((float)((tr[0].min * 0x3c + tr[0].sec) * 100) + tr[0].centi * k_f_100),
                      (int)((float)((tr[1].min * 0x3c + tr[1].sec) * 100) + tr[1].centi * k_f_100), &cmp.sign, &cmp.v1, &cmp.v2, (int *)&cmp.fv3);
      goto sign_row_1;
sign_row_1:
      ui_format_number_string(cmp.v2, cmp.fv3, sec_text, centi_text);
      if (cmp.sign == 2) {
        sprintf(text, g_frontend_5dec10, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      else {
        sprintf(text, s__s_d_s_s_s_s_005debf8, cmp.sign == 1 ? g_results_screen_5dec08 : g_results_screen_5debf4, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      text_draw_plain(text, 590.0f, y_a, tint, 0.6f, 1);
      sprintf(text, g_frontend_5de9b4, 2);
      text_draw_plain(text, px + float_10_0, top + k_f_77, tint, 0.7f, 0);
      ui_get_triple_r(1, 1, &tr[2].centi, &tr[2].sec, &tr[2].min);
      ui_format_number_string(tr[2].sec, tr[2].centi * k_f_100, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, tr[2].min, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      y_a = top + k_f_79;
      text_draw_plain(text, 270.0f, y_a, tint, 0.6f, 2);
      ui_get_triple_r(0, 1, &tr[3].centi, &tr[3].sec, &tr[3].min);
      ui_format_number_string(tr[3].sec, tr[3].centi * k_f_100, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, tr[3].min, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      text_draw_plain(text, 400.0f, y_a, tint, 0.6f, 2);
      ui_compare_sign((int)((float)((tr[2].min * 0x3c + tr[2].sec) * 100) + tr[2].centi * k_f_100),
                      (int)((float)((tr[3].min * 0x3c + tr[3].sec) * 100) + tr[3].centi * k_f_100), &cmp.sign, &cmp.v1, &cmp.v2, (int *)&cmp.fv3);
      ui_format_number_string(cmp.v2, cmp.fv3, sec_text, centi_text);
      if (cmp.sign == 2) {
        sprintf(text, g_frontend_5dec10, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      else {
        sprintf(text, s__s_d_s_s_s_s_005debf8, cmp.sign == 1 ? g_results_screen_5dec08 : g_results_screen_5debf4, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      text_draw_plain(text, 590.0f, y_a, tint, 0.6f, 1);
      ui_rt2d_text_draw(0x154, px + float_10_0, top + k_f_109, tint, 0.7f, 0);
      ui_format_number_string(g_results_screen_69d20c, (float)g_results_screen_69d678, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, g_results_screen_69d214, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      y_a = top + k_f_111;
      text_draw_plain(text, 270.0f, y_a, tint, 0.6f, 2);
      ui_format_number_string(g_results_screen_69d208, (float)g_results_screen_69d674, sec_text, centi_text);
      sprintf(text, g_frontend_5dec10, g_results_screen_69d210, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      text_draw_plain(text, 400.0f, y_a, tint, 0.6f, 2);
      ui_compare_sign((g_results_screen_69d214 * 0x3c + g_results_screen_69d20c) * 100 + g_results_screen_69d678,
                      (g_results_screen_69d210 * 0x3c + g_results_screen_69d208) * 100 + g_results_screen_69d674,
                      &cmp.sign, &cmp.v1, &cmp.v2, (int *)&cmp.fv3);
      ui_format_number_string(cmp.v2, cmp.fv3, sec_text, centi_text);
      if (cmp.sign == 2) {
        sprintf(text, g_frontend_5dec10, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      else {
        sprintf(text, s__s_d_s_s_s_s_005debf8, cmp.sign == 1 ? g_results_screen_5dec08 : g_results_screen_5debf4, cmp.v1, g_frontend_5dec1c, sec_text, g_frontend_5dec1c, centi_text);
      }
      text_draw_plain(text, 590.0f, y_a, tint, 0.6f, 1);
      if (g_results_screen_69d288 == 0) {
        goto row_done;
      }
      star_off = (float)(ui_rt2d_text_width(text, 0.6f) * k_f_0_5 + k_f_8);
      star_flag = 1;
star_update:
      g_results_screen_69cbdc = g_frame_seconds + g_results_screen_69cbdc;
      if (g_results_screen_69cbdc > k_f_0_075) {
        if (g_frontend_69d31c < k_f_359) {
          g_frontend_69d31c = g_frontend_69d31c + k_f_45;
        }
        else {
          g_frontend_69d31c = 359.0f;
          if (g_results_screen_69cbdc <= k_f_0_75) {
            goto star_loop;
          }
          g_frontend_69d31c = 0.0f;
        }
        g_results_screen_69cbdc = 0.0f;
      }
star_loop:
      star_count = (int)(g_frontend_69d31c * k_f_0_022222223);
      radius = 38.0f;
      if (star_flag == 0) {
        radius = 26.0f;
      }
      star_index = 0;
      if (star_count >= 0) {
        do {
          if (g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_TIME_TRIAL_2) {
            cur_cx = 392.0f;
            cur_cy = ((top + k_f_13) - float_7_0) + k_f_96;
          }
          else {
            cur_cx = (((pw + px) - float_5_0) - star_off) - float_7_0;
            cur_cy = (py + k_f_13) - float_7_0;
          }
          angle = (float)star_index * k_f_45;
          cur_cx = (float)(sin(degrees_to_radians(angle)) * (double)radius * k_f_1_4660155e13 + cur_cx);
          cy_term = cos(degrees_to_radians(angle)) * (double)radius + cur_cy;
          star_sx = (float)((double)screen_width() * cur_cx * k_f_0_0015625);
          star_sy = (float)((double)screen_height() * cy_term * k_f_0_0020833334);
          star_w = (float)screen_width() * k_f_0_025;
          star_h = (float)screen_height() * k_f_0_033333335;
          slot_color = (slot_color & 0xff000000) | 0x1d94f7;
          draw_sprite(find_texture_dict_c(g_frontend_5debd0), star_sx, star_sy, star_w, star_h, alpha << 24, 1);
          star_sx = star_sx - k_f_2;
          star_sy = star_sy - k_f_2;
          draw_sprite(find_texture_dict_c(g_frontend_5debd0), star_sx, star_sy, star_w, star_h, slot_color, 1);
          star_index = star_index + 1;
        } while (star_index <= star_count);
      }
      if (star_flag != 0) {
        goto row_done;
      }
stat_columns:
      sprintf(text, g_frontend_5de9b4, round_get_round_652dbc());
      y_a = py + k_f_15;
      text_draw_plain(text, px + k_f_155, y_a, tint, 0.6f, 2);
      stat_rows = g_team_mode != 0 ? 2 : nplayers;
      stat_x = 0;
      stat_i = 0;
      do {
        if (stat_i < stat_rows) {
          sprintf(text, g_frontend_5de9b4, player_get_stat(g_results_screen_69d1f8[index], stat_i));
          text_draw_plain(text, (float)stat_x + px + k_f_225, y_a, tint, 0.6f, 2);
        }
        stat_x = stat_x + 0x46;
        stat_i = stat_i + 1;
      } while (stat_x < 0x118);
row_done:
      py = py + k_f_48;
next_row:
      index = index + 1;
    } while (index < rows);
  }
  if (show_tutorial != 0) {
    bar_alpha = alpha | (rows << 8);
    ui_draw_framed_rect_r(alpha, 60.0f, 340.0f, 520.0f, 60.0f);
    scroll = ((float)g_tutorial_display_ticks - k_f_640) - k_f_1000;
    if (scroll < k_f_100) {
      if (scroll <= k_f_n1500) {
        scroll = scroll + k_f_1600;
      }
      else {
        scroll = 100.0f;
      }
    }
    line = 0;
    if (g_tutorial_line_count > 0) {
      do {
        id = g_tutorial_line_entries[line];
        line_end = (float)(text_width(id, 0.6f) + scroll);
        if (line_end >= k_f_60) {
          if (scroll < k_f_60) {
            dat = dat_get_string(id);
            clip_w = k_f_60 - scroll;
            clip[0] = (char)dat[1];
            clip[1] = 0;
            star_off = 0.0f;
            j = 1;
            if (k_f_0 < clip_w) {
              do {
                star_off = (float)(ui_rt2d_text_width(clip, 0.6f) + star_off);
                clip[0] = (char)dat[j + 1];
                j = j + 1;
              } while (star_off < clip_w);
            }
            pos = 0;
            if (j < *dat + 1) {
              do {
                clip[pos] = (char)dat[j];
                pos = pos + 1;
                j = j + 1;
              } while (j < *dat + 1);
            }
            clip[pos] = 0;
            text_draw_plain(clip, (scroll - k_f_60) + star_off + k_f_60, 376.0f, tint, 0.6f, 0);
            goto line_advance;
          }
          if (scroll <= k_f_580) {
            if (line_end <= k_f_580) {
              ui_rt2d_text_draw(id, scroll, 376.0f, tint, 0.6f, 0);
            }
            else {
              dat = dat_get_string(id);
              len = *dat;
              clip[1] = 0;
              if (k_f_580 < line_end) {
                do {
                  if (len == 0) {
                    break;
                  }
                  clip[0] = (char)dat[len];
                  len = len - 1;
                  line_end = (float)(line_end - ui_rt2d_text_width(clip, 0.6f));
                } while (k_f_580 < line_end);
              }
              j = 0;
              if (len > 0) {
                do {
                  dat = dat + 1;
                  clip[j] = (char)*dat;
                  j = j + 1;
                } while (j < len);
              }
              clip[j] = 0;
              text_draw_plain(clip, scroll, 376.0f, tint, 0.6f, 0);
            }
line_advance:
            scroll = (float)(text_width(id, 0.6f) + scroll + k_f_8);
          }
        }
        else {
          scroll = (float)(text_width(id, 0.6f) + scroll + k_f_8);
        }
        line = line + 1;
      } while (line < g_tutorial_line_count);
    }
    slot_color = (alpha << 24) | 0x146ef0;
    bar_color = slot_color;
    draw_rect(60.0f, 340.0f, 10.0f, 60.0f, bar_color);
    gfx_draw_rect_2d_four_alpha(70.0f, 340.0f, 30.0f, 60.0f, bar_color, bar_alpha, 0, bar_alpha, 0);
    gfx_draw_rect_2d_four_alpha(540.0f, 340.0f, 30.0f, 60.0f, bar_color, 0, bar_alpha, 0, bar_alpha);
    draw_rect(570.0f, 340.0f, 10.0f, 60.0f, bar_color);
    gfx_draw_rect_2d_four_alpha(40.0f, 340.0f, 20.0f, 60.0f, bar_color, 0, bar_alpha, 0, bar_alpha);
    gfx_draw_rect_2d_four_alpha(580.0f, 340.0f, 20.0f, 60.0f, bar_color, bar_alpha, 0, bar_alpha, 0);
    draw_rect(0.0f, 344.0f, 640.0f, 2.0f, tint);
    title_w = (float)text_width(0x74, 0.6f);
    page_w = (float)text_width(g_tutorial_page_index + 0xad, 0.6f);
    if (g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_TIME_TRIAL_2) {
      title_w = 0.0f;
      page_w = (float)text_width(0xc4, 0.6f);
    }
    ui_draw_framed_rect_r(alpha, 80.0f, 344.0f, (float)(page_w + title_w) + k_f_4, 16.0f);
    if (g_event_type == EVENT_TIME_TRIAL_0 || g_event_type == EVENT_TIME_TRIAL_1 || g_event_type == EVENT_TIME_TRIAL_2) {
      ui_rt2d_text_draw(0xc4, 82.0f, 351.0f, tint, 0.6f, 0);
    }
    else {
      ui_rt2d_text_draw(0x74, 82.0f, 351.0f, tint, 0.6f, 0);
      ui_rt2d_text_draw(g_tutorial_page_index + 0xad, title_w + k_f_82, 351.0f, tint, 0.6f, 0);
    }
  }
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)1);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)1);
  if (g_menu_69cc48 <= 3) {
    top = (float)(g_tutorial_69d2c8 + -0x127) + k_f_8;
    bar_alpha = g_frontend_69c99c;
    if (bar_alpha > 0xa0) {
      bar_alpha = 0xa0;
    }
    gfx_draw_rect_2d_four_alpha(top, 108.0f, 256.0f, 220.0f, 0, bar_alpha, bar_alpha, bar_alpha, bar_alpha);
    draw_rect(top - k_f_8, 100.0f, 256.0f, 220.0f, slot_color);
  }
}

/* ui_draw_framed_rect_r: C -> original ui_draw_framed_rect (registers eax,s0,s1,s2,s3) */
__declspec(naked) void ui_draw_framed_rect_r(unsigned int p0, float p1, float p2, float p3, float p4)
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

/* ui_get_triple_r: C -> original ui_get_triple (registers eax,ecx,s0,s1,s2) */
__declspec(naked) void ui_get_triple_r(int p0, int p1, float * p2, int * p3, int * p4)
{
    __asm {
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        mov eax, dword ptr [esp + 16]
        mov ecx, dword ptr [esp + 20]
        call ui_get_triple
        add esp, 12
        ret
    }
}
