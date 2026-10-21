#include "mfl_tier2.h"
/*0040e660*/ extern int race_setup_use_5();
/*0040ff40*/ extern int race_setup_set_slot_type();
/*00434a10*/ extern int is_team_mode();
/*00435fc0*/ extern int __cdecl career_event_set_progress(undefined4 p0);
/*00436520*/ extern int player_slots_count_active();
/*004a20c0*/ extern int get_app_state();
/*004a5b20*/ extern int __cdecl controller_present_004a70c0(int p0);
/*0069cc40*/ extern undefined4 g_team_mode;
/*0069cf90*/ extern undefined4 g_menu_powerup_set;
/*0069cf94*/ extern undefined4 g_menu_markers_enabled;
/*0069cf98*/ extern undefined4 g_menu_bot_skill_a;
/*0069cfa4*/ extern undefined4 g_menu_game_mode_choice;
/*0069cfa8*/ extern undefined4 g_menu_player_count_index;
/*0069d2e0*/ extern struct MenuRecord g_menu_records[14];
/*0069d688*/ extern undefined4 g_career_row;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d0*/ extern undefined4 g_menu_7ef7d0;
/*008c8080*/ extern undefined4 g_menu_8c8080;
/*008c8084*/ extern undefined4 g_menu_8c8084;
/*008c8088*/ extern undefined4 g_menu_page_8c8088;
/*008c808c*/ extern undefined4 g_menu_page_8c808c;
/*008c80a0*/ extern struct MenuItemAnim g_menu_item_anims[30];
/* menu_page_update_entry_states */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(page): enables / disables the entries of a menu page from the career progress, player count and
   options" */

void menu_page_update_entry_states(int param_1)

{
  int controller_index;
  int iVar1;
  int *piVar2;
  int *piVar3;
  bool bVar4;
  
  piVar2 = g_menu_records[param_1 + 1].entry_enabled;
  piVar3 = piVar2;
  for (controller_index = 0xc; iVar1 = g_menu_player_count_index, controller_index != 0;
      controller_index = controller_index + -1) {
    *piVar3 = 1;
    piVar3 = piVar3 + 1;
  }
  switch(g_menu_records[param_1 + 1].page_id) {
  case 1:
    if (g_game_state.option_flags[0] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[3] = 0;
    }
    g_menu_7ef7d0 = 0;
    controller_index = 0;
    do {
      race_setup_set_slot_type(controller_index,0);
      controller_index = controller_index + 1;
    } while (controller_index < 4);
    break;
  case 2:
    if (g_game_state.progress[0xe] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[3] = 0;
    }
    if (g_game_state.progress[0xf] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[4] = 0;
    }
    if (g_game_state.progress[0x10] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[5] = 0;
    }
    break;
  case 3:
    g_menu_powerup_set = 1;
    race_setup_use_5();
    controller_index = 0;
    do {
      race_setup_set_slot_type(controller_index,0);
      controller_index = controller_index + 1;
    } while (controller_index < 4);
    break;
  case 10:
    if (g_menu_player_count_index != 2) {
      g_menu_records[param_1 + 1].entry_enabled[1] = 0;
    }
    if (iVar1 != 3) {
      g_menu_records[param_1 + 1].entry_enabled[2] = 0;
    }
    break;
  case 0x12:
    if (g_menu_game_mode_choice == 1) {
      g_menu_records[param_1 + 1].entry_enabled[3] = 0;
      g_menu_bot_skill_a = 0;
LAB_0043872d:
      controller_index = player_slots_count_active();
      if (controller_index == 2) {
        g_menu_records[param_1 + 1].entry_enabled[5] = 0;
        g_menu_markers_enabled = 0;
      }
    }
    else {
      if (g_menu_bot_skill_a == 0) goto LAB_0043872d;
      g_menu_records[param_1 + 1].entry_enabled[5] = 1;
    }
    if (g_game_state.progress[g_career_row * 0x12 + 9] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[6] = 0;
      g_menu_game_mode_choice = 0;
    }
    else {
      g_menu_records[param_1 + 1].entry_enabled[6] = 1;
    }
    bVar4 = game_mode == 2;
    g_menu_records[param_1 + 1].entry_enabled[1] = 1;
    if (bVar4) {
      g_team_mode = 0;
    }
    controller_index = player_slots_count_active();
    if ((controller_index != 4) && (controller_index = is_team_mode(), controller_index == 0))
    break;
    goto LAB_004387b7;
  case 0x18:
    controller_index = career_event_set_progress(1);
    if (controller_index != 0) break;
LAB_004387b7:
    g_menu_records[param_1 + 1].entry_enabled[3] = 0;
    break;
  case 0x1c:
    *piVar2 = 1;
    g_menu_records[param_1 + 1].entry_enabled[1] = 1;
    g_menu_records[param_1 + 1].entry_enabled[2] = 1;
    g_menu_records[param_1 + 1].entry_enabled[3] = 1;
    controller_index = 0;
    do {
      iVar1 = controller_present_004a70c0(controller_index);
      if (iVar1 == 0) {
        *piVar2 = 0;
      }
      controller_index = controller_index + 1;
      piVar2 = piVar2 + 1;
    } while (controller_index < 4);
    g_menu_8c8080 = 0;
    g_menu_8c8084 = 1;
    g_menu_page_8c8088 = 2;
    g_menu_page_8c808c = 3;
    g_menu_item_anims[1].ref_a = 0x23b;
    g_menu_item_anims[2].ref_a = 0x23c;
    g_menu_item_anims[3].ref_a = 0x23d;
    g_menu_item_anims[4].ref_a = 0x23e;
    break;
  case 0x22:
    if (g_game_state.progress[0x39] == 0) {
      g_menu_records[param_1 + 1].entry_enabled[1] = 0;
    }
    break;
  case 0x25:
    controller_index = get_app_state();
    if (controller_index != 1) {
      *piVar2 = 0;
      g_menu_records[param_1 + 1].entry_enabled[1] = 0;
    }
  }
  controller_index = g_menu_records[param_1 + 1].selected;
  if (controller_index == -1) {
    controller_index = 0;
  }
  if (g_menu_records[param_1 + 1].entry_enabled[controller_index] != 1) {
    iVar1 = 0;
    if (0 < g_menu_records[param_1 + 1].entry_count) {
      do {
        if (g_menu_records[param_1 + 1].entry_enabled[controller_index] == 1) {
          g_menu_records[param_1 + 1].selected = controller_index;
          return;
        }
        controller_index = controller_index + 1;
        if (controller_index == g_menu_records[param_1 + 1].entry_count) {
          controller_index = 0;
        }
        iVar1 = iVar1 + 1;
      } while (iVar1 < g_menu_records[param_1 + 1].entry_count);
    }
    g_menu_records[param_1 + 1].selected = -1;
  }
  return;
}


