#include "mfl_tier2.h"
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d690*/ extern undefined4 g_menu_event_column;
/* medal_asset_names */
/* "TurboT NightT TimeTrial BronzeMedal" */
/* career_row_title_string_id */
/* "(row or -1): string id of the event title for the current event type" */
/* is_team_mode */
/* "returns the flag at 0x69CC40 (team play)" */
/* get_second_camera */
/* "returns the camera pointer at 0x69D69C used in split screen" */
/* ui_get_8c8730 */
/* "returns &g_ui_8c8730" */
/* ui_camera_update_view_rect */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "computes the viewport rectangle of the secondary frontend camera from the screen size" */
/* get_event_type */
/* "returns g_event_type (the column of the career grid, 0..0x11; 0x11 is a special variant)" */
/* event_type_from_menu_choice */
/* Copies the career menu's event column (g_menu_event_column, 0..0x11) into g_event_type.
   Columns 0-8 and 0xb-0x11 map to the same event type value; columns 9, 0xa and anything
   above 0x11 match no case and leave g_event_type unchanged. */
void event_type_from_menu_choice(void)
{
  switch(g_menu_event_column) {
  case 0:
    g_event_type = EVENT_TIME_TRIAL_0;
    return;
  case 1:
    g_event_type = EVENT_TIME_TRIAL_1;
    return;
  case 2:
    g_event_type = EVENT_TIME_TRIAL_2;
    return;
  case 3:
    g_event_type = EVENT_CUP_3;
    return;
  case 4:
    g_event_type = EVENT_CUP_4;
    return;
  case 6:
    g_event_type = EVENT_MULTIPLAYER_6;
    return;
  case 7:
    g_event_type = EVENT_MULTIPLAYER_7;
    return;
  case 8:
    g_event_type = EVENT_MULTIPLAYER_8;
    return;
  case 5:
    g_event_type = EVENT_CUP_5;
    return;
  case 0xb:
    g_event_type = EVENT_QUICK_RACE_11;
    return;
  case 0xc:
    g_event_type = EVENT_QUICK_RACE_12;
    return;
  case 0xd:
    g_event_type = EVENT_QUICK_RACE_13;
    return;
  case 0xe:
    g_event_type = EVENT_TURBO_TOURNAMENT;
    return;
  case 0xf:
    g_event_type = EVENT_ICE_TOURNAMENT;
    return;
  case 0x10:
    g_event_type = EVENT_NIGHT_TOURNAMENT;
    return;
  case 0x11:
    g_event_type = EVENT_TRAINING;
  }
  return;
}
