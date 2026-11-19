#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*00434a10*/ extern int is_team_mode();
/*007ef7b8*/ extern undefined4 game_mode;


/* score_target_a */

/* "first score threshold: 12 (4 players) or 8 players otherwise, 8 in team mode (inferred)" */

int score_target_a(void)

{
  int target;

  target = (get_player_count() == 4) ? 12 : 8;
  if (is_team_mode()) {
    target = 8;
  }
  if (game_mode == GAME_MODE_HOLD_THE_FLAG) {
    return 8;
  }
  if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
    return 8;
  }
  return target;
}
