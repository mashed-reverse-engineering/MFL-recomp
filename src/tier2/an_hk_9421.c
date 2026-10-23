#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* career_flag_enabled */
/* "tests one enabled flag of a player slot: flag ids 0..0x11 or 1000..1017 select byte 0..0x11 of the slot's 18 byte row; ids 6..10 do not exist" */
/* TIER2: validated 2026-10-21 authority=diffemu */
int __cdecl career_flag_enabled(int slot, int flag)
{
  int column;

  column = flag;
  if (flag >= 1000) {
    column = flag - 1000;
  }
  if (column < 0 || column > 0x11) {
    return 0;
  }
  if (column >= 6 && column <= 10) {
    return 0;
  }
  return g_game_state.enabled[slot * 0x12 + column] == 1;
}
