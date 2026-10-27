#include "mfl_tier2.h"
/*0040fdc0*/ extern int race_get_phase();
/*0040ff70*/ extern int race_get_last_active_player();
/*0040ffc0*/ extern int round_find_winner();
/*00468a20*/ extern int __cdecl car_is_playable(undefined4 p0);
/*00468ac0*/ extern int voice_queue_add();
/*00469040*/ extern int voice_queue_remove_table_entries();
/*007ef7b8*/ extern undefined4 game_mode;
/*008bed00*/ extern undefined4 g_sound_frame_8bed00;
extern void sound_ducking_values_decay(void);


/* voice_queue_final_lines */

/* "queues the end of race lines (winner / last active player) depending on the race phase and game
   mode" */

void voice_queue_final_lines(void)
{
  int mode;
  int race_phase;
  int last_player;
  int winner;
  int playable;
  
  race_phase = race_get_phase();
  mode = game_mode;
  last_player = race_get_last_active_player();
  winner = round_find_winner();
  if (last_player != -1) {
    if (g_sound_frame_8bed00 == race_phase) {
      return;
    }
    if ((race_phase == 7) && (winner == -1)) {
      voice_queue_remove_table_entries();
      voice_queue_add(1,last_player,0xffffffff);
    }
  }
  else {
    if ((winner == -1) && (race_phase != 0xb)) {
      last_player = 0;
      do {
        playable = car_is_playable(last_player);
        if (playable != 0) {
          voice_queue_add(0x27,last_player,0xffffffff);
        }
        last_player = last_player + 1;
      } while (last_player < 4);
    }
  }
  if (g_sound_frame_8bed00 != race_phase) {
    if ((race_phase == 0xb) && (winner != -1)) {
      voice_queue_remove_table_entries();
      voice_queue_add(0,winner,0xffffffff);
      sound_ducking_values_decay();
    }
    if (((g_sound_frame_8bed00 != race_phase) && (race_phase == 0xb)) && (mode == 4)) {
      voice_queue_remove_table_entries();
      voice_queue_add(0,0,0xffffffff);
      sound_ducking_values_decay();
      return;
    }
  }
  return;
}
