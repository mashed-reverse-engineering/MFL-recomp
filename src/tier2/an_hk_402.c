#include "mfl_tier2.h"
/*0069cb10*/ extern undefined4 g_ui_69cb10;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0240*/ extern char g_ai_editor_view_origin_x[];
#define SLOT_NO_CONTROLLER (-1)
#define SLOT_STATE_UNUSED (-1)
#define UI_CONTROLLER_STRIDE (3)
/* player_slots_reset_state
 * Resets the state of every PlayerSlot to SLOT_STATE_UNUSED, then, for each slot that has a
 * controller, sets its state from g_ui_69cb10 (state = ui value - 1, inferred). Counts the active
 * slots and how many are in state 0 and state 1, and returns a result code from those counts
 * (inferred: 0x1000 for a mixed split, 0 / 1 / 2 / 3 / -1 for the other layouts).
 * Parameters: none. Returns the result code described above. */

/* "state of every PlayerSlot = -1" */

int player_slots_reset_state(void)

{
  int *state_ptr;
  int state0_count;
  int active_count;
  int state1_count;
  bool state1_matches;

  state_ptr = &g_player_slots[0].state;
  do {
    *state_ptr = SLOT_STATE_UNUSED;
    state_ptr = state_ptr + 4;
  } while ((LONG_PTR)state_ptr < ((LONG_PTR)&g_ai_editor_view_origin_x));
  /* per-slot pass: slots 0..3 in order, each with a controller counts as active */
  active_count = 0;
  state1_count = 0;
  state0_count = 0;
  if (SLOT_NO_CONTROLLER < g_player_slots[0].controller) {
    g_player_slots[0].state = (&g_ui_69cb10)[g_player_slots[0].controller * UI_CONTROLLER_STRIDE] + -1;
    if (g_player_slots[0].state == 0) {
      state0_count = 1;
    }
    else if (g_player_slots[0].state == 1) {
      state1_count = 1;
    }
    active_count = 1;
  }
  if (SLOT_NO_CONTROLLER < g_player_slots[1].controller) {
    g_player_slots[1].state = (&g_ui_69cb10)[g_player_slots[1].controller * UI_CONTROLLER_STRIDE] + -1;
    if (g_player_slots[1].state == 0) {
      state0_count = state0_count + 1;
    }
    else if (g_player_slots[1].state == 1) {
      state1_count = state1_count + 1;
    }
    active_count = active_count + 1;
  }
  if (SLOT_NO_CONTROLLER < g_player_slots[2].controller) {
    g_player_slots[2].state = (&g_ui_69cb10)[g_player_slots[2].controller * UI_CONTROLLER_STRIDE] + -1;
    if (g_player_slots[2].state == 0) {
      state0_count = state0_count + 1;
    }
    else if (g_player_slots[2].state == 1) {
      state1_count = state1_count + 1;
    }
    active_count = active_count + 1;
  }
  if (SLOT_NO_CONTROLLER < g_player_slots[3].controller) {
    g_player_slots[3].state = (&g_ui_69cb10)[g_player_slots[3].controller * UI_CONTROLLER_STRIDE] + -1;
    if (g_player_slots[3].state == 0) {
      state0_count = state0_count + 1;
    }
    else if (g_player_slots[3].state == 1) {
      state1_count = state1_count + 1;
    }
    active_count = active_count + 1;
  }
  /* 2 active slots: 0x1000 only for a 1 / 1 split between state 0 and state 1 (inferred) */
  if (active_count == 2) {
    if ((state0_count == 1) && (state1_count == 1)) {
      return 0x1000;
    }
    return 0;
  }
  /* 3 active slots: branches on how many are in state 0 (inferred split rules) */
  if (active_count == 3) {
    if (state0_count != 1) {
      if (state0_count == 2) {
        if (state1_count == 1) {
          return 0x1000;
        }
        goto LAB_00430e92;
      }
LAB_00430e82:
      if (state0_count == 0) {
        return 1;
      }
      goto LAB_00430e92;
    }
LAB_00430e8d:
    state1_matches = state1_count == 2;
  }
  else {
    /* 4 active slots: same split rules as the 3-slot case (inferred); other counts return -1 */
    if (active_count != 4) {
      return -1;
    }
    if (state0_count == 2) goto LAB_00430e8d;
    if (state0_count != 1) {
      if (state0_count == 3) {
        if (state1_count == 1) {
          return 0x1000;
        }
        goto LAB_00430e92;
      }
      goto LAB_00430e82;
    }
    state1_matches = state1_count == 3;
  }
  /* shared tail: 0x1000 when the state-1 count matches, otherwise 2 or 3 by state-1 count (inferred) */
  if (state1_matches) {
    return 0x1000;
  }
LAB_00430e92:
  return (state1_count != 0) + 2;
}


