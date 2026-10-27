#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;


/* option_slider_2_reset */

/* "g_game_state.option_sliders[2] = 0.7" */

void option_slider_2_reset(void)
{
  g_game_state.option_sliders[2] = 0x3f333333;
  return;
}
