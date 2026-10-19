#include "mfl_tier2.h"
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f024c*/ extern undefined4 g_ai_editor_menu_cursor;
/* ai_editor_cursor_move */

/* "AI editor: cursor movement" */

/* Moves the AI editor menu cursor by one step on a rising edge of the
   down (+0x39) or up (+0x38) input byte (inferred: both bytes sit in the
   unnamed PlayerInput gap just before insult). The valid range depends on
   g_ai_editor_mode (inferred: the menu page); the cursor wraps at both ends.
   Takes no parameters; updates g_ai_editor_menu_cursor, returns nothing. */
void ai_editor_cursor_move(void)

{
  int cursor_min;
  int cursor_max;

  /* default range 0..8 for unknown modes (inferred) */
  cursor_min = 0;
  cursor_max = 8;
  /* last valid cursor index for the current mode (inferred) */
  if (g_ai_editor_mode == 1) {
    cursor_max = 3;
  }
  else if (g_ai_editor_mode == 4) {
LAB_00426e24:
    cursor_max = 5;
  }
  else if (g_ai_editor_mode == 2) {
    cursor_max = 4;
  }
  else if (g_ai_editor_mode == 3) {
    cursor_max = 3;
  }
  else {
    if (g_ai_editor_mode != 5) {
      if (g_ai_editor_mode != 6) goto LAB_00426e2e;
      goto LAB_00426e24;
    }
    cursor_max = 3;
  }
  cursor_min = 2;
LAB_00426e2e:
  /* down edge: advance, wrap to cursor_min past cursor_max */
  if ((((*(byte *)((char *)&g_player_inputs[0] + 57)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 57)) == '\0')) &&
     (g_ai_editor_menu_cursor = g_ai_editor_menu_cursor + 1, cursor_max < (int)g_ai_editor_menu_cursor)) {
    g_ai_editor_menu_cursor = cursor_min;
  }
  /* up edge: step back, wrap to cursor_max below cursor_min */
  if ((((*(byte *)((char *)&g_player_inputs[0] + 56))!= '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 56)) == '\0')) &&
     (g_ai_editor_menu_cursor = g_ai_editor_menu_cursor + -1, (int)g_ai_editor_menu_cursor < cursor_min)) {
    g_ai_editor_menu_cursor = cursor_max;
  }
  return;
}


