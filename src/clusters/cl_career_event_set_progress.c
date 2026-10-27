// cluster-entries: career_event_set_progress
/* Installed cluster career_event_set_progress (tools/cluster_install.py from output/cluster_pilot_j1/career_event_set_progress.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char PTR_LAB_004361b0[];
/*00608428*/ extern undefined4 g_menu_608428;
/*0069cbd4*/ extern undefined4 g_event_type;
/*0069d688*/ extern undefined4 g_career_row;
/*007eebc0*/ extern struct GameState g_game_state;
/* career_event_set_progress */

/* "switches on the event type and writes the progress entry of the player" */

undefined4 __cdecl career_event_set_progress(int player)
{
  uint type = g_event_type;
  int *progress = &g_game_state.progress[player * 0x12];

  if (type == 3 || type == 4 || type == 5 || type == 0xe || type == 0xf || type == 0x10) {
    if (0x11 < (int)type) return 0;
  }
  else if (type != 0x11) {
    if ((&g_menu_608428)[g_career_row * 0xc] == -1) {
      /* first jump table: a reset of the entry the event type owns */
      if (0x11 < type) return 0;
      switch (type) {
      case 0: case 1: case 2: progress[2] = 0; break;
      case 6: case 7: case 8: progress[8] = 0; break;
      case 0xb: case 0xc: case 0xd: progress[0xd] = 0; break;
      case 0xe: progress[0xe] = 0; break;
      case 0xf: progress[0xf] = 0; break;
      case 0x10: progress[0x10] = 0; break;
      case 0x11: progress[0x11] = 0; break;
      default: break;     /* 3, 4, 5, 9, 10: fall to the range check */
      }
    }
    else if (0x11 < (int)type) return 0;
  }
  /* second jump table: read the entry */
  if (type == 9 || type == 10) return 0;
  return progress[type];
}

