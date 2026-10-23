#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*00801b44*/ extern undefined4 g_savegame_replay[0x1000];
/*008432d0*/ extern undefined4 g_savegame_state[0x1000];
/*00843dc8*/ extern undefined4 g_gamesave_843dc8;
/*008d8b08*/ extern char *g_replay_buffer;
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;
extern void *memcpy(void *, const void *, unsigned int);
#pragma intrinsic(memcpy)
/* gamesave_apply */

/* g_savegame -> live state (replay buffer and game state block) */

void gamesave_apply(void)

{
  uint *save_src;
  int count;

  memcpy(&g_game_state, g_savegame_state, 0x2d9 * 4);
  count = 0;
  save_src = &g_player_inputs[0].rumble_enabled;
  do {
    *save_src = (uint)g_game_state.player_bytes[count];
    save_src = save_src + 0x13;
    count = count + 1;
  } while ((char *)save_src < (char *)&g_player_inputs + 0x3b4);
  g_gamesave_flag_c0c = g_gamesave_843dc8;
  if (g_replay_buffer != 0) {
    memcpy((void *)g_replay_buffer, g_savegame_replay, 0x105e3 * 4);
  }
  return;
}
