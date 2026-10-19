// cluster-entries: menu_team_choice_step
/* Installed cluster menu_team_choice_step (tools/cluster_install.py from output/cluster_pilot11/menu_team_choice_step.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfb8*/ extern undefined4 g_menu_player_team_choice_1;
/*0069cfbc*/ extern undefined4 g_menu_player_team_choice_2;
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/* menu_team_choice_step */

/* WARNING: Unknown calling convention */
/* "steps the team choice of player (in EAX) by +-1 skipping choices already taken (wraps 0..6)" */

void menu_team_choice_step_n(int in_eax, int in_esi, int param_3)

{
  int iVar1;
  bool bVar2;
  int iVar3;
  
  if ((&g_menu_player_team_choice)[in_eax] != 0) {
    bVar2 = false;
    iVar3 = g_player_slots[0].team + 1;
    do {
      if ((&g_menu_player_team_choice)[in_eax] == iVar3) {
        bVar2 = true;
        do {
          iVar1 = (&g_menu_player_team_choice)[in_eax];
          (&g_menu_player_team_choice)[in_eax] = iVar1 + in_esi;
        } while (iVar1 + in_esi == iVar3);
      }
      if ((in_eax != 0) && ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice)) {
        do {
          if ((&g_menu_player_team_choice)[in_eax] == 0) break;
          (&g_menu_player_team_choice)[in_eax] = (&g_menu_player_team_choice)[in_eax] + in_esi;
          bVar2 = true;
        } while ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice);
      }
      if ((in_eax != 1) && ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice_1))
      {
        do {
          if ((&g_menu_player_team_choice)[in_eax] == 0) break;
          (&g_menu_player_team_choice)[in_eax] = (&g_menu_player_team_choice)[in_eax] + in_esi;
          bVar2 = true;
        } while ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice_1);
      }
      if ((in_eax != 2) && ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice_2))
      {
        do {
          if ((&g_menu_player_team_choice)[in_eax] == 0) break;
          (&g_menu_player_team_choice)[in_eax] = (&g_menu_player_team_choice)[in_eax] + in_esi;
          bVar2 = true;
        } while ((&g_menu_player_team_choice)[in_eax] == g_menu_player_team_choice_2);
      }
      bVar2 = (bool)(bVar2 ^ 1);
      if (in_esi == 1) {
        if (param_3 == 0) {
          if ((&g_menu_player_team_choice)[in_eax] == 7) {
            (&g_menu_player_team_choice)[in_eax] = 1;
          }
        }
        else if ((&g_menu_player_team_choice)[in_eax] == 7) {
          (&g_menu_player_team_choice)[in_eax] = 0;
        }
      }
      else if (((param_3 == 0) && ((&g_menu_player_team_choice)[in_eax] == 0)) ||
              ((&g_menu_player_team_choice)[in_eax] == -1)) {
        (&g_menu_player_team_choice)[in_eax] = 6;
      }
    } while (!bVar2);
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
