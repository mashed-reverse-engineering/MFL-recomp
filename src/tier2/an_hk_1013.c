#include "mfl_tier2.h"
/*00434100*/ extern char *__cdecl menu_icon_texture_lookup(undefined4 p0);
/*0043cda0*/ extern int menu_texture_dict_load();
/*004824a0*/ extern int draw_rect();
/*004832d0*/ extern int draw_sprite_uv(int *param_3, float param_4, float param_5, float param_6, float param_7, uint param_8, RwReal param_9, RwReal param_10, RwReal param_11, RwReal param_12, int param_13, int param_14);
/*00484180*/ extern int ui_draw_sprite_oriented(int texture, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 colour, int p6, undefined4 p7, undefined4 p8);
/*004b2b48*/ extern int _ftol();
/*005dd33c*/ extern float k_f_4;
/*005dd634*/ extern float k_f_2;
/*005dec48*/ extern float k_f_36;
/*005dec4c*/ extern float k_f_272;
/*005dec50*/ extern float k_f_182;
/*00607e68*/ extern undefined g_menu_layout_607e68;
/*00608138*/ extern undefined g_menu_layout_608138;
/*00608408*/ extern undefined g_menu_layout_608408;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc30*/ extern undefined4 g_menu_layout_69cc30;
/*0069d664*/ extern undefined4 g_hud_69d664;
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;
/*007eebc0*/ extern struct GameState g_game_state;
/*008c86c0*/ extern undefined4 g_menu_layout_8c86c0;
/*007ef7f8*/ extern float g_menu_wave_time;
/* menu_layout_draw_b */

/* "builds and draws the 6 entry menu layout (variant b)" */

void menu_layout_draw_b(int alpha,int compact)

{
  float fbits1;
  float fbits2;
  float fbits3;
  undefined4 icon_variant;
  bool event_handled;
  uint wave_phase;
  uint phase_ahead;
  int sprite_texture;
  int *icon_tex;
  int fade_a_or_event;
  int fade_b_or_flag;
  int slot;
  float icon_x;
  float x_arg;
  float y_arg;
  float w_arg;
  float h_arg;
  RwReal uv_0;
  RwReal uv_1;
  RwReal uv_2;
  RwReal uv_3;
  RwReal uv_one;
  struct {
    undefined4 colour;
    int border_colour;
  } frame;
  struct {
    float rect_x;
    float rect_y;
    float rect_w;
    float rect_h;
    undefined4 icon_cycle [16];
  } frame2;
  
  frame2.icon_cycle[3] = 3;
  frame2.icon_cycle[9] = 3;
  frame2.icon_cycle[0xf] = 3;
  frame2.icon_cycle[0] = 0;
  frame2.icon_cycle[1] = 1;
  frame2.icon_cycle[2] = 2;
  frame2.icon_cycle[4] = 4;
  frame2.icon_cycle[5] = 5;
  frame2.icon_cycle[6] = 0;
  frame2.icon_cycle[7] = 1;
  frame2.icon_cycle[8] = 2;
  frame2.icon_cycle[10] = 4;
  frame2.icon_cycle[0xb] = 5;
  frame2.icon_cycle[0xc] = 0;
  frame2.icon_cycle[0xd] = 1;
  frame2.icon_cycle[0xe] = 2;
  frame.colour = 0xffffff;
  frame2.rect_x = 330.0;
  frame2.rect_y = 96.0;
  frame2.rect_w = 270.0;
  frame2.rect_h = 180.0;
  if (compact != 0) {
    frame2.rect_x = 321.0;
    frame2.rect_y = 65.0;
    frame2.rect_w = 255.0;
    frame2.rect_h = 200.0;
  }
  wave_phase = ((int)(g_menu_wave_time * 60.0f));
  wave_phase = wave_phase & 0x1ff;
  if (wave_phase == 0x1e0) {
    g_menu_layout_69cc30 = g_menu_layout_69cc30 + 2;
  }
  else if (wave_phase == 0xe0) {
    g_hud_69d664 = g_hud_69d664 + 2;
  }
  if ((g_hud_69d664 & 1) == 0) {
    g_hud_69d664 = g_hud_69d664 + 1;
  }
  if (wave_phase < 0xe0) {
    fade_b_or_flag = 0;
LAB_0043d357:
    fade_a_or_event = 0xff;
  }
  else if (wave_phase < 0x100) {
    fade_b_or_flag = wave_phase - 0xe0;
    if (fade_b_or_flag < 0x10) {
      fade_b_or_flag = fade_b_or_flag * 0x10;
      goto LAB_0043d357;
    }
    fade_a_or_event = (0x1f - fade_b_or_flag) * 0x10;
    fade_b_or_flag = 0xff;
  }
  else if (wave_phase < 0x1e0) {
    fade_a_or_event = 0;
    fade_b_or_flag = 0xff;
  }
  else {
    fade_a_or_event = wave_phase - 0x1e0;
    if (0xf < fade_a_or_event) {
      fade_b_or_flag = (0x1f - fade_a_or_event) * 0x10;
      goto LAB_0043d357;
    }
    fade_a_or_event = fade_a_or_event * 0x10;
    fade_b_or_flag = 0xff;
  }
  icon_variant = frame2.icon_cycle[g_hud_69d664 & 0xf];
  if (fade_a_or_event != 0) {
    fade_a_or_event = fade_a_or_event * alpha;
    phase_ahead = wave_phase + 0x20;
    frame.colour = CONCAT13(((char)(fade_a_or_event / 0xff) + (char)(fade_a_or_event >> 0x1f)) -
                        (char)((longlong)fade_a_or_event * 0x80808081 >> 0x3f),0xffffff);
    if (0x1ff < phase_ahead) {
      phase_ahead = wave_phase - 0x1e0;
    }
    sprite_texture = menu_texture_dict_load(0);
    ui_draw_sprite_oriented(sprite_texture,*(int *)&frame2.rect_x,*(int *)&frame2.rect_y,*(int *)&frame2.rect_w,*(int *)&frame2.rect_h,frame.colour,(int)(phase_ahead << 8) / 0x120, frame2.icon_cycle[g_menu_layout_69cc30 & 0xf],0);
  }
  if (fade_b_or_flag != 0) {
    fade_b_or_flag = fade_b_or_flag * alpha;
    frame.colour = CONCAT13(((char)(fade_b_or_flag / 0xff) + (char)(fade_b_or_flag >> 0x1f)) -
                        (char)((longlong)fade_b_or_flag * 0x80808081 >> 0x3f),(undefined3)frame.colour);
    sprite_texture = menu_texture_dict_load(1);
    ui_draw_sprite_oriented(sprite_texture,*(int *)&frame2.rect_x,*(int *)&frame2.rect_y,*(int *)&frame2.rect_w,*(int *)&frame2.rect_h,frame.colour,(int)((wave_phase - 0xe0) * 0x100) / 0x120, icon_variant,0);
  }
  if (compact == 0) {
    frame.border_colour = alpha << 0x18;
    frame2.rect_x = frame2.rect_x - k_f_2;
    frame2.rect_y = frame2.rect_y - k_f_2;
    draw_rect(*(int *)&frame2.rect_x,*(int *)&frame2.rect_y,0x40000000,(fbits1 = frame2.rect_h + k_f_4, *(int *)&fbits1),frame.border_colour);
    draw_rect(*(int *)&frame2.rect_x,*(int *)&frame2.rect_y,0x43890000,0x40000000,frame.border_colour);
    frame2.rect_y = frame2.rect_y + k_f_182;
    draw_rect(*(int *)&frame2.rect_x,*(int *)&frame2.rect_y,0x43890000,0x40000000,frame.border_colour);
    draw_rect((fbits2 = frame2.rect_x + k_f_272, *(int *)&fbits2),(fbits3 = frame2.rect_y - k_f_182, *(int *)&fbits3),0x40000000,0x43360000,frame.border_colour);
  }
  frame2.rect_x = 330.0;
  frame2.rect_y = 100.0;
  if (compact != 0) {
    frame2.rect_x = 322.0;
    frame2.rect_y = 76.0;
  }
  compact = g_career_row * 0xc;
  frame2.rect_h = 40.0;
  frame2.rect_w = 40.0;
  frame.colour = CONCAT13((char)alpha,(undefined3)frame.colour);
  if (g_menu_event_column == 5) {
    compact = compact + 8;
    goto LAB_0043d5dc;
  }
  if (g_menu_event_column != 4) {
    if (g_menu_event_column == 8) {
      compact = compact + 8;
      goto LAB_0043d5dc;
    }
    if (g_menu_event_column != 7) {
      if (g_menu_event_column == 0xd) {
        compact = compact + 8;
        goto LAB_0043d5dc;
      }
      if (g_menu_event_column != 0xc) {
        if (g_menu_event_column == 2) {
          compact = compact + 8;
          goto LAB_0043d5dc;
        }
        if (g_menu_event_column != 1) goto LAB_0043d5dc;
      }
    }
  }
  compact = compact + 4;
LAB_0043d5dc:
  /* from here on fade_a_or_event / fade_b_or_flag are reused: event type, layout-table entry (non-zero = slot has an icon) */
  slot = 0;
  g_game_state.mirror_mode = 0;
  fade_a_or_event = g_event_type;
  icon_x = frame2.rect_x;
  do {
    if (((fade_a_or_event == EVENT_CUP_3) || (fade_a_or_event == EVENT_CUP_4)) || (fade_a_or_event == EVENT_CUP_5)) {
      fade_b_or_flag = *(int *)((char *)&g_menu_layout_607e68 + (compact + slot) * 4);
LAB_0043d730:
      if (fade_b_or_flag != 0) {
        if (slot == 3) {
          g_game_state.mirror_mode = 1;
        }
        fade_b_or_flag = 0;
        fade_a_or_event = 1;
        uv_3 = 1.0;
        uv_2 = 0.0;
        uv_1 = 1.0;
        uv_0 = 0.0;
        x_arg = icon_x;
        y_arg = frame2.rect_y;
        w_arg = frame2.rect_w;
        h_arg = frame2.rect_h;
        wave_phase = frame.colour;
        icon_tex = (int *)menu_icon_texture_lookup(slot);
        draw_sprite_uv(icon_tex,x_arg,y_arg,w_arg,h_arg,wave_phase,uv_0,uv_1,uv_2,
                       uv_3,fade_a_or_event,fade_b_or_flag);
        icon_x = frame2.rect_x + k_f_36;
        fade_a_or_event = g_event_type;
        frame2.rect_x = icon_x;
      }
    }
    else {
      event_handled = false;
      if (fade_a_or_event == EVENT_TURBO_TOURNAMENT) {
        if (*(int *)((char *)&g_menu_layout_608138 + (compact + slot) * 4) != 0) {
          if (slot == 3) {
            g_game_state.mirror_mode = 1;
          }
          fade_b_or_flag = 0;
          fade_a_or_event = 1;
          uv_3 = 1.0;
          uv_2 = 0.0;
          uv_1 = 1.0;
          uv_0 = 0.0;
          x_arg = icon_x;
          y_arg = frame2.rect_y;
          w_arg = frame2.rect_w;
          h_arg = frame2.rect_h;
          wave_phase = frame.colour;
          icon_tex = (int *)menu_icon_texture_lookup(slot);
          draw_sprite_uv(icon_tex,x_arg,y_arg,w_arg,h_arg,wave_phase,uv_0,uv_1,uv_2
                         ,uv_3,fade_a_or_event,fade_b_or_flag);
          icon_x = frame2.rect_x + k_f_36;
          fade_a_or_event = g_event_type;
          frame2.rect_x = icon_x;
        }
        event_handled = true;
      }
      if (fade_a_or_event == EVENT_ICE_TOURNAMENT) {
        if (slot == 0) {
          fade_a_or_event = 1;
          uv_one = 1.0;
          uv_3 = 1.0;
          x_arg = icon_x;
          y_arg = frame2.rect_y;
          w_arg = frame2.rect_w;
          h_arg = frame2.rect_h;
          wave_phase = frame.colour;
          uv_0 = 0.0;
          uv_1 = 0.0;
          uv_2 = 0.0;
          icon_tex = (int *)menu_icon_texture_lookup(0);
          draw_sprite_uv(icon_tex,x_arg,y_arg,w_arg,h_arg,wave_phase,uv_0,uv_3,uv_1,
                         uv_one,fade_a_or_event,(int)uv_2);
          fade_a_or_event = g_event_type;
        }
        event_handled = true;
      }
      if (fade_a_or_event == EVENT_NIGHT_TOURNAMENT) {
        if (slot == 0) {
          fade_a_or_event = 1;
          uv_one = 1.0;
          uv_3 = 1.0;
          x_arg = icon_x;
          y_arg = frame2.rect_y;
          w_arg = frame2.rect_w;
          h_arg = frame2.rect_h;
          wave_phase = frame.colour;
          uv_0 = 0.0;
          uv_1 = 0.0;
          uv_2 = 0.0;
          icon_tex = (int *)menu_icon_texture_lookup(1);
          draw_sprite_uv(icon_tex,x_arg,y_arg,w_arg,h_arg,wave_phase,uv_0,uv_3,uv_1,
                         uv_one,fade_a_or_event,(int)uv_2);
          fade_a_or_event = g_event_type;
        }
        event_handled = true;
      }
      if ((fade_a_or_event != EVENT_TRAINING) && (!event_handled)) {
        fade_b_or_flag = *(int *)((char *)&g_menu_layout_608408 + (compact + slot) * 4);
        goto LAB_0043d730;
      }
    }
    slot = slot + 1;
    if (3 < slot) {
      g_menu_layout_8c86c0 = g_menu_layout_8c86c0 + 0x20;
      return;
    }
  } while( true );
}


