#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
typedef signed char sbyte;
/*004b4ba7*/ extern int entry();
/*0069cbd0*/ extern undefined4 g_menu_current;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];

#define MENU_SELECTION_NONE (-1)

/* menu_selection_step */

/*
 * Moves the selection of the current menu page by delta entries, wrapping
 * around at both ends. Entries whose enabled flag is not 1 are skipped.
 * delta: signed step (+1 next, -1 previous) (inferred)
 * If the page has no entries, or no enabled entry is found, the selection is
 * set to MENU_SELECTION_NONE.
 */

void menu_selection_step(int delta)

{
  bool found_enabled;
  int menu_index;
  int new_selected;
  int entries_checked;

  menu_index = g_menu_current;
  entries_checked = 0;
  found_enabled = false;
  /* empty page: nothing to select */
  if (g_menu_records[g_menu_current].entry_count == 0) {
    g_menu_records[g_menu_current].selected = MENU_SELECTION_NONE;
    return;
  }
  /* step and wrap until an enabled entry is found; give up after one full cycle */
  while( true ) {
    new_selected = g_menu_records[menu_index].selected + delta;
    g_menu_records[menu_index].selected = new_selected;
    if (new_selected == g_menu_records[menu_index].entry_count) {
      g_menu_records[menu_index].selected = 0;
    }
    else if (new_selected < 0) {
      g_menu_records[menu_index].selected = g_menu_records[menu_index].entry_count + -1;
    }
    if (g_menu_records[menu_index].entry_enabled[g_menu_records[menu_index].selected] == 1) {
      found_enabled = true;
    }
    entries_checked = entries_checked + 1;
    if (g_menu_records[menu_index].entry_count < entries_checked) break;
    if (found_enabled) {
      return;
    }
  }
  g_menu_records[menu_index].selected = MENU_SELECTION_NONE;
  return;
}
