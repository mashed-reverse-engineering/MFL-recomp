#include "mfl_tier2.h"
/*0040c9f0*/ extern undefined __cdecl players_get_wins(void *p0);
/*0040cad0*/ extern int players_get_points();
/*0040cc40*/ extern int player_get_stat_8b90();
/*0040cc50*/ extern int player_get_wins();
/*0040fdb0*/ extern int get_player_count();
/*004136d0*/ extern undefined __cdecl race_start_setup(undefined4 p0);
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00427730*/ extern int player_stat_get_turbo_used_team();
/*004277b0*/ extern int player_stat_get_aggression_team();
/*00427830*/ extern int __cdecl player_stat_get_kills(undefined4 p0);
/*004278c0*/ extern int player_stat_get_wall_hits_team();
/*00427940*/ extern int __cdecl player_stat_get_unforced_errors(undefined4 p0);
/*004279d0*/ extern int player_stat_get_turbo_used_total_team();
/*00427a50*/ extern int player_stat_get_aggression_total_team();
/*00427ad0*/ extern int __cdecl player_stat_get_kills_total(undefined4 p0);
/*00427b60*/ extern int player_stat_get_wall_hits_total_team();
/*00427be0*/ extern int __cdecl player_stat_get_unforced_errors_total(undefined4 p0);
/*00427c70*/ extern int player_stat_get_place_team();
/*00427d30*/ extern int player_stat_get_team_l();
/*00427de0*/ extern undefined __cdecl player_stat_get_by_kind(void *p0, undefined4 p1, undefined4 p2);
/*0042c8e0*/ extern double text_width();
/*0042e960*/ extern int intro_banner_set_active();
/*0042e990*/ extern int time_is_better_than_best();
/*00431b60*/ extern int ui_format_number_999();
/*00431c20*/ extern int tutorial_text_format_ratio();
/*00431d70*/ extern int tutorial_state_reset();
/*00432060*/ extern int team_names_string_build();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd7e8*/ extern float k_f_60;
/*005ddb48*/ extern float k_f_16;
/*005ddb50*/ extern float k_f_8;
/*005ddc28*/ extern float k_f_0;
/*005de3e4*/ extern float k_f_40;
/*0060ad5c*/ extern undefined4 g_menu_60ad5c;
/*0060ad60*/ extern undefined4 g_menu_60ad60;
/*0060aeec*/ extern void *g_tutorial_pages_event;
/*0060af44*/ extern void *g_tutorial_pages_basic;
/*0069c998*/ extern undefined4 g_menu_panel_state_69c998;
/*0069c99c*/ extern int g_frontend_69c99c;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cbe0*/ extern undefined4 g_tutorial_page_index;
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cc48*/ extern undefined4 g_menu_69cc48;
/*0069d014*/ extern undefined4 g_tutorial_ranked_players[0x1000];
/*0069d0e8*/ extern float g_tutorial_69d0e8;
/*0069d0ec*/ extern float g_tutorial_69d0ec;
/*0069d0f0*/ extern float g_tutorial_69d0f0;
/*0069d0f4*/ extern float g_tutorial_69d0f4;
/*0069d100*/ extern undefined4 g_tutorial_line_count;
/*0069d104*/ extern undefined4 g_tutorial_display_ticks;
/*0069d108*/ extern undefined4 g_tutorial_line_entries;
/*0069d10c*/ extern undefined4 g_tutorial_69d10c;
/*0069d158*/ extern float g_tutorial_line_width;
/*0069d15c*/ extern float g_tutorial_69d15c;
/*0069d160*/ extern undefined4 g_tutorial_reset_requested;
/*0069d21c*/ extern undefined4 g_menu_multiplayer_kind;
/*0069d2a8*/ extern undefined4 g_tutorial_69d2a8;
/*0069d2c8*/ extern undefined4 g_tutorial_69d2c8;
/*0069d67c*/ extern float g_tutorial_69d67c;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef7ec*/ extern float g_frame_seconds;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* tutorial_text_update */

/* (was: "runs game_tick for the frame time, then builds the instruction text lines for event types 0..2 using text_width (inferred)")
   Calls race_start_setup(0x32) ceil(g_frame_ticks / 50) times (not game_tick), then, while the menu panel state is 1 and no line is pending,
   interprets the current tutorial page script (the basic pages for event types 0..2 = time trials, the event pages for every other type) and
   collects the text lines with their widths (text_width at scale 0.7). */

void tutorial_text_update(void)

{
  uint prev_state;
  int text_id_a;
  int text_id_b;
  int opcode_or_text;
  int *page_script;
  int *ranked_player;
  int stat_value;
  double text_w;
  undefined4 ratio_flag_arg;
  struct {
    int *script_cursor;
    int ratio_ref;
    undefined4 ratio_flag;
    int rows_left;
  } frame;
  
  if (0 < (int)g_frame_ticks) {
    opcode_or_text = (g_frame_ticks - 1U) / 0x32 + 1;
    do {
      race_start_setup(0x32);
      opcode_or_text = opcode_or_text + -1;
    } while (opcode_or_text != 0);
  }
  if (g_menu_panel_state_69c998 == 1) {
    if (((g_event_type == EVENT_TIME_TRIAL_0) || (g_event_type == EVENT_TIME_TRIAL_1)) || (g_event_type == EVENT_TIME_TRIAL_2)) {
      intro_banner_set_active(0);
    }
    if (g_tutorial_reset_requested != 0) {
      tutorial_state_reset();
      g_tutorial_reset_requested = 0;
    }
    if (g_tutorial_line_count == 0) {
      g_tutorial_display_ticks = 0x8e8;
      if (((g_event_type == EVENT_TIME_TRIAL_0) || (g_event_type == EVENT_TIME_TRIAL_1)) || (g_event_type == EVENT_TIME_TRIAL_2)) {
        page_script = (int *)(&g_tutorial_pages_basic)[g_tutorial_page_index];
      }
      else {
        page_script = (int *)(&g_tutorial_pages_event)[g_tutorial_page_index];
      }
      opcode_or_text = *page_script;
      stat_value = frame.rows_left;
      if (opcode_or_text != -1) {
LAB_004322cd:
        frame.script_cursor = page_script;
        switch(opcode_or_text) {
        case 1:
          frame.script_cursor = page_script + 1;
          if (((g_event_type == EVENT_TIME_TRIAL_0) || (g_event_type == EVENT_TIME_TRIAL_1)) || (g_event_type == EVENT_TIME_TRIAL_2)) {
LAB_00432313:
            frame.script_cursor = page_script + 1;
            (&g_tutorial_line_entries)[g_tutorial_line_count] = *frame.script_cursor;
            g_tutorial_line_count = g_tutorial_line_count + 1;
            text_w = (double)text_width(*frame.script_cursor,0x3f333333);
            g_tutorial_line_width =
                 (float)(text_w + (double)g_tutorial_line_width + (double)k_f_8);
          }
          break;
        case 2:
          frame.script_cursor = page_script + 1;
          player_stat_get_by_kind(g_tutorial_ranked_players,page_script[1],0);
          frame.rows_left = get_player_count();
          if (g_team_mode == 0) {
            if (frame.rows_left < 1) break;
          }
          else {
            frame.rows_left = 2;
          }
          opcode_or_text = 0x84;
          ranked_player = g_tutorial_ranked_players;
          do {
            if (g_team_mode == 0) {
              text_id_b = g_player_slots[*ranked_player].team + 0x7c;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = text_id_b;
              text_w = (double)text_width(text_id_b,0x3f333333);
            }
            else {
              text_id_b = g_player_slots[*ranked_player].state;
              team_names_string_build(text_id_b);
              text_id_b = text_id_b + 0xa7;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = text_id_b;
              text_w = (double)text_width(text_id_b,0x3f333333);
            }
            g_tutorial_line_width =
                 (float)(text_w + (double)g_tutorial_line_width + (double)k_f_8);
            if (*frame.script_cursor == 0) {
              stat_value = player_stat_get_crowns(*ranked_player);
            }
            if (*frame.script_cursor == 1) {
              stat_value = player_stat_get_pants(*ranked_player);
            }
            if (*frame.script_cursor == 4) {
              stat_value = player_stat_get_kills(*ranked_player);
            }
            if (*frame.script_cursor == 6) {
              stat_value = player_stat_get_unforced_errors(*ranked_player);
            }
            if (*frame.script_cursor == 7) {
              stat_value = player_stat_get_crowns_total(*ranked_player);
            }
            if (*frame.script_cursor == 8) {
              stat_value = player_stat_get_pants_total(*ranked_player);
            }
            if (*frame.script_cursor == 0xb) {
              stat_value = player_stat_get_kills_total(*ranked_player);
            }
            if (*frame.script_cursor == 0xd) {
              stat_value = player_stat_get_unforced_errors_total(*ranked_player);
            }
            (&g_tutorial_69d10c)[g_tutorial_line_count] = opcode_or_text;
            ui_format_number_999(stat_value,opcode_or_text);
            text_w = (double)text_width(opcode_or_text,0x3f333333);
            g_tutorial_line_count = g_tutorial_line_count + 2;
            ranked_player = ranked_player + 1;
            opcode_or_text = opcode_or_text + 1;
            frame.rows_left = frame.rows_left + -1;
            g_tutorial_line_width =
                 (float)(text_w + (double)g_tutorial_line_width + (double)k_f_8);
          } while (frame.rows_left != 0);
          goto LAB_00432894;
        case 3:
          opcode_or_text = page_script[1];
          frame.script_cursor = page_script + 1;
          if ((opcode_or_text == 5) || (opcode_or_text == 0xc)) {
            ratio_flag_arg = 1;
          }
          else {
            ratio_flag_arg = 0;
          }
          player_stat_get_by_kind(g_tutorial_ranked_players,opcode_or_text,ratio_flag_arg);
          frame.rows_left = get_player_count();
          if (g_team_mode == 0) {
            if (frame.rows_left < 1) break;
          }
          else {
            frame.rows_left = 2;
          }
          text_id_b = 0x91;
          ranked_player = g_tutorial_ranked_players;
          opcode_or_text = frame.ratio_ref;
          do {
            if (g_team_mode == 0) {
              text_id_a = g_player_slots[*ranked_player].team + 0x7c;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = text_id_a;
              text_w = (double)text_width(text_id_a,0x3f333333);
            }
            else {
              opcode_or_text = g_player_slots[*ranked_player].state;
              team_names_string_build(opcode_or_text);
              opcode_or_text = opcode_or_text + 0xa7;
              (&g_tutorial_line_entries)[g_tutorial_line_count] = opcode_or_text;
              text_w = (double)text_width(opcode_or_text,0x3f333333);
              opcode_or_text = frame.ratio_ref;
            }
            g_tutorial_line_width =
                 (float)(text_w + (double)g_tutorial_line_width + (double)k_f_8);
            if (*frame.script_cursor == 2) {
              opcode_or_text = player_stat_get_place_team(*ranked_player);
              opcode_or_text = opcode_or_text * 2000;
              stat_value = player_stat_get_turbo_used_team(*ranked_player);
              frame.ratio_flag = 0;
              frame.ratio_ref = opcode_or_text;
            }
            if (*frame.script_cursor == 3) {
              opcode_or_text = player_stat_get_aggression_team(g_tutorial_ranked_players[0]);
              stat_value = player_stat_get_aggression_team(*ranked_player);
              frame.ratio_flag = 0;
              frame.ratio_ref = opcode_or_text;
            }
            if (*frame.script_cursor == 5) {
              opcode_or_text = player_stat_get_wall_hits_team(g_tutorial_ranked_players[0]);
              stat_value = player_stat_get_wall_hits_team(*ranked_player);
              frame.ratio_flag = 1;
              frame.ratio_ref = opcode_or_text;
            }
            if (*frame.script_cursor == 9) {
              opcode_or_text = player_stat_get_team_l(*ranked_player);
              opcode_or_text = opcode_or_text * 2000;
              stat_value = player_stat_get_turbo_used_total_team(*ranked_player);
              frame.ratio_flag = 0;
              frame.ratio_ref = opcode_or_text;
            }
            if (*frame.script_cursor == 10) {
              opcode_or_text = player_stat_get_aggression_total_team(g_tutorial_ranked_players[0]);
              stat_value = player_stat_get_aggression_total_team(*ranked_player);
              frame.ratio_flag = 0;
              frame.ratio_ref = opcode_or_text;
            }
            if (*frame.script_cursor == 0xc) {
              opcode_or_text = player_stat_get_wall_hits_total_team(g_tutorial_ranked_players[0]);
              stat_value = player_stat_get_wall_hits_total_team(*ranked_player);
              frame.ratio_flag = 1;
              frame.ratio_ref = opcode_or_text;
            }
            (&g_tutorial_69d10c)[g_tutorial_line_count] = text_id_b;
            tutorial_text_format_ratio(opcode_or_text,stat_value,text_id_b,frame.ratio_flag);
            text_w = (double)text_width(text_id_b,0x3f333333);
            g_tutorial_line_count = g_tutorial_line_count + 2;
            ranked_player = ranked_player + 1;
            text_id_b = text_id_b + 1;
            frame.rows_left = frame.rows_left + -1;
            g_tutorial_line_width =
                 (float)(text_w + (double)g_tutorial_line_width + (double)k_f_8);
          } while (frame.rows_left != 0);
LAB_00432894:
          frame.script_cursor = page_script + 1;
          break;
        case 4:
          goto switchD_004322dc_caseD_4;
        case 5:
          opcode_or_text = time_is_better_than_best();
          if (opcode_or_text != 0) goto LAB_00432313;
          g_tutorial_page_index = g_tutorial_page_index + 1;
          goto switchD_004322dc_caseD_4;
        case 6:
          if (page_script[1] == 6) {
            stat_value = player_stat_get_unforced_errors(0);
          }
          (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x84;
          ui_format_number_999(stat_value,0x84);
          text_w = (double)text_width(0x84,0x3f333333);
          g_tutorial_line_width = (float)(text_w + (double)g_tutorial_line_width);
          (&g_tutorial_69d10c)[g_tutorial_line_count] = 0xc6;
          text_w = (double)text_width(0xc6,0x3f333333);
          goto LAB_00432624;
        case 7:
          if (page_script[1] == 2) {
            stat_value = player_stat_get_turbo_used_team(0);
            (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x91;
            tutorial_text_format_ratio(2000,stat_value,0x91,0);
            text_w = (double)text_width(0x91,0x3f333333);
            g_tutorial_line_width = (float)(text_w + (double)g_tutorial_line_width);
          }
          if (page_script[1] == 5) {
            stat_value = player_stat_get_wall_hits_team(0);
            (&g_tutorial_line_entries)[g_tutorial_line_count] = 0x92;
            tutorial_text_format_ratio(0,stat_value,0x92,1);
            text_w = (double)text_width(0x92,0x3f333333);
            g_tutorial_line_width = (float)(text_w + (double)g_tutorial_line_width);
          }
          (&g_tutorial_69d10c)[g_tutorial_line_count] = 0xc6;
          text_w = (double)text_width(0xc6,0x3f333333);
LAB_00432624:
          frame.script_cursor = page_script + 1;
          g_tutorial_line_count = g_tutorial_line_count + 2;
          g_tutorial_line_width =
               (float)(text_w + (double)g_tutorial_line_width + (double)k_f_16);
        }
        goto switchD_004322dc_default;
      }
    }
    else {
      g_tutorial_display_ticks = g_tutorial_display_ticks + -0xc;
      if ((float)(int)g_tutorial_display_ticks + g_tutorial_line_width < k_f_60) {
        tutorial_state_reset();
      }
    }
LAB_004328e0:
    prev_state = g_menu_69cc48;
    if ((((g_event_type == EVENT_TIME_TRIAL_0) || (g_event_type == EVENT_TIME_TRIAL_1)) || (g_event_type == EVENT_TIME_TRIAL_2)) &&
       ((int)g_menu_69cc48 < 3)) {
      g_tutorial_69d2c8 = 0;
      g_menu_69cc48 = 3;
      g_tutorial_69d67c = 0.0;
      g_tutorial_69d15c = 0.0;
    }
    else if (5 < (int)g_menu_69cc48) goto LAB_00432ab5;
    switch(g_menu_69cc48) {
    case 0:
      g_tutorial_69d67c = (float)g_frontend_69c99c;
      g_tutorial_69d2c8 = 0x15e;
      players_get_points(g_tutorial_ranked_players);
      g_tutorial_69d15c = g_frame_seconds * k_f_3 + g_tutorial_69d15c;
      if (float_1_0 < g_tutorial_69d15c) {
        g_tutorial_69d15c = 1.0;
        g_menu_69cc48 = 1;
      }
      g_tutorial_69d2a8 = 0;
      break;
    case 1:
      g_tutorial_69d67c = (float)g_frontend_69c99c;
      players_get_points(g_tutorial_ranked_players);
      break;
    case 2:
      players_get_points(g_tutorial_ranked_players);
      g_tutorial_69d67c = 0.0;
      g_tutorial_69d15c = 0.0;
      if ((int)g_tutorial_69d2c8 < 0x14) {
        g_tutorial_69d2c8 = 0;
      }
      else {
        g_tutorial_69d2c8 = g_tutorial_69d2c8 + -0x14;
        if (g_tutorial_69d2c8 != 0) break;
      }
      g_menu_69cc48 = 3;
      g_tutorial_69d0e8 = g_tutorial_69d0e8 + k_f_40;
      g_tutorial_69d0ec = g_tutorial_69d0ec + k_f_40;
      g_tutorial_69d0f0 = g_tutorial_69d0f0 + k_f_40;
      g_tutorial_69d0f4 = g_tutorial_69d0f4 + k_f_40;
      break;
    case 3:
      players_get_wins(g_tutorial_ranked_players);
      g_tutorial_69d15c = 1.0;
      g_tutorial_69d67c = 255.0;
      g_menu_69cc48 = 4;
      break;
    case 4:
    case 5:
      players_get_wins(g_tutorial_ranked_players);
    }
  }
  else {
    g_tutorial_69d15c = g_tutorial_69d15c - g_frame_seconds * k_f_4;
    if (g_tutorial_69d15c < k_f_0) {
      g_tutorial_69d15c = 0.0;
    }
  }
LAB_00432ab5:
  opcode_or_text = g_tutorial_69d2a8;
  if (g_menu_multiplayer_kind == 1) {
    stat_value = 0;
    do {
      text_id_b = player_get_wins(stat_value);
      stat_value = stat_value + 1;
      opcode_or_text = stat_value;
      if ((int)g_menu_60ad5c <= text_id_b) break;
      opcode_or_text = g_tutorial_69d2a8;
    } while (stat_value < 4);
  }
  g_tutorial_69d2a8 = opcode_or_text;
  opcode_or_text = g_tutorial_69d2a8;
  if (g_menu_multiplayer_kind == 2) {
    opcode_or_text = 0;
    while( true ) {
      stat_value = player_get_stat_8b90(opcode_or_text);
      opcode_or_text = opcode_or_text + 1;
      if ((int)g_menu_60ad60 <= stat_value) break;
      if (3 < opcode_or_text) {
        return;
      }
    }
  }
  g_tutorial_69d2a8 = opcode_or_text;
  return;
switchD_004322dc_caseD_4:
  frame.script_cursor = page_script + 1;
switchD_004322dc_default:
  opcode_or_text = frame.script_cursor[1];
  page_script = frame.script_cursor + 1;
  if (opcode_or_text == -1) goto LAB_004328e0;
  goto LAB_004322cd;
}


