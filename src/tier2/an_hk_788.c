#include "mfl_tier2.h"
/*0040e610*/ extern int race_setup_use_0();
/*00429680*/ extern int best_times_init_defaults();
/*00437710*/ extern int option_slider_1_reset();
/*00437720*/ extern int option_slider_2_reset();
/*00437730*/ extern int option_slider_0_reset();
/*00437740*/ extern int option_slider_4_reset();
/*00437930*/ extern int menu_options_defaults_reset();
/*0050b5e0*/ extern int nop_stub();
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef740*/ extern undefined4 g_game_options_7ef740[0x1000];
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*00801b2c*/ extern float g_camera_fov_factor;
/*007ef124*/ extern char g_game_state_field2_0x564[];
/*007ef00c*/ extern char g_game_state_records_5_[];
/*007ef048*/ extern char g_game_state_records_20_[];
/*007ef598*/ extern char g_enabled_flags[];
/*007efaa4*/ extern char g_player_inputs_8__field27_0x24[];

/* words cleared by the reset loop; larger than g_game_options_7ef740[0x1000], so the loop writes past it (check disassembly) */
#define OPTIONS_CLEAR_WORD_COUNT (0x151d1)
/* progress columns per row (18) */
#define PROGRESS_COLUMNS (0x12)
/* progress value for a finished entry (0 locked, 1 open, 2 done) */
#define PROGRESS_DONE (2)
/* best-record value meaning no record yet */
#define RECORD_EMPTY (-1)
/* sizeof(struct PlayerInput) / 4 = 0x4c / 4 */
#define PLAYER_INPUT_WORDS (0x13)
/* default camera field-of-view factor */
#define CAMERA_FOV_FACTOR_DEFAULT (0.75f)

/*
 * Resets the game options to their defaults: clears the options block, race setup 0,
 * option flags, mirror mode, race timer and game mode, the option sliders, and the
 * per-player rumble flags; rebuilds the progress / record / enabled tables; then
 * restores the best times, camera FOV factor and menu defaults.
 * Takes no parameters. Always returns 1.
 * (was: "resets sliders, race setup 0 and options to defaults")
 */

undefined4 game_options_reset(void)

{
  uint *player_input;
  int row_offset;
  int column;
  int *row_records;
  undefined4 *options_cursor;

  /* clear the options block one 32-bit word at a time */
  options_cursor = g_game_options_7ef740;
  for (row_offset = OPTIONS_CLEAR_WORD_COUNT; row_offset != 0; row_offset = row_offset + -1) {
    *options_cursor = 0;
    options_cursor = options_cursor + 1;
  }
  race_setup_use_0();
  g_game_state.option_flags[0] = 0;
  g_game_state.option_flags[1] = 0;
  g_game_state.option_flags[2] = 0;
  g_game_state.option_flags[3] = 0;
  g_game_state.option_flags[4] = 0;
  g_game_state.mirror_mode = 0;
  g_race_time_ticks = 0;
  game_mode = 0;
  option_slider_1_reset();
  option_slider_2_reset();
  option_slider_0_reset();
  nop_stub();
  option_slider_4_reset();
  /* set rumble_enabled = 1 on player inputs 0..7 (inferred: loop stops at input 8's field_0x24) */
  player_input = &g_player_inputs[0].rumble_enabled;
  do {
    *player_input = 1;
    player_input = player_input + PLAYER_INPUT_WORDS;
  } while ((int)player_input < ((int)&g_player_inputs_8__field27_0x24));
  /* rebuild the tables row by row: 15 rows, each with 5 records and 18 progress columns */
  row_offset = 0;
  row_records = g_game_state.records;
  do {
    *row_records = RECORD_EMPTY;
    row_records[1] = RECORD_EMPTY;
    row_records[2] = RECORD_EMPTY;
    row_records[3] = RECORD_EMPTY;
    row_records[4] = RECORD_EMPTY;
    column = 0;
    do {
      /* every entry starts selectable (enabled = 1) */
      *(undefined1 *)(row_offset + ((int)&g_enabled_flags) + column) = 1;
      /* progress for this entry: the cases that test row_records compare the row's
         records against the thresholds g_game_state_records_5_ / _20_ (inferred) */
      switch(column) {
      case 0:
      case 1:
      case 2:
      case 0xe:
      case 0xf:
      case 0x10:
        g_game_state.progress[row_offset + column] = 0;
        break;
      case 3:
        g_game_state.progress[row_offset + 3] = 0;
        if ((int)row_records < ((int)&g_game_state_records_5_)) {
          g_game_state.progress[row_offset + 3] = 1;
        }
        break;
      case 4:
        g_game_state.progress[row_offset + 4] = 0;
        break;
      case 5:
        g_game_state.progress[row_offset + 5] = 0;
        break;
      case 6:
        if ((int)row_records < ((int)&g_game_state_records_20_)) {
          g_game_state.progress[row_offset + 6] = PROGRESS_DONE;
        }
        break;
      case 7:
        g_game_state.progress[row_offset + 7] = 0;
        break;
      case 8:
        g_game_state.progress[row_offset + 8] = 0;
        break;
      case 9:
        g_game_state.progress[row_offset + 9] = 0;
        break;
      case 10:
        g_game_state.progress[row_offset + 10] = 0;
        break;
      case 0xb:
        g_game_state.progress[row_offset + 0xb] = (uint)((int)row_records < ((int)&g_game_state_records_5_));
        break;
      case 0x11:
        g_game_state.progress[row_offset + 0x11] = 0;
        if ((int)row_records < ((int)&g_game_state_records_5_)) {
          g_game_state.progress[row_offset + 0x11] = 1;
        }
      }
      column = column + 1;
    } while (column < PROGRESS_COLUMNS);
    row_records = row_records + 5;
    row_offset = row_offset + PROGRESS_COLUMNS;
  } while ((int)row_records < ((int)&g_game_state_field2_0x564));
  best_times_init_defaults();
  g_camera_fov_factor = CAMERA_FOV_FACTOR_DEFAULT;
  g_camera_7ef7d8 = 0;
  menu_options_defaults_reset();
  return 1;
}


