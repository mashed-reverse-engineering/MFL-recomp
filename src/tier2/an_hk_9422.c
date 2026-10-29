#include "mfl_tier2.h"
/*0042fc00*/ extern void menu_selection_step(int step);
/*00434be0*/ extern void event_type_from_menu_choice(void);
/*00435fc0*/ extern int __cdecl career_event_set_progress(int row);
/*00436200*/ extern int menu_event_column_progress_unless_special(void);
/*0069cbd0*/ extern int g_menu_current;
/*0069cbd4*/ extern int g_event_type;
/*0069cc44*/ extern int g_menu_69cc44;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern int g_career_row;
/*0069d690*/ extern int g_menu_event_column;
/*0069d6b0*/ extern int g_menu_69d6b0;
/* menu_career_row_next */
/* "menu_selection_step(1) and advances g_career_row/column with wrap and event type update" */
/* TIER2: unvalidated 2026-10-29 authority=static */
void menu_career_row_next(void)
{
  int type;
  int rows;
  int row;
  int column;
  int target;

  type = g_event_type;
  rows = 15;
  if (type == 0xe || type == 0xf || type == 0x10) {
    rows = 5;
  }
  if (type == 0x11) {
    rows = 6;
  }
  menu_selection_step(1);
  if (g_menu_records[g_menu_current].page_id >= 6 && g_menu_records[g_menu_current].page_id <= 7) {
    column = g_menu_event_column;
    row = g_career_row;
again:
    row++;
    g_career_row = row;
    if (row >= rows) {
      row = rows - 1;
      g_career_row = row;
      g_menu_69d6b0 = 0;
    }
    if (career_event_set_progress(row) != 0) {
      return;
    }
    target = g_menu_69cc44 != 0 ? 6 : 3;
    if (type == 0xb || type == 0xc || type == 0xd) {
      target = 0xb;
    }
    if (type == 0 || type == 1 || type == 2) {
      target = 0;
    }
    if (type == 0xe) {
      target = type;
    }
    else if (type == 0xf) {
      target = type;
    }
    else if (type == 0x10) {
      target = type;
    }
    else if (type == 0x11) {
      target = type;
    }
    if (column == target) {
      row--;
      g_menu_69d6b0 = 0;
      g_career_row = row;
      return;
    }
    do {
      column--;
      g_menu_event_column = column;
      if (column < target) {
        column = target;
        g_menu_event_column = column;
        event_type_from_menu_choice();
        type = g_event_type;
        row--;
        goto again;
      }
    } while (menu_event_column_progress_unless_special() == 0);
    event_type_from_menu_choice();
    type = g_event_type;
    row--;
    goto again;
  }
}
