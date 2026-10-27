// cluster-entries: score_panel_entry_init
/* Installed cluster score_panel_entry_init (tools/cluster_install.py from output/cluster_pilot11/score_panel_entry_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c5380*/ extern void FUN_004c5380(undefined4 p0, undefined4 p1);
/*00658dbc*/ extern int g_score_panel_658dbc;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* score_panel_entry_init */

/* "per player entry of the panel.txd score panel: bit mask from the team table, colour 0xFF323232"
    */

void score_panel_entry_init_n(int in_esi)

{
  unsigned char local_4[8];  /* 8: with 4 bytes cl reuses the (dead) argument slot as the colour, which the frame window of diffemu cannot see */
  
  *(undefined4 *)(in_esi + 0x68) = 0;
  *(undefined4 *)(in_esi + 0x6c) = 0;
  local_4[0] = 0x32;
  local_4[1] = 0x32;
  local_4[2] = 0x32;
  *(int *)(in_esi + 0x68) = 1 << g_player_slots[*(int *)(in_esi + 100)].team;
  local_4[3] = 0xff;
  FUN_004c5380(*(undefined4 *)(in_esi + 0x14),local_4);
  FUN_004c5380(*(undefined4 *)(in_esi + 8),local_4);
  if (*(int *)(in_esi + 100) < g_score_panel_658dbc) {
    *(undefined4 *)(in_esi + 0x6c) = 1;
  }
  return;
}

/* entry thunk at the original address of score_panel_entry_init: register inputs (esi) -> natural C score_panel_entry_init_n; ret 0 */
__declspec(naked) void score_panel_entry_init(void)
{
    __asm {
        push esi
        call score_panel_entry_init_n
        add esp, 4
        ret
    }
}
