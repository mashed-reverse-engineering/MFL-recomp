#include "mfl_tier2.h"
/*008d8ca0*/ extern undefined g_player_8d8ca0;
/*008d8d3c*/ extern undefined g_player_route_8d8d3c;
/*008d8d40*/ extern undefined g_data_8d8d40;
/*008d8d44*/ extern undefined g_data_8d8d44;
/* player_time_update_cb_408ef0 */

/* function pointer pushed/stored by player_time_update (PUSH 0x408ef0) */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
void player_time_update_cb_408ef0(char *unused_1,char *param_2,undefined4 *param_3,char *unused_4,int *param_5)
{
  int iVar1;
  
  iVar1 = *param_5 * 0x30c;
  *(uint *)((char *)&g_player_8d8ca0 + iVar1) =
       (uint)*(ushort *)
              (*(char **)(*(char **)(param_5[1] + 0x10) +
                       ((uint)*(ushort *)(*(char **)(param_2 + 4) + 6 + param_3[6] * 8) +
                       (uint)*(ushort *)(param_2 + 0x80)) * 4) + 4);
  *(undefined4 *)((char *)&g_player_route_8d8d3c + iVar1) = *param_3;
  *(undefined4 *)((char *)&g_data_8d8d40 + iVar1) = param_3[1];
  *(undefined4 *)((char *)&g_data_8d8d44 + iVar1) = param_3[2];
  return;
}


