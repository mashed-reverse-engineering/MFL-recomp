#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*00434a10*/ extern int is_team_mode();
/*007ef7b8*/ extern undefined4 game_mode;


/* score_target_b */

/* "second score threshold: 10 (4 players) or 7 in team mode (inferred)" */

int score_target_b(void)

{
  int target;

  target = (get_player_count() == 4) ? 10 : 7;
  if (is_team_mode()) {
    target = 7;
  }
  if (game_mode == GAME_MODE_HOLD_THE_FLAG) {
    return 7;
  }
  if (game_mode == GAME_MODE_HARE_AND_HOUNDS) {
    return 7;
  }
  return target;
}
