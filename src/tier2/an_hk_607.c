#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*007eef30*/ extern char g_game_state_progress_220_[];

#define PROGRESS_COLUMNS (0x12) /* columns per row of progress[] (15 rows x 18 columns) */
#define PROGRESS_GROUP_STRIDE (0x6c) /* 6 rows: the loop steps one group of six rows at a time */

/* career_progress_event_unlocked */

/* "loops the progress table, tests the event ids 0xC..0xE" */

/*
 * Returns true when career event event_id is unlocked, i.e. its own cell progress[event_id][3]
 * is non-zero and no group of six rows blocks it. Only event ids 0xC..0xE (12..14) are gated;
 * any other id just returns its own cell. The gate column for an event is event_id - 9
 * (inferred: 12 -> column 3, 13 -> column 4, 14 -> column 5). Parameter: event_id, the row of
 * progress[] (0 locked, 1 open, 2 done per the struct comment).
 */
bool __cdecl career_progress_event_unlocked(int event_id)

{
  bool prerequisite_missing;
  uint *group_cell;
  uint cell_value;

  prerequisite_missing = false;
  group_cell = g_game_state.progress + 4;
  /* walk the table one group of six rows at a time, starting at row 0 column 4; stop at progress[220] */
  do {
    /* event 12: column 3 of all six rows must be non-zero (row 0 here, rows 1..5 below) */
    if (event_id == 0xc) {
      cell_value = group_cell[-1];
LAB_00433f5b:
      if (cell_value != 0) goto LAB_00433f5f;
LAB_00433fcf:
      prerequisite_missing = true;
      break;
    }
    /* event 13: column 4 of all six rows must be non-zero */
    if (event_id == 0xd) {
      cell_value = *group_cell;
      goto LAB_00433f5b;
    }
    /* event 14: column 5 of all six rows must be non-zero */
    if (event_id == 0xe) {
      cell_value = group_cell[1];
      goto LAB_00433f5b;
    }
LAB_00433f5f:
    /* events 13 and 14: rows 1..5 of their column; a zero in any of them blocks the event */
    if (event_id != 0xc) {
      if (event_id == 0xd) {
        if (((group_cell[PROGRESS_COLUMNS] != 0) && (group_cell[0x24] != 0)) &&
           ((group_cell[0x36] != 0 && (group_cell[0x48] != 0)))) {
          cell_value = group_cell[0x5a];
          goto LAB_0043400f;
        }
      }
      else {
        if (event_id != 0xe) goto LAB_00434013;
        if ((((group_cell[0x13] != 0) && (group_cell[0x25] != 0)) && (group_cell[0x37] != 0)) &&
           (group_cell[0x49] != 0)) {
          cell_value = group_cell[0x5b];
          goto LAB_0043400f;
        }
      }
      goto LAB_00433fcf;
    }
    /* event 12: rows 1..5 of column 3 (row 5 is read into cell_value and tested at LAB_0043400f) */
    if (((group_cell[0x11] == 0) || (group_cell[0x23] == 0)) || ((group_cell[0x35] == 0 || (group_cell[0x47] == 0)))
       ) goto LAB_00433fcf;
    cell_value = group_cell[0x59];
LAB_0043400f:
    if (cell_value == 0) goto LAB_00433fcf;
LAB_00434013:
    group_cell = group_cell + PROGRESS_GROUP_STRIDE;
  } while ((LONG_PTR)group_cell < ((LONG_PTR)&g_game_state_progress_220_));
  /* unlocked = own cell set and no group blocked it */
  return g_game_state.progress[event_id * PROGRESS_COLUMNS + 3] != 0 && !prerequisite_missing;
}


