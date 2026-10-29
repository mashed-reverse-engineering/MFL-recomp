#include "mfl_tier2.h"
/*00434be0*/ extern int event_type_from_menu_choice();
/*00435fc0*/ extern int __cdecl career_event_set_progress(undefined4 p0);
/*00436200*/ extern int menu_event_column_progress_unless_special();
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069cc44*/ extern undefined4 g_menu_69cc44;
/*0069d688*/ extern undefined4 g_career_row;
/*0069d690*/ extern undefined4 g_menu_event_column;
/*0069d6b0*/ extern undefined4 g_menu_69d6b0;
/* career_row_previous */

/* "decrements g_career_row skipping disabled rows" */

/*
 * Moves the career row selection to the previous selectable row (inferred from the
 * name and the description above). Takes no parameters and returns nothing.
 * Globals: g_career_row is the row, g_menu_event_column the menu column,
 * g_event_type the event type, g_menu_69cc44 a flag read once on entry (inferred).
 * Returns early, leaving g_career_row unchanged, if career_event_set_progress() != 0.
 */
void career_row_previous(void)

{
  int saved_menu_69cc44;
  int row;
  int result;
  int target_column;
  int column;

  saved_menu_69cc44 = g_menu_69cc44;
  column = g_menu_event_column;
LAB_00438106:
  /* step back one row; wrap at 0 and reset g_menu_69d6b0 (inferred) */
  do {
    g_career_row = g_career_row + -1;
    if ((int)g_career_row < 0) {
      g_career_row = 0;
      g_menu_69d6b0 = 0;
    }
    row = g_career_row;
    result = career_event_set_progress(g_career_row);
    if (result != 0) {
      return;
    }
    /* default column for this event type: 3 or 6 from the flag, overridden by the
       event type ranges below (inferred) */
    result = (-(uint)(saved_menu_69cc44 != 0) & 3) + 3;
    if (((g_event_type == EVENT_QUICK_RACE_11) || (g_event_type == EVENT_QUICK_RACE_12)) || (g_event_type == EVENT_QUICK_RACE_13)) {
      result = 0xb;
    }
    if (((g_event_type == EVENT_TIME_TRIAL_0) || (g_event_type == EVENT_TIME_TRIAL_1)) || (g_event_type == EVENT_TIME_TRIAL_2)) {
      result = 0;
    }
    /* target column: the event type itself for most types, else the default above (inferred) */
    target_column = g_event_type;
    if (((g_event_type != EVENT_TURBO_TOURNAMENT) && (g_event_type != EVENT_ICE_TOURNAMENT)) &&
       ((g_event_type != EVENT_NIGHT_TOURNAMENT && (target_column = result, g_event_type == EVENT_TRAINING)))) {
      target_column = g_event_type;
    }
    if (column == target_column) {
      g_career_row = row + -1;
      return;
    }
    /* walk the menu column down toward target_column, stopping on a special column
       (inferred); reaching the bottom of the range jumps back to the row loop */
    do {
      column = column + -1;
      if (column < target_column) {
        g_menu_event_column = target_column;
        event_type_from_menu_choice();
        g_career_row = row + 1;
        column = target_column;
        goto LAB_00438106;
      }
      g_menu_event_column = column;
      result = menu_event_column_progress_unless_special();
    } while (result == 0);
    event_type_from_menu_choice();
    g_career_row = row + 1;
  } while( true );
}


