#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/* byte offset of the word read by game_state_get_2812 (0xAFC); inside the unnamed gap between enabled[] and option_sliders[] */
#define GAME_STATE_WORD_OFFSET (2812)
/* game_state_get_2812 */
/* Returns the uint at byte offset GAME_STATE_WORD_OFFSET (2812) of g_game_state. That offset is not a named member
   (inferred: unused or unmapped field in the GameState gap). Takes no parameters. */
undefined4 game_state_get_2812(void)
{
  return (*(uint *)((char *)&g_game_state + GAME_STATE_WORD_OFFSET));
}
