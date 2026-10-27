#include "mfl_tier2.h"
#define CAREER_PROGRESS_ROW_STRIDE 0x12
#define CAREER_PROGRESS_OPEN 1

/*00435f30*/ extern int career_time_under_target();
/*0069d2d0*/ extern undefined4 g_menu_career_race_active;
/*0069d688*/ extern undefined4 g_career_row;
/*007eebc0*/ extern struct GameState g_game_state;


/* career_mark_row_unlocked */

/* Marks the current career row as unlocked (open) if the race is active and target time was beaten. */
void career_mark_row_unlocked(void)
{
  int is_time_under_target;
  
  is_time_under_target = career_time_under_target();
  if ((is_time_under_target != 0) && (g_menu_career_race_active != 0)) {
    /* Set progress for the first event of the row to open */
    g_game_state.progress[g_career_row * CAREER_PROGRESS_ROW_STRIDE] = CAREER_PROGRESS_OPEN;
  }
  return;
}
