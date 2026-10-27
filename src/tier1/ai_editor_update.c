#include "mfl_tier2.h"
/*00426bb0*/ extern int ai_editor_paint_input();
/*00426de0*/ extern int ai_editor_cursor_move();
/*00426e90*/ extern int ai_editor_brush_select();
/*004271e0*/ extern int ai_editor();
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];


/* ai_editor_update */

/* "runs the three AI editor input handlers when player slot 3 is the editor slot" */

void ai_editor_update(void)
{
  if (g_player_slots[3].extra == 1) {
    ai_editor_paint_input();
    ai_editor_cursor_move();
    ai_editor_brush_select();
    ai_editor();
    return;
  }
  return;
}
