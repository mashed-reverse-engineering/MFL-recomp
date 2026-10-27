#include "mfl_tier2.h"
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];    /* per race statistics, 0x138-byte records (source) */
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];  /* totals (destination), same layout */


/* player_stats_copy_to_totals: copies the 4 per-race statistic records into the totals table, and adds 1 to each record's place field. */

void player_stats_copy_to_totals(void)
{
  undefined4 field_a;
  undefined4 field_b;
  int place;
  int record_offset;   /* byte offset of the current 0x138-byte record */

  record_offset = 0;
  do {
    /* the copied fields are read and written in an interleaved order (as in the original) */
    field_a = *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x6c);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x18) =
         *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x18);
    field_b = *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x14);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x6c) = field_a;
    field_a = *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x68);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x14) = field_b;
    field_b = *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x1c);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x68) = field_a;
    field_a = *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -100);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x1c) = field_b;
    place = *(int *)((char *)&g_player_stats_race[0].place + record_offset);
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -100) = field_a;
    *(undefined4 *)((int)g_player_stats_result[0].last_collision_time + record_offset + -0x10) =
         *(undefined4 *)((int)g_player_stats_race[0].last_collision_time + record_offset + -0x10);
    /* place of the totals record = place of the race record + 1 */
    *(int *)((char *)&g_player_stats_result[0].place + record_offset) = place + 1;
    record_offset = record_offset + 0x138;
  } while (record_offset < 0x4e0);   /* 0x4e0 = 4 records of 0x138 */
  return;
}
