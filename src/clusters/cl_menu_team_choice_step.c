// cluster-entries: menu_team_choice_step
/* Installed cluster menu_team_choice_step (tools/cluster_install.py from output/cluster_pilot11/menu_team_choice_step.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfb8*/ extern undefined4 g_menu_player_team_choice_1;
/*0069cfbc*/ extern undefined4 g_menu_player_team_choice_2;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* menu_team_choice_step */

/* "steps the team choice of player (in EAX) by +-1 skipping choices already taken (wraps 0..6)" */

void menu_team_choice_step_n(int player, int step, int skip_none_flag)

{
  int old_choice;
  bool moved;
  int taken_choice;
  
  if ((&g_menu_player_team_choice)[player] != 0) {
    moved = false;
    taken_choice = g_player_slots[0].team + 1;
    do {
      if ((&g_menu_player_team_choice)[player] == taken_choice) {
        moved = true;
        do {
          old_choice = (&g_menu_player_team_choice)[player];
          (&g_menu_player_team_choice)[player] = old_choice + step;
        } while (old_choice + step == taken_choice);
      }
      if ((player != 0) && ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice)) {
        do {
          if ((&g_menu_player_team_choice)[player] == 0) break;
          (&g_menu_player_team_choice)[player] = (&g_menu_player_team_choice)[player] + step;
          moved = true;
        } while ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice);
      }
      if ((player != 1) && ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice_1))
      {
        do {
          if ((&g_menu_player_team_choice)[player] == 0) break;
          (&g_menu_player_team_choice)[player] = (&g_menu_player_team_choice)[player] + step;
          moved = true;
        } while ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice_1);
      }
      if ((player != 2) && ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice_2))
      {
        do {
          if ((&g_menu_player_team_choice)[player] == 0) break;
          (&g_menu_player_team_choice)[player] = (&g_menu_player_team_choice)[player] + step;
          moved = true;
        } while ((&g_menu_player_team_choice)[player] == g_menu_player_team_choice_2);
      }
      moved = (bool)(moved ^ 1);
      if (step == 1) {
        if (skip_none_flag == 0) {
          if ((&g_menu_player_team_choice)[player] == 7) {
            (&g_menu_player_team_choice)[player] = 1;
          }
        }
        else if ((&g_menu_player_team_choice)[player] == 7) {
          (&g_menu_player_team_choice)[player] = 0;
        }
      }
      else if (((skip_none_flag == 0) && ((&g_menu_player_team_choice)[player] == 0)) ||
              ((&g_menu_player_team_choice)[player] == -1)) {
        (&g_menu_player_team_choice)[player] = 6;
      }
    } while (!moved);
  }
  return;
}

/* entry thunk at the original address of menu_team_choice_step: register inputs (eax,esi,s0) -> natural C menu_team_choice_step_n; ret 0 */
__declspec(naked) void menu_team_choice_step(void)
{
    __asm {
        push eax
        push dword ptr [esp + 8]
        push esi
        push eax
        call menu_team_choice_step_n
        add esp, 12
        pop eax
        ret
    }
}
