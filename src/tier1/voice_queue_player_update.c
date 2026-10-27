#include "mfl_tier2.h"
/*004687f0*/ extern int sound_ducking_adjust_by_event();
/*00468b80*/ extern int voice_queue_remove();
/*00468cd0*/ extern int voice_queue_update();
/*00471ff0*/ extern int sound_stream_start_if_idle();
/*008be200*/ extern undefined g_weapon_8be200;
/*008be7e0*/ extern undefined g_sound_8be7e0;
/*008d9940*/ extern undefined4 g_sound_8d9940;
extern undefined4 __cdecl player_voice_counter_increment(int param_1);


/* voice_queue_player_update */

/* "per player: counts frames (player_voice_counter_increment), updates its queue entries and
   voices" */

/* Per-player voice queue step: when the frame counter ticks and the player's state is 1 and
   g_sound_8d9940[player] is 0, play the head entry's sound and remove it from the queue. */
void voice_queue_player_update(int player_index)
{
  undefined4 *queue_entry;
  int counter_ticked;                  /* nonzero when the per-player frame counter ticked */

  counter_ticked = player_voice_counter_increment(player_index);
  if (((counter_ticked != 0) && (*(int *)(&g_weapon_8be200 + player_index * 4) == 1)) &&
     ((&g_sound_8d9940)[player_index] == 0)) {
    queue_entry = *(undefined4 **)(&g_sound_8be7e0 + player_index * 4);
    sound_ducking_adjust_by_event(*queue_entry,queue_entry[1],queue_entry[2]);
    voice_queue_update(player_index);
    sound_stream_start_if_idle(player_index);
    queue_entry = *(undefined4 **)(&g_sound_8be7e0 + player_index * 4);
    voice_queue_remove(*queue_entry,queue_entry[1],queue_entry[2]);
  }
  return;
}
