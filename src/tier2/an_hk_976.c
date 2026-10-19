#include "mfl_tier2.h"
/*00435f10*/ extern int get_career_row();
/*00435f20*/ extern int ui_get_menu_69d690();
/*00435fb0*/ extern int ui_get_menu_69d2d0();
/*0062ac98*/ extern undefined g_ui_62ac98;
/*0069bb60*/ extern undefined4 g_best_69bb60;
/*0069bb68*/ extern undefined4 g_best_69bb68;
/*0069bb70*/ extern float g_best_69bb70;
/*007eebc0*/ extern struct GameState g_game_state;
/* best_time_load_current */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void best_time_load_current(void)

{
  int course;
  int rank;
  
  course = ui_get_menu_69d2d0();
  if (course != 0) {
    /* best time of the career row from the float table at g_ui_62ac98 - 8 (12 bytes per row: minutes, seconds, fraction) */
    g_best_69bb60 = (int)*(float *)((char *)&g_ui_62ac98 - 8 + get_career_row() * 0xc);
    g_best_69bb68 = (int)*(float *)((char *)&g_ui_62ac98 - 4 + get_career_row() * 0xc);
    g_best_69bb70 = *(float *)((char *)&g_ui_62ac98 + get_career_row() * 0xc);
    return;
  }
  course = get_career_row();
  rank = ui_get_menu_69d690();
  g_best_69bb60 = g_game_state.best_minutes[course * 3 + rank];
  course = get_career_row();
  rank = ui_get_menu_69d690();
  g_best_69bb68 = g_game_state.best_seconds[course * 3 + rank];
  course = get_career_row();
  rank = ui_get_menu_69d690();
  g_best_69bb70 = g_game_state.best_fraction[course * 3 + rank];
  return;
}


