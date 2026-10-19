#include "mfl_tier2.h"
/*0041a090*/ extern undefined __cdecl ai_paint_cell(undefined4 p0, undefined4 p1, undefined4 p2);
/*0041a170*/ extern undefined __cdecl ai_erase_cell(undefined4 p0, undefined4 p1);
/*004b2b48*/ extern int _ftol();
/*005dd628*/ extern float k_f_0_25;
/*006642fc*/ extern undefined4 g_ai_editor_6642fc;
/*00664300*/ extern undefined4 g_ai_editor_664300;
/*00664304*/ extern undefined4 g_ai_editor_664304;
/*00664308*/ extern undefined4 g_ai_editor_664308;
/*0066430c*/ extern undefined4 g_ai_editor_66430c;
/*00664314*/ extern undefined4 g_ai_editor_664314;
/*00664318*/ extern undefined4 g_ai_editor_664318;
/*0066431c*/ extern undefined4 g_ai_editor_66431c;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
/*007f024c*/ extern undefined4 g_ai_editor_menu_cursor;
/*007f0278*/ extern undefined4 g_ai_editor_brush;

/* view origin (world units) to cell index: scale, negated */
#define VIEW_TO_CELL_SCALE (-4.0f)
/* frames a direction must be held before auto-repeat starts (inferred) */
#define KEY_REPEAT_DELAY (10)

/* ai_editor_paint_input */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "AI editor: menu select with the paint tool paints the cell under the cursor (ai_paint_cell)" */

/*
 * Per-frame input handler for the AI editor (inferred from the code).
 * Paints or erases the grid cell at the cursor from player 0's input, and
 * pans the editor view with the four direction flags.
 * Globals: the g_ai_editor_66xxxx key-state bytes are 0 = released,
 * 1 = just pressed, 2 = held; the 664xxx counters are key-repeat timers.
 * Takes no parameters, returns nothing.
 */
void ai_editor_paint_input(void)

{
  int origin_cell_z;
  int origin_cell_x;

  /* cell index of the view origin (inferred: the editor cursor cell) */
  origin_cell_z = ((int)(g_ai_editor_view_origin_z * VIEW_TO_CELL_SCALE));
  origin_cell_x = ((int)(g_ai_editor_view_origin_x * VIEW_TO_CELL_SCALE));
  /* menu select in editor mode 0 on menu cursor 4: paint the cell at the cursor */
  if (((g_player_inputs[0].menu_select != 0) && (g_ai_editor_mode == 0)) &&
     (g_ai_editor_menu_cursor == 4)) {
    ai_paint_cell(-0x14 - origin_cell_z,-0xc - origin_cell_x,(byte)g_ai_editor_brush & 0xffffff03);
  }
  /* byte +0x3e of player 0 is not a named struct member (inferred: erase button) */
  if ((((*(byte *)((char *)&g_player_inputs[0] + 62)) != '\0') && (g_ai_editor_mode == 0)) &&
     (g_ai_editor_menu_cursor == 4)) {
    ai_erase_cell(-0x14 - origin_cell_z,-0xc - origin_cell_x);
  }
  /* left (flag 0): move the view origin z down; first press steps once, then
     auto-repeats after KEY_REPEAT_DELAY frames (inferred) */
  if (g_player_inputs[0].direction_flags[0] == 0) {
    g_ai_editor_66430c = 0;
LAB_00426ca3:
    g_ai_editor_664318 = 0;
  }
  else {
    if (g_ai_editor_664318 == 0) {
      g_ai_editor_view_origin_z = g_ai_editor_view_origin_z - k_f_0_25;
    }
    if (g_ai_editor_66430c == 0) {
      g_ai_editor_66430c = 1;
      g_ai_editor_664318 = KEY_REPEAT_DELAY;
    }
    else {
      g_ai_editor_66430c = 2;
      if (g_ai_editor_664318 == 0) {
        g_ai_editor_664318 = 1;
      }
      else {
        g_ai_editor_664318 = g_ai_editor_664318 + -1;
        if ((int)g_ai_editor_664318 < 0) goto LAB_00426ca3;
      }
    }
  }
  /* right (flag 1): move the view origin z up, same repeat logic as left */
  if (g_player_inputs[0].direction_flags[1] == 0) {
    g_ai_editor_66431c = 0;
LAB_00426d04:
    g_ai_editor_664304 = 0;
  }
  else {
    if (g_ai_editor_664304 == 0) {
      g_ai_editor_view_origin_z = g_ai_editor_view_origin_z + k_f_0_25;
    }
    if (g_ai_editor_66431c == 0) {
      g_ai_editor_66431c = 1;
      g_ai_editor_664304 = KEY_REPEAT_DELAY;
    }
    else {
      g_ai_editor_66431c = 2;
      if (g_ai_editor_664304 == 0) {
        g_ai_editor_664304 = 1;
      }
      else {
        g_ai_editor_664304 = g_ai_editor_664304 + -1;
        if ((int)g_ai_editor_664304 < 0) goto LAB_00426d04;
      }
    }
  }
  /* up (flag 2): move the view origin x up, same repeat logic as left */
  if (g_player_inputs[0].direction_flags[2] == 0) {
    g_ai_editor_6642fc = 0;
  }
  else {
    if (g_ai_editor_664314 == 0) {
      g_ai_editor_view_origin_x = g_ai_editor_view_origin_x + k_f_0_25;
    }
    if (g_ai_editor_6642fc == 0) {
      g_ai_editor_6642fc = 1;
      g_ai_editor_664314 = KEY_REPEAT_DELAY;
      goto LAB_00426d6b;
    }
    g_ai_editor_6642fc = 2;
    if (g_ai_editor_664314 == 0) {
      g_ai_editor_664314 = 1;
      goto LAB_00426d6b;
    }
    g_ai_editor_664314 = g_ai_editor_664314 + -1;
    if (-1 < (int)g_ai_editor_664314) goto LAB_00426d6b;
  }
  g_ai_editor_664314 = 0;
LAB_00426d6b:
  /* down (flag 3): move the view origin x down, same repeat logic as left;
     releasing all repeat state here */
  if (g_player_inputs[0].direction_flags[3] == 0) {
    g_ai_editor_664300 = 0;
    g_ai_editor_664308 = 0;
    return;
  }
  if (g_ai_editor_664308 == 0) {
    g_ai_editor_view_origin_x = g_ai_editor_view_origin_x - k_f_0_25;
  }
  if (g_ai_editor_664300 == 0) {
    g_ai_editor_664308 = KEY_REPEAT_DELAY;
    g_ai_editor_664300 = 1;
    return;
  }
  g_ai_editor_664300 = 2;
  if (g_ai_editor_664308 == 0) {
    g_ai_editor_664308 = 1;
    return;
  }
  g_ai_editor_664308 = g_ai_editor_664308 + -1;
  if ((int)g_ai_editor_664308 < 0) {
    g_ai_editor_664308 = 0;
  }
  return;
}


