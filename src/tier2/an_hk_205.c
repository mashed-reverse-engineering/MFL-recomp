#include "mfl_tier2.h"
/*00430b50*/ extern int ui_input_mode_menu();
/*00431a20*/ extern int ui_get_message_state();
/*00446310*/ extern int quick_race_defaults_set();
/*00481fc0*/ extern int gfx_set_89ddfc();
/*004c7cc0*/ extern undefined __cdecl message(undefined4 p0, undefined4 p1);
/*0064d8a8*/ extern float g_attract_timer_64d8a8;
/*0064d8ac*/ extern float g_attract_timer_64d8ac;
/*0064d8b8*/ extern undefined4 g_attract_timer_64d8b8;
/*007ef7ec*/ extern float g_frame_seconds;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efa8a*/ extern char g_player_inputs_8__menu_select[];

#define PLAYER_INPUT_STRIDE (0x4c) /* sizeof(struct PlayerInput) in bytes */
#define PLAYER_INPUT_CLEAR_DWORDS (0x98) /* 4-byte steps to clear the direction bytes: 8 players x 76 bytes / 4 (inferred) */

/* attract_timer_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "adds g_frame_seconds while no message is shown; after the limit starts the attract / demo" */

/* Per-frame attract / demo timer. While no message is shown, adds g_frame_seconds to the
   attract timer. Once it passes g_attract_timer_64d8ac it either runs the quick race defaults
   (flag 64d8b8 == 0) or clears the held directions and restarts the timer (flag != 0); the flag
   is then toggled. Also sets menu_select for every player whose pause byte is set (inferred).
   Takes no parameters and returns nothing. */
void attract_timer_update(void)

{
  int message_state;
  byte *menu_select_ptr;
  int *clear_ptr;

  message_state = ui_get_message_state();
  if (message_state == 0) {
    g_attract_timer_64d8a8 = g_attract_timer_64d8a8 + g_frame_seconds;
    gfx_set_89ddfc(message_state);
    ui_input_mode_menu();
    /* timer expired: start the attract demo, or restart the timer */
    if (g_attract_timer_64d8a8 > g_attract_timer_64d8ac) {
      if (g_attract_timer_64d8b8 != 0) {
        clear_ptr = (int *)g_player_inputs;
        for (message_state = PLAYER_INPUT_CLEAR_DWORDS;message_state != 0; message_state = message_state + -1) {
          *clear_ptr = 0;
          clear_ptr = clear_ptr + 1;
        }
        g_attract_timer_64d8a8 = 0.0;
      }
      else {
        quick_race_defaults_set();
      }
      g_attract_timer_64d8b8 = g_attract_timer_64d8b8 - 1 & 1;
    }
    /* for each player: when the byte before menu_select (pause, inferred) is set, set menu_select */
    menu_select_ptr = &g_player_inputs[0].menu_select;
    do {
      if (menu_select_ptr[-1] != 0) {
        *menu_select_ptr = 1;
      }
      menu_select_ptr = menu_select_ptr + PLAYER_INPUT_STRIDE;
    } while ((LONG_PTR)menu_select_ptr < ((LONG_PTR)&g_player_inputs_8__menu_select));
  }
  return;
}


