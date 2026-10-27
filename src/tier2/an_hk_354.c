#include "mfl_tier2.h"
/*0069cf90*/ extern undefined4 g_menu_powerup_set;
/*0069cfac*/ extern undefined4 g_menu_bot_skill_b;
/*0069cfb0*/ extern undefined4 g_menu_event_course_index;
/*0069cfb4*/ extern undefined4 g_menu_player_team_choice;
/*0069cfb8*/ extern undefined4 g_menu_player_team_choice_1;
/*0069cfbc*/ extern undefined4 g_menu_player_team_choice_2;
/*0069cfc8*/ extern undefined4 g_menu_team_mode_choice;
static int menu_team_choice_step_r(int reg_eax, int reg_esi, int param_3);
extern void menu_team_choice_step(void);
__declspec(naked) int menu_team_choice_step_r(int reg_eax, int reg_esi, int unused_3)
{
    __asm {
        push esi
        push dword ptr [esp + 16]
        mov eax, dword ptr [esp + 12]
        mov esi, dword ptr [esp + 16]
        call menu_team_choice_step
        add esp, 4
        pop esi
        ret
    }
}

/* menu_options_defaults_reset */

/* "sets the menu option variables to their defaults and refreshes the team entries" */

/*
 * Resets the menu option globals to their default values, then refreshes the
 * team entries. Takes no parameters and returns nothing.
 *   g_menu_powerup_set, g_menu_bot_skill_b: 1 (default on)
 *   g_menu_event_course_index: 0 (inferred: first event course)
 *   g_menu_player_team_choice, _1, _2: 1, 2, 3 (inferred: one per player slot)
 *   g_menu_team_mode_choice: 0
 */
void menu_options_defaults_reset(void)

{
  int team_step_result;

  g_menu_powerup_set = 1;
  g_menu_bot_skill_b = 1;
  g_menu_event_course_index = 0;
  g_menu_player_team_choice = 1;
  g_menu_player_team_choice_1 = 2;
  g_menu_player_team_choice_2 = 3;
  g_menu_team_mode_choice = 0;
  /* refresh the team entries (inferred: called once per slot 0..2, the result of slot 0 is passed to slot 1) */
  team_step_result = menu_team_choice_step_r(0,1,0);
  menu_team_choice_step_r(1,1,team_step_result);
  menu_team_choice_step_r(2,1,0);
  return;
}


