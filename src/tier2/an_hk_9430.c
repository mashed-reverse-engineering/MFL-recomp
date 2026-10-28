#include "mfl_tier2.h"
/*00403a40*/ extern int viewport_set_view_window_080();
/*0041d460*/ extern int hudpanel_render_if_loaded_b();
/*0041e6b0*/ extern int score_panel_render();
/*0041f140*/ extern int team_panel_render();
/*0041f380*/ extern int hudpanel_render_if_loaded();
/*0041fd40*/ extern int kc_panel_render();
/*004208f0*/ extern int endpoint_panel_render();
/*00420c00*/ extern int stopclock_render_timed_parts_forward();
/*00420fa0*/ extern undefined __cdecl stopclock_render(undefined4 p0);
/*00421940*/ extern int vehicle_markers_render_if_loaded();
/*0042b0b0*/ extern int course_get_split_sector_8();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00437f10*/ extern int training_lesson_get();
/*00437f20*/ extern int ui_get_car_8c86c8();
/*00437f30*/ extern int ui_get_car_69d2d8();
/*00652f68*/ extern undefined4 g_race_phase;
/*007ef7b8*/ extern undefined4 game_mode;
/* hud_render_by_event_and_mode */

/* HUD dispatcher for race phases COUNTDOWN..STANDINGS: picks the marker/stopclock/hudpanel/score/team/kc/endpoint panel from event type and game mode, then draws the timed parts unless the event is a quick race (inferred) (was: "phase 7 only ...")
    */

void hud_render_by_event_and_mode(void)

{
  int query_result;
  undefined4 event_type;
  bool use_single_panel;
  
  query_result = course_get_split_sector_8();
  if (query_result == 0) {
    return;
  }
  if (g_race_phase < RACE_PHASE_COUNTDOWN) {
    return;
  }
  if (6 < g_race_phase) {
    if (g_race_phase != RACE_PHASE_STANDINGS) {
      return;
    }
    event_type = get_event_type();
    switch(event_type) {
    case 0:
    case 1:
    case 2:
      vehicle_markers_render_if_loaded();
      return;
    case 3:
    case 4:
    case 5:
    case 0xe:
    case 0xf:
    case 0x10:
      switch(game_mode) {
      case 4:
      case 7:
      case 9:
        stopclock_render(1);
        return;
      case 5:
        hudpanel_render_if_loaded();
        return;
      case 8:
        stopclock_render(0);
        return;
      case 10:
        hudpanel_render_if_loaded_b();
        return;
      }
    default:
      query_result = is_team_mode();
      use_single_panel = query_result == 0;
      break;
    case 6:
    case 7:
    case 8:
    case 0xb:
    case 0xc:
    case 0xd:
      query_result = is_team_mode();
      if (query_result == 0) goto LAB_0040fa58;
      use_single_panel = game_mode == GAME_MODE_HARE_AND_HOUNDS;
      break;
    case 0x11:
      query_result = training_lesson_get();
      if (query_result == 1) {
        return;
      }
      query_result = training_lesson_get();
      if (query_result == 4) {
        return;
      }
      goto LAB_0040fa58;
    }
    if (!use_single_panel) {
      endpoint_panel_render();
      return;
    }
LAB_0040fa58:
    kc_panel_render();
    return;
  }
  event_type = get_event_type();
  switch(event_type) {
  case 0:
  case 1:
  case 2:
    vehicle_markers_render_if_loaded();
    break;
  case 3:
  case 4:
  case 5:
  case 0xe:
  case 0xf:
  case 0x10:
    switch(game_mode) {
    case 4:
    case 7:
    case 9:
      goto switchD_0040fab4_caseD_4;
    case 5:
      hudpanel_render_if_loaded();
      break;
    default:
      goto switchD_0040fa80_caseD_9;
    case 8:
      stopclock_render(0);
      break;
    case 10:
      hudpanel_render_if_loaded_b();
    }
    break;
  case 6:
  case 7:
  case 8:
  case 0xb:
  case 0xc:
  case 0xd:
    query_result = is_team_mode();
    if (query_result != 0) {
      use_single_panel = game_mode == GAME_MODE_HARE_AND_HOUNDS;
      goto LAB_0040fb20;
    }
    goto LAB_0040fb29;
  default:
switchD_0040fa80_caseD_9:
    query_result = is_team_mode();
    use_single_panel = query_result == 0;
LAB_0040fb20:
    if (!use_single_panel) {
      team_panel_render();
      break;
    }
LAB_0040fb29:
    score_panel_render();
    break;
  case 0x11:
    query_result = ui_get_car_69d2d8();
    if ((query_result != 0) ||
       (((query_result = training_lesson_get(), query_result == 1 && (query_result = ui_get_car_8c86c8(), query_result != 1)) &&
        (query_result = ui_get_car_8c86c8(), query_result != 4)))) break;
    query_result = training_lesson_get();
    if (query_result != 4) goto LAB_0040fb29;
switchD_0040fab4_caseD_4:
    stopclock_render(1);
    break;
  case 0x12:
    viewport_set_view_window_080();
  }
  query_result = get_event_type();
  if (query_result == EVENT_QUICK_RACE) {
    return;
  }
  stopclock_render_timed_parts_forward();
  return;
}


