#include "mfl_tier2.h"
/*008d8ca0*/ extern undefined g_player_8d8ca0;
/*008d8d3c*/ extern undefined g_player_route_8d8d3c;
/*008d8d40*/ extern undefined g_data_8d8d40;
/*008d8d44*/ extern undefined g_data_8d8d44;
/* player_time_update_cb_408ef0 */

/* function pointer pushed/stored by player_time_update (PUSH 0x408ef0) */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
/* Callback: for player player_ref[0] (stride 0x30c) stores the route-table record index found via hit_info/route_ctx into g_player_8d8ca0 and copies hit_info[0..2] into g_player_route_8d8d3c / 8d8d40 / 8d8d44 (inferred). */
void player_time_update_cb_408ef0(char *unused_1,char *route_ctx,undefined4 *hit_info,char *unused_4,int *player_ref)
{
  int player_offset;
  
  player_offset = *player_ref * 0x30c;
  *(uint *)((char *)&g_player_8d8ca0 + player_offset) =
       (uint)*(ushort *)
              (*(char **)(*(char **)(player_ref[1] + 0x10) +
                       ((uint)*(ushort *)(*(char **)(route_ctx + 4) + 6 + hit_info[6] * 8) +
                       (uint)*(ushort *)(route_ctx + 0x80)) * 4) + 4);
  *(undefined4 *)((char *)&g_player_route_8d8d3c + player_offset) = *hit_info;
  *(undefined4 *)((char *)&g_data_8d8d40 + player_offset) = hit_info[1];
  *(undefined4 *)((char *)&g_data_8d8d44 + player_offset) = hit_info[2];
  return;
}


