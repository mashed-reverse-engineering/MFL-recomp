#include "mfl_tier2.h"
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;
/*007eebc0*/ extern struct GameState g_game_state;

#define EVENT_TYPE_OFFSET (3)
#define COLUMNS_PER_ROW (18)

/* menu_event_column_progress_unless_special */

/* Returns 0 when the current menu event column is a special column for the current event type
   (not a playable event slot), otherwise returns the progress state of that event in the current
   career row (0 locked, 1 open, 2 done). Special columns depend on the event type (inferred from
   the branch chains below): for event types 3-5 columns 0-2 and 6-17 are special; for event
   types 6-8 columns 0-5 and 9-17 are special. Event types outside 3-8 return the raw event offset
   (inferred: probably unintended). */

uint menu_event_column_progress_unless_special(void)

{
  uint result;
  bool is_group_last_special;

  result = g_event_type - EVENT_TYPE_OFFSET;
  switch(result) {
  /* event types 3-5: columns 0-2 are special, columns 3-5 are normal events */
  case 0:
  case 1:
  case 2:
    if (g_menu_event_column == 0) {
      return 0;
    }
    if (g_menu_event_column == 1) {
      return 0;
    }
    if (g_menu_event_column == 2) {
      return 0;
    }
    if (g_menu_event_column == 6) {
      return 0;
    }
    if (g_menu_event_column == 7) {
      return 0;
    }
    is_group_last_special = g_menu_event_column == 8;
    break;
  /* event types 6-8: columns 0-5 are special, columns 6-8 are normal events */
  case 3:
  case 4:
  case 5:
    if (g_menu_event_column == 0) {
      return 0;
    }
    if (g_menu_event_column == 1) {
      return 0;
    }
    if (g_menu_event_column == 2) {
      return 0;
    }
    if (g_menu_event_column == 3) {
      return 0;
    }
    if (g_menu_event_column == 4) {
      return 0;
    }
    is_group_last_special = g_menu_event_column == 5;
    break;
  default:
    goto switchD_00436211_default;
  }
  /* columns 9-17 are special for every event type */
  if ((((((is_group_last_special) || (g_menu_event_column == 9)) || (g_menu_event_column == 10)) ||
       ((g_menu_event_column == 0xb || (g_menu_event_column == 0xc)))) ||
      ((g_menu_event_column == 0xd || ((g_menu_event_column == 0xe || (g_menu_event_column == 0xf)))
       ))) || ((g_menu_event_column == 0x10 || (g_menu_event_column == 0x11)))) {
    return 0;
  }
  /* normal event: progress of this column in the current career row */
  result = g_game_state.progress[g_menu_event_column + g_career_row * COLUMNS_PER_ROW];
switchD_00436211_default:
  return result;
}


