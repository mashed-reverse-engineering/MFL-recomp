// cluster-entries: course_warzone_cb1_sub_45db70
/* Installed cluster course_warzone_cb1_sub_45db70 (tools/cluster_install.py from output/cluster_pilot_kb/course_warzone_cb1_sub_45db70.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00409eb0*/ extern double player_rank_key();
/*0045da60*/ extern int course_warzone_cb1_sub_45da60();
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*0060d9d8*/ extern undefined4 g_course_player_60d9d8[0x1000];
/* course_warzone_cb1_sub_45db70 */

/* single caller course_warzone_cb1 */

/* For each of 4 players, trigger a 3-sound effect once when the player's rank key passes a table threshold entry (entry[-1] <= key < entry[-1]+1) not yet fired (inferred). */
void course_warzone_cb1_sub_45db70(void)

{
  float rank_key;
  undefined4 sound_pos;
  int slot;
  int player;
  int *entry;
  float10 rank_key_x87;
  
  player = 0;
  do {
    rank_key_x87 = (float10)player_rank_key(player);
    rank_key = (float)rank_key_x87;
    slot = 0;
    entry = g_course_player_60d9d8;
    do {
      if ((((float)entry[-1] <= rank_key) && (rank_key < (float)(entry[-1] + 1))) &&
         (*entry == 0)) {
        sound_pos = course_warzone_cb1_sub_45da60(slot);
        *entry = 1;
        sfx_play_at(0x12,sound_pos);
        sfx_play_at(0x15,sound_pos);
        sfx_play_player_set_at(sound_pos);
      }
      entry = entry + 3;
      slot = slot + 1;
    } while ((int)entry < (int)g_course_player_60d9d8 + 0x60);
    player = player + 1;
  } while (player < 4);
  return;
}

