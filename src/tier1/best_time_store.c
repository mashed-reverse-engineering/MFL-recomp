#include "mfl_tier2.h"
/*0069bb60*/ extern undefined4 g_best_69bb60;
/*0069bb68*/ extern undefined4 g_best_69bb68;
/*0069bb70*/ extern float g_best_69bb70;
/*007eebc0*/ extern struct GameState g_game_state;
/* best-time slots per career row (inferred: best_minutes[45] = 15 rows x 3) */
#define BEST_TIMES_PER_ROW (3)
/* best_time_store */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* Stores the just-finished race time (minutes, seconds, fraction of a second) in the
 * best-time tables for the current career row and menu slot. Only the first three
 * parameters are used; param_4..param_8 are unused here (inferred). */
void __cdecl best_time_store(float fraction,uint seconds,uint minutes,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8)
{
  g_best_69bb70 = fraction;
  g_best_69bb68 = seconds;
  g_best_69bb60 = minutes;
  g_game_state.best_minutes[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_best_69bb60;
  g_game_state.best_seconds[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_best_69bb68;
  g_game_state.best_fraction[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_best_69bb70;
  return;
}
