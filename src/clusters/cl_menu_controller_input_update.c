// cluster-entries: menu_controller_input_update
/* Installed cluster menu_controller_input_update (tools/cluster_install.py from output/cluster_pilot_uc3/menu_controller_input_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00435ec0*/ extern int event_type_uses_standard_menu(void);
/*0040ff20*/ extern int race_setup_get_slot_type(int slot);
/*004a70c0*/ extern int controller_present_004a70c0(int index);
/*004a5b30*/ extern int app_get_app_770fb4(void);
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
extern int g_message_state;
extern int g_input_mode;
extern int g_menu_update_69d2d4;
extern int g_menu_player0_team_choice;
extern float g_hud_69d66c;
extern float g_hud_69d670;
extern float g_menu_69d26c;
extern float g_frame_seconds;
extern int g_menu_7ef7d0;

/* Position of the colour chooser of one controller (color_id -1: controller absent). */
typedef struct PlayerColorSlot { int color_id; float x; float y; } PlayerColorSlot;
extern PlayerColorSlot player_color_id[12];

/* Menu update of the controller / player colour choice.  Input mode 0x1000 (setup of all 12 controllers): left / right edges of each controller change its colour
   index 0..6 (g_menu_update_69d2d4 = 1 when nobody changed anything).  Otherwise the player slots' controllers step the team choice 1..6 (the update
   flag is set at the ends).  Then, for the standard menus, the choice marker is placed (x 85 for team 0, else 70 pixels per team from 170), or
   for the others the colour choosers of the present controllers are laid out in a column (34 pixels apart) from the menu item position. */
static int menu_item_y_position_r(int p0, int p1);
extern void menu_item_y_position(void);
void menu_controller_input_update(void)
{
  int decreased = 0;
  int increased = 0;
  int i;
  int controller;
  float y;
  int present;
  PlayerColorSlot *slot;

  g_menu_update_69d2d4 = 0;
  if (g_message_state != 0) {
    return;
  }
  if (g_input_mode == 0x1000) {
    for (i = 0; i < 12; i++) {
      int *colour = &player_color_id[i].color_id;
      if (g_player_inputs[i].direction_flags[0] != 0 && g_player_inputs_previous[i].direction_flags[0] == 0 && *colour > 0) {
        *colour = *colour - 1;
        decreased = 1;
      }
      if (g_player_inputs[i].direction_flags[1] != 0 && g_player_inputs_previous[i].direction_flags[1] == 0 && *colour < 6) {
        *colour = *colour + 1;
        increased = 1;
      }
    }
    if (!decreased && !increased) {
      g_menu_update_69d2d4 = 1;
    }
  } else {
    for (i = 0; i < 4; i++) {
      controller = g_player_slots[i].controller;
      if (controller != -1) {
        if (race_setup_get_slot_type(i) != 2 && g_player_inputs[controller].direction_flags[0] != 0 && g_player_inputs_previous[controller].direction_flags[0] == 0) {
          if (g_menu_player0_team_choice < 2) {
            g_menu_update_69d2d4 = 1;
          } else {
            g_menu_player0_team_choice = g_menu_player0_team_choice - 1;
          }
        }
        if (g_player_inputs[controller].direction_flags[1] != 0 && g_player_inputs_previous[controller].direction_flags[1] == 0) {
          if (g_menu_player0_team_choice < 6) {
            g_menu_player0_team_choice = g_menu_player0_team_choice + 1;
          } else {
            g_menu_update_69d2d4 = 1;
          }
        }
      }
    }
  }
  if (event_type_uses_standard_menu() != 0) {
    if (g_menu_player0_team_choice == 0) {
      g_hud_69d66c = 85.0f;
      g_hud_69d670 = 220.0f;
    } else {
      g_hud_69d670 = 220.0f;
      g_hud_69d66c = (float)(g_menu_player0_team_choice * 0x46 - 0x46) + 170.0f;
    }
    return;
  }
  g_menu_69d26c = g_frame_seconds + g_menu_69d26c;
  y = (float)menu_item_y_position_r(app_get_app_770fb4(), 0x22);
  if (g_menu_7ef7d0 != 0) {
    present = 0;
    for (i = 0; i < 12; i++) {
      if (controller_present_004a70c0(i) != 0) {
        present++;
      }
    }
    y = (float)menu_item_y_position_r(present, 0x22);
  }
  for (i = 0; i < 12; i++) {
    slot = &player_color_id[i];
    if (controller_present_004a70c0(i) == 0) {
      slot->color_id = -1;
    } else {
      if (slot->color_id == -1) {
        slot->color_id = 0;
        slot->x = 85.0f;
        slot->y = y;
      } else if (slot->color_id < 0) {
        continue;
      } else if (slot->color_id == 0) {
        slot->x = 85.0f;
        slot->y = y;
      } else {
        slot->y = y;
        slot->x = (float)(slot->color_id * 0x46 - 0x46) + 170.0f;
      }
      y = y + 34.0f;
    }
  }
}

/* menu_item_y_position_r: C -> original menu_item_y_position (registers eax,ecx) */
__declspec(naked) int menu_item_y_position_r(int p0, int p1)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        mov ecx, dword ptr [esp + 8]
        call menu_item_y_position
        ret
    }
}
