// cluster-entries: course_warzone_cb1_sub_45e0f0
/* Installed cluster course_warzone_cb1_sub_45e0f0 (tools/cluster_install.py from output/cluster_pilot_kb/course_warzone_cb1_sub_45e0f0.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0045c8f0*/ extern int course_effect_spawn_at_helper_45c8f0();
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*0060d9a8*/ extern undefined4 g_course_atomics_60d9a8;
/*006a2834*/ extern undefined4 g_course_atomics_6a2834;
/* course_warzone_cb1_sub_45e0f0 */

/* single caller course_warzone_cb1 */

/* (inferred names) for each of the 4 players: every trigger whose progress range [key, key+1) holds the player rank key and that has not fired yet (flag in trigger_cursor[0]) spawns an emitter and effect with sounds 0x12 / 0x15 at its point (course+0x5430, 0x40 bytes apart) and is marked fired */
void course_warzone_cb1_sub_45e0f0(int course)

{
  float rank_key;
  int *trigger_cursor;
  int player;
  int point_ptr;
  undefined4 *state_cursor;
  float10 rank_key_x87;
  struct {
    undefined4 dir_x;
    undefined4 dir_y;
    undefined4 dir_z;
  } frame;
  
  player = 0;
  do {
    rank_key_x87 = (float10)player_rank_key(player);
    rank_key = (float)rank_key_x87;
    trigger_cursor = (int *)&g_course_atomics_60d9a8;
    state_cursor = (undefined4 *)&g_course_atomics_6a2834;
    point_ptr = course + 0x5430;
    do {
      if ((((float)trigger_cursor[-1] <= rank_key) && (rank_key < (float)(trigger_cursor[-1] + 1))) &&
         (*trigger_cursor == 0)) {
        frame.dir_x = 0;
        frame.dir_y = 0x3f800000;
        frame.dir_z = 0;
        fx_emitter_spawn_b_default(point_ptr,&frame.dir_x,0x42340000,0x40000000);
        *state_cursor = 2;
        course_effect_spawn_at_helper_45c8f0(point_ptr,0x41f00000);
        *trigger_cursor = 1;
        sfx_play_at(0x12,point_ptr);
        sfx_play_at(0x15,point_ptr);
        sfx_play_player_set_at(point_ptr);
      }
      state_cursor = state_cursor + 2;
      point_ptr = point_ptr + 0x40;
      trigger_cursor = trigger_cursor + 3;
    } while ((int)state_cursor < (int)((undefined *)&g_course_atomics_6a2834) + 0x40);
    player = player + 1;
  } while (player < 4);
  return;
}

