#include "mfl_tier2.h"
/*0040d130*/ extern int __cdecl find_texture_a(RwChar *p0);
/*004169f0*/ extern int career_event_slot_table();
/*0042c440*/ extern int ui_rt2d_text_draw();
/*0042ebd0*/ extern double time_get_fraction();
/*0042ebe0*/ extern int time_get_minutes();
/*0042ebf0*/ extern int time_get_seconds();
/*00430ab0*/ extern int screen_width();
/*00430ac0*/ extern int screen_height();
/*00475730*/ extern int get_viewport_camera();
/*004d78f0*/ extern struct RwRaster * RwRasterSubRaster(struct RwRaster *p0, struct RwRaster *p1, struct RwRect *p2);
/*005dd7e8*/ extern float k_f_60;
/*006091a4*/ extern undefined g_career_row_6091a4;
/*006091b8*/ extern undefined g_career_row_6091b8;
/*006091cc*/ extern undefined g_career_row_6091cc;
/*006091e0*/ extern undefined g_career_row_6091e0;
/*0062ac90*/ extern undefined g_ui_62ac90;
/*0062ac94*/ extern undefined g_ui_62ac94;
/*0062ac98*/ extern undefined g_ui_62ac98;
/*0069c9e8*/ extern undefined4 g_menu_panel_state_69c9e8;
/*0069c9ec*/ extern undefined1 g_hud_69c9ec;
/*0069ca34*/ extern undefined4 g_menu_update_69ca34;
/*0069cb10*/ extern undefined4 g_ui_69cb10;
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cc4c*/ extern undefined4 g_menu_69cc4c;
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/*0069cf9c*/ extern undefined4 g_menu_race_length_choice;
/*0069cfa4*/ extern undefined4 g_menu_game_mode_choice;
/*0069cfac*/ extern undefined4 g_menu_bot_skill_b;
/*0069cfb0*/ extern undefined4 g_menu_event_course_index;
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfc4*/ extern float g_menu_69cfc4;
/*0069cfd0*/ extern undefined4 g_message_state;
/*0069d2c8*/ extern undefined4 g_tutorial_69d2c8;
/*0069d2d0*/ extern undefined4 g_menu_career_race_active;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*0069d69c*/ extern undefined4 g_ui_69d69c;
/*0069d6a8*/ extern undefined4 g_menu_select_pressed;
/*0069d6ac*/ extern undefined4 g_message_69d6ac;
/*0069d6b0*/ extern undefined4 g_menu_69d6b0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7d0*/ extern undefined4 g_menu_7ef7d0;
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*008c8730*/ extern undefined4 g_ui_8c8730;
/*008c8734*/ extern undefined4 g_ui_camera_8c8734;
/*008c8738*/ extern undefined4 g_ui_camera_8c8738;
/*008c873c*/ extern undefined4 g_ui_camera_8c873c;


extern undefined4 medal_asset_names(char *param_1);


extern int __cdecl career_row_title_string_id(char *param_1);


extern undefined4 is_team_mode(void);


extern undefined4 get_second_camera(void);


extern undefined * ui_get_8c8730(void);


extern undefined4 ui_camera_update_view_rect(void);


extern undefined4 get_event_type(void);


/* ui_get_menu_69d6a8 */

/* "return g_menu_69d6a8;" */

extern undefined4 ui_get_menu_69d6a8(void);


/* ui_get_message_69d6ac */

/* "return g_message_69d6ac;" */

extern undefined4 ui_get_message_69d6ac(void);


/* ui_get_menu_69d6b0 */

/* "return g_menu_69d6b0;" */

extern undefined4 ui_get_menu_69d6b0(void);


/* ui_get_menu_69cc4c */

/* "return g_menu_69cc4c;" */

extern undefined4 ui_get_menu_69cc4c(void);


/* ui_menu_69cc4c_clear */

/* "g_menu_69cc4c = 0; return;" */

extern void ui_menu_69cc4c_clear(void);


/* menu_player_direction_navigate */

/* "per player slot steps four counters (0..3) from the direction flag edges" */

extern void menu_player_direction_navigate(void);


/* menu_controller_navigation_update */

/* "per controller checks previous direction flags and adjusts counters" */

extern void menu_controller_navigation_update(void);


/* team_color_names */

/* "NFLRed NFLBluejay NFLMelon NFLGold" */

extern void team_color_names(char *param_1);


/* menu_markers_enabled_for_event */

/* "0 for event type 0x12 else g_menu_markers_enabled" */

extern uint menu_markers_enabled_for_event(void);


/* race_length_setting */

/* 0..2 -> time limit 900000 / 1800000 / 3600000 game ticks (3000 per second = 5 / 10 / 20 minutes)
    */

extern undefined4 race_length_setting(void);


/* menu_bot_skill_b_get */

/* "returns g_menu_bot_skill_b" */

extern undefined4 menu_bot_skill_b_get(void);


/* menu_panel_draw_69c9e8 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "panel with state flag g_menu_panel_state_69c9e8 (fade counter 69c9ec)" */

extern void menu_panel_draw_69c9e8(void);

#define CAREER_ROW_STRIDE (0x30) /* bytes per career row in the table returned by career_event_slot_table (inferred) */
#define NO_ID (-1) /* "no team / no course" result (inferred) */
#define TEAM_COUNT (6) /* team ids searched for a free team (inferred) */
#define PLAYER_SLOT_COUNT (4)


/* ui_collect_team_ids */

/* "fills the 4 team ids of the player slots" */

/* Returns the team id of player slot `slot` (0..3), or NO_ID if the slot has none.
   Normal event types: team_ids[0] is player 0's team; team_ids[1..3] are the entries of the
   current career row, with an entry that clashes with player 0's team replaced by a free team id (inferred).
   Event types 0xb..0xd: `slot` counts the players with a non-zero g_menu_player_team_choice, and the
   result is that entry of g_menu_event_course_index minus 1 (inferred; the index used is one past the
   matched player, so this may be an off-by-one). */

/* TIER2: validated 2026-10-18 authority=diffemu */
int __cdecl ui_collect_team_ids(int slot)
{
  int result;
  int *row_group;
  int *entry_ptr;
  int candidate_team;
  int team_or_count;
  int resolved_team;
  int index;
  int team_ids [4];
  
  result = NO_ID;
  team_ids[1] = NO_ID;
  team_ids[2] = NO_ID;
  team_ids[3] = NO_ID;
  /* Event types other than 0xb..0xd: team ids come from the career row table. */
  if (((g_event_type != EVENT_QUICK_RACE_11) && (g_event_type != EVENT_QUICK_RACE_12)) && (g_event_type != EVENT_QUICK_RACE_13)) {
    team_ids[0] = g_player_slots[0].team;
    result = career_event_slot_table();
    row_group = (int *)(result + g_career_row * CAREER_ROW_STRIDE);
    if (g_event_type == EVENT_CUP_4) {
      row_group = row_group + 4;
    }
    else if (g_event_type == EVENT_CUP_5) {
      row_group = row_group + 8;
    }
    /* row_group[0] = number of entries (at most 3), row_group[1..3] = their team ids (inferred). */
    result = *row_group;
    index = 0;
    entry_ptr = row_group + 1;
    if (0 < result) {
      /* For each entry: keep its team; if it equals player 0's team, search for a free team id (inferred). */
      do {
        team_or_count = *entry_ptr;
        resolved_team = team_or_count;
        if (team_or_count == g_player_slots[0].team) {
          candidate_team = 0;
          do {
            if (((candidate_team != g_player_slots[0].team) && (candidate_team != row_group[1])) &&
               ((candidate_team != row_group[2] && (resolved_team = candidate_team, candidate_team != row_group[3])))) break;
            candidate_team = candidate_team + 1;
            resolved_team = team_or_count;
          } while (candidate_team < TEAM_COUNT);
        }
        team_ids[index + 1] = resolved_team;
        index = index + 1;
        entry_ptr = entry_ptr + 1;
      } while (index < result);
    }
    return team_ids[slot];
  }
  /* Count selected players until reaching `slot`; index ends one past the last checked player. */
  team_or_count = 0;
  index = 0;
  do {
    if (slot <= team_or_count) break;
    if ((&g_menu_player_team_choice)[index] != 0) {
      team_or_count = team_or_count + 1;
    }
    index = index + 1;
  } while (index < PLAYER_SLOT_COUNT);
  if (team_or_count == slot) {
    result = (&g_menu_event_course_index)[index] + -1;
  }
  return result;
}


/* event_type_uses_standard_menu */

/* "false for event types 6..10 and 0x12, true for the others" */

extern undefined4 event_type_uses_standard_menu(void);


/* get_career_row */

/* "returns g_career_row (selected row of the career grid, 0..14)" */

extern undefined4 get_career_row(void);


/* ui_get_menu_69d690 */

/* "return g_menu_69d690;" */

extern undefined4 ui_get_menu_69d690(void);


/* career_time_under_target */

/* "current career time (min * 60 + s + fraction) is below the sum of the thresholds of the row" */

extern undefined4 career_time_under_target(void);


/* ui_get_menu_69d2d0 */

/* "return g_menu_69d2d0;" */

extern undefined4 ui_get_menu_69d2d0(void);


/* menu_event_column_is_special */

/* "returns whether the event column is one of 0,1,2,6,7,8 depending on the event type" */

extern uint menu_event_column_is_special(void);


/* career_event_has_entry */

/* "switch on event type returning 1 for the listed events" */

extern undefined4 career_event_has_entry(void);


/* option_slider_1_reset */

/* "g_game_state.option_sliders[1] = 0.7" */

extern void option_slider_1_reset(void);


/* option_slider_2_reset */

/* "g_game_state.option_sliders[2] = 0.7" */

extern void option_slider_2_reset(void);


/* option_slider_0_reset */

/* "g_game_state.option_sliders[0] = 0.7" */

extern void option_slider_0_reset(void);


/* option_slider_4_reset */

/* "g_game_state.option_sliders[4] = 2" */

extern void option_slider_4_reset(void);


/* menu_get_69cfc4 */

/* "float g_menu_69cfc4" */

extern double menu_get_69cfc4(void);


/* menu_set_69cfc4 */

/*  */

extern void menu_set_69cfc4(char *param_1);


/* option_get_slider_4 */

/* "g_game_state.option_sliders[4]" */

extern uint option_get_slider_4(void);


