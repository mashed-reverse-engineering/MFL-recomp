#include "mfl_tier2.h"
/*00435f10*/ extern int get_career_row();
/*00435f20*/ extern int ui_get_menu_69d690();
/*0069bb5c*/ extern undefined4 g_course_69bb5c;
/*0069bb64*/ extern undefined4 g_course_69bb64;
/*0069bb6c*/ extern float g_course_69bb6c;
/*007eebc0*/ extern struct GameState g_game_state;
/* Entries per career row in the best_* arrays (inferred from their [45] size = 15 rows x 3). */
#define BEST_TIMES_PER_ROW (3)
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* best_time_store_previous: stores the course's best time (minutes, seconds, fraction) into
   g_game_state, at index career_row * BEST_TIMES_PER_ROW + menu_slot (inferred: the slot is the
   menu selection, the row is the current career row). Takes no parameters, returns nothing. */
void best_time_store_previous(void)
{
  g_game_state.best_minutes[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_course_69bb5c;
  g_game_state.best_seconds[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_course_69bb64;
  g_game_state.best_fraction[get_career_row() * BEST_TIMES_PER_ROW + ui_get_menu_69d690()] = g_course_69bb6c;
  return;
}
