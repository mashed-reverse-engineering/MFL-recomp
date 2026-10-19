#include "mfl_tier2.h"
/*0069ca34*/ extern undefined4 g_menu_update_69ca34;
/*0069cfd0*/ extern undefined4 g_message_state;
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f007c*/ extern char g_menu_update_7f007c[];

#define PLAYER_INPUT_SIZE (0x4c) /* sizeof(struct PlayerInput) */
#define INPUT_GROUP_STRIDE (0x130) /* four player inputs per pass */
#define CURRENT_INPUTS_OFFSET (-0x4c0) /* previous to current frame inputs (negative offset, inferred) */
#define CURSOR_GROUP_STRIDE (0xc) /* ints of cursor state per pass (four slots, three ints each) */
#define CURSOR_MAX (3) /* last cursor position */

/* menu_player_direction_navigate */

/* "per player slot steps four counters (0..3) from the direction flag edges" */
/* Moves each player's menu cursor (0..CURSOR_MAX) on the frame a left or right
 * press starts: held now (current inputs) and not held on the previous frame.
 * Does nothing while g_message_state is non-zero. Returns nothing.
 */

void menu_player_direction_navigate(void)

{
  int *slot_cursor;
  byte *dir_flags;

  if (g_message_state == 0) {
    slot_cursor = &g_menu_update_69ca34;
    dir_flags = g_player_inputs_previous[0].direction_flags;
    /* One pass covers four player slots: slot s reads its flags at s * PLAYER_INPUT_SIZE
     * and its cursor at slot_cursor[3 * s - 3]. Left decrements, right increments. */
    do {
      if (((dir_flags[CURRENT_INPUTS_OFFSET] != '\0') && (*dir_flags == 0)) && (slot_cursor[-3] != 0)) {
        slot_cursor[-3] = slot_cursor[-3] + -1;
      }
      if (((dir_flags[1 + CURRENT_INPUTS_OFFSET] != '\0') && (dir_flags[1] == 0)) && (slot_cursor[-3] < CURSOR_MAX)) {
        slot_cursor[-3] = slot_cursor[-3] + 1;
      }
      if (((dir_flags[PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET] != '\0') && (dir_flags[PLAYER_INPUT_SIZE] == '\0')) && (*slot_cursor != 0)) {
        *slot_cursor = *slot_cursor + -1;
      }
      if (((dir_flags[PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET + 1] != '\0') && (dir_flags[PLAYER_INPUT_SIZE + 1] == '\0')) && (*slot_cursor < CURSOR_MAX)) {
        *slot_cursor = *slot_cursor + 1;
      }
      if (((dir_flags[2 * PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET] != '\0') && (dir_flags[2 * PLAYER_INPUT_SIZE] == '\0')) && (slot_cursor[3] != 0)) {
        slot_cursor[3] = slot_cursor[3] + -1;
      }
      if (((dir_flags[2 * PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET + 1] != '\0') && (dir_flags[2 * PLAYER_INPUT_SIZE + 1] == '\0')) && (slot_cursor[3] < CURSOR_MAX)) {
        slot_cursor[3] = slot_cursor[3] + 1;
      }
      if (((dir_flags[3 * PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET] != '\0') && (dir_flags[3 * PLAYER_INPUT_SIZE] == '\0')) && (slot_cursor[6] != 0)) {
        slot_cursor[6] = slot_cursor[6] + -1;
      }
      if (((dir_flags[3 * PLAYER_INPUT_SIZE + CURRENT_INPUTS_OFFSET + 1] != '\0') && (dir_flags[3 * PLAYER_INPUT_SIZE + 1] == '\0')) && (slot_cursor[6] < CURSOR_MAX)) {
        slot_cursor[6] = slot_cursor[6] + 1;
      }
      dir_flags = dir_flags + INPUT_GROUP_STRIDE;
      slot_cursor = slot_cursor + CURSOR_GROUP_STRIDE;
    } while (dir_flags < (byte *)&g_menu_update_7f007c);
  }
  return;
}
