#include "mfl_tier2.h"
/*00659cb8*/ extern undefined4 g_game_659cb8;
/*00659cbc*/ extern undefined4 g_hud_game_659cbc;
/*00659cc0*/ extern undefined4 g_hud_game_659cc0;
/*00659cd0*/ extern undefined4 g_hud_game_659cd0;
/*00659cd4*/ extern undefined4 g_hud_game_659cd4;
/*00659cd8*/ extern undefined4 g_hud_game_659cd8;
/*00659ce4*/ extern undefined4 g_hud_game_659ce4;
/*00659cec*/ extern undefined4 g_hud_game_659cec;
/*00659cf0*/ extern undefined4 g_hud_game_659cf0;
/*00659cf4*/ extern undefined4 g_hud_game_659cf4;
/*00659cf8*/ extern undefined4 g_game_659cf8;
/*00659cfc*/ extern undefined4 g_game_659cfc;


/* hud_game_vars_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the counters at 0x659CB8..0x659CF8" */

void hud_game_vars_clear(void)
{
  g_game_659cb8 = 0;
  g_hud_game_659cbc = 0;
  g_hud_game_659cc0 = 0;
  g_hud_game_659ce4 = 0;
  g_hud_game_659cf0 = 0;
  g_hud_game_659cf4 = 0;
  g_game_659cf8 = 0;
  g_game_659cfc = 0;
  g_hud_game_659cd0 = 0;
  g_hud_game_659cd4 = 0;
  g_hud_game_659cec = 0x47;
  g_hud_game_659cd8 = 0;
  return;
}
