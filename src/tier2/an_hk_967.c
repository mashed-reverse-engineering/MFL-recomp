#include "mfl_tier2.h"
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*00801b40*/ extern undefined4 g_savegame;
/*00801b44*/ extern undefined4 g_savegame_replay[0x1000];
/*008432d0*/ extern undefined4 g_savegame_state[0x1000];
#define SAVE_FLAG_INDEX (0x2be)      /* dword of g_savegame_state at 0x00843dc8 */
/*008d8b08*/ extern undefined4 g_replay_buffer;
/*008d8c0c*/ extern undefined4 g_gamesave_flag_c0c;
typedef struct { undefined4 d[0x2d9]; } Dw0x2d9;      /* the 0xB64 byte game state block */
typedef struct { undefined4 d[0x105e3]; } DwReplay;      /* replay buffer */
/* gamesave_gather */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* live state -> g_savegame (sets 0xDEADBEEF; copies the replay buffer and the 0xB64 byte game state
   block) */

void gamesave_gather(void)

{
  uint *player_input_byte;
  int count;
  
  count = 0;
  player_input_byte = &g_player_inputs[0].rumble_enabled;
  do {
    g_game_state.player_bytes[count] = (byte)*player_input_byte;
    player_input_byte = player_input_byte + 0x13;
    count = count + 1;
  } while ((int)player_input_byte < (int)&g_player_inputs + 0x3b4);
  *(Dw0x2d9 *)g_savegame_state = *(Dw0x2d9 *)&g_game_state;
  g_savegame_state[SAVE_FLAG_INDEX] = g_gamesave_flag_c0c;      /* after the block copy (the slot lies inside the copied block); written through the array so the compiler keeps the order */
  if (g_replay_buffer != 0) {
    *(DwReplay *)g_savegame_replay = *(DwReplay *)g_replay_buffer;
  }
  g_savegame = 0xdeadbeef;
  return;
}


