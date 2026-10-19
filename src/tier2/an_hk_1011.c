#include "mfl_tier2.h"
/*008014ac*/ extern char g_ai_data_routes_4644_[];
/*00800ea0*/ extern char g_ai_data_routes_3096_[];
/*00800894*/ extern char g_ai_data_routes_1548_[];
/*00800288*/ extern char g_ai_data_routes_0_[];
/*00409f60*/ extern int grid_position();
/*004270b0*/ extern int ai_save();
/*004271a0*/ extern int ai_clear();
/*0047b520*/ extern int car_current_matrix();
/*0047eb50*/ extern undefined __cdecl car_spawn(uint p0, RwReal x, float y, RwReal z, RwReal p4, undefined4 p5);
/*005dd330*/ extern float float_1_0;
/*006642f8*/ extern undefined4 g_ai_editor_6642f8;
/*00664310*/ extern undefined4 g_ai_editor_664310;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f0240*/ extern float g_ai_editor_view_origin_x;
/*007f0244*/ extern float g_ai_editor_view_origin_z;
/*007f024c*/ extern undefined4 g_ai_editor_menu_cursor;
/*007f0250*/ extern undefined4 g_ai_editor_route_set;
/*007f0254*/ extern undefined4 g_ai_editor_route_index;
/*007f0258*/ extern undefined4 g_ai_editor_step_flag_a;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0280*/ extern undefined4 g_ai_7f0280;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/* ai_editor */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "the built-in AI editor: reset car, start / stop race, single step, route editing (uses
   grid_position and car_spawn)" */

void ai_editor(void)

{
  undefined4 *puVar1;
  int iVar2;
  int iVar3;
  struct {
    RwReal local_14;
    int local_10;
    RwReal local_c;
    float local_8;
    RwReal local_4;
  } frame;
  
  iVar2 = g_ai_editor_menu_cursor;
  switch(g_ai_editor_route_set) {
  default:
    iVar3 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_0_);
    break;
  case 1:
    iVar3 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_1548_);
    break;
  case 2:
    iVar3 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_3096_);
    break;
  case 3:
    iVar3 = g_ai_editor_route_index * 0x204 + ((int)&g_ai_data_routes_4644_);
  }
  g_ai_editor_step_flag_a = 0;
  if ((g_player_inputs[0].menu_back == 0) || (g_player_inputs_previous[0].menu_back != 0)) {
LAB_004274f6:
    if (g_ai_editor_mode != 4) goto LAB_0042759f;
  }
  else {
    if (g_ai_editor_mode == 0) {
      if (g_ai_editor_menu_cursor == 0) {
        g_ai_editor_mode = 6;
        g_ai_editor_menu_cursor = 2;
        g_ai_editor_route_set = 0;
        return;
      }
      if (g_ai_editor_menu_cursor == 1) {
        g_ai_editor_mode = 6;
        g_ai_editor_menu_cursor = 2;
        g_ai_editor_route_set = iVar2;
        return;
      }
      if (g_ai_editor_menu_cursor == 2) {
        g_ai_editor_mode = 6;
        g_ai_editor_menu_cursor = 2;
        g_ai_editor_route_set = 2;
        return;
      }
      if (g_ai_editor_menu_cursor != 3) {
        if (g_ai_editor_menu_cursor == 8) {
          g_ai_editor_mode = 4;
          g_ai_editor_menu_cursor = 2;
          g_ai_editor_step_flag_b = 0;
          return;
        }
        if (g_ai_editor_menu_cursor != 7) {
          if (g_ai_editor_menu_cursor != 5) {
            g_ai_editor_step_flag_a = 0;
            return;
          }
          g_ai_editor_mode = 3;
          g_ai_editor_menu_cursor = 2;
          return;
        }
        g_ai_editor_mode = 1;
        g_ai_editor_menu_cursor = 2;
        return;
      }
      g_ai_editor_mode = 6;
      g_ai_editor_menu_cursor = 2;
      g_ai_editor_route_set = iVar2;
      return;
    }
    if (g_ai_editor_mode == 1) {
      if (g_ai_editor_menu_cursor == 2) {
        g_ai_editor_mode = 0;
        g_ai_editor_menu_cursor = 0;
        g_ai_editor_step_flag_a = 0;
        return;
      }
      ai_clear();
      return;
    }
    if (g_ai_editor_mode == 3) {
      if (g_ai_editor_menu_cursor == 2) {
        g_ai_editor_mode = 0;
        g_ai_editor_menu_cursor = 0;
        g_ai_editor_step_flag_a = 0;
        return;
      }
      ai_save();
      g_ai_editor_mode = 0;
      g_ai_editor_menu_cursor = 0;
      return;
    }
    if (g_ai_editor_mode != 4) {
LAB_00427417:
      if (g_ai_editor_mode == 6) {
        if (g_ai_editor_menu_cursor == 2) {
          *(float *)(iVar3 + g_ai_7f0280 * 8) = g_ai_editor_view_origin_x + float_1_0;
          *(float *)(iVar3 + 4 + g_ai_7f0280 * 8) = g_ai_editor_view_origin_z - float_1_0;
          if (g_ai_7f0280 == *(int *)(iVar3 + 0x200)) {
            *(int *)(iVar3 + 0x200) = *(int *)(iVar3 + 0x200) + 1;
          }
        }
        if (g_ai_editor_menu_cursor == 3) {
          if (*(int *)(iVar3 + 0x200) == 0x40) goto LAB_004274f6;
          iVar2 = *(int *)(iVar3 + 0x200) + 1;
          if ((int)g_ai_7f0280 < iVar2) {
            puVar1 = (undefined4 *)(iVar3 + -4 + iVar2 * 8);
            do {
              puVar1[1] = puVar1[-1];
              puVar1[2] = *puVar1;
              iVar2 = iVar2 + -1;
              puVar1 = puVar1 + -2;
            } while ((int)g_ai_7f0280 < iVar2);
          }
          *(float *)(iVar3 + g_ai_7f0280 * 8) = g_ai_editor_view_origin_x + float_1_0;
          *(float *)(iVar3 + 4 + g_ai_7f0280 * 8) = g_ai_editor_view_origin_z - float_1_0;
          if (g_ai_7f0280 == *(int *)(iVar3 + 0x200)) {
            g_ai_7f0280 = g_ai_7f0280 + 1;
          }
          *(int *)(iVar3 + 0x200) = *(int *)(iVar3 + 0x200) + 1;
        }
        if (g_ai_editor_menu_cursor == 5) {
          g_ai_editor_mode = 0;
          g_ai_editor_menu_cursor = 0;
          return;
        }
      }
      goto LAB_004274f6;
    }
    if (g_ai_editor_menu_cursor != 4) {
      if (g_ai_editor_menu_cursor == 3) {
        grid_position(0,0,1,&frame.local_c,&frame.local_14);
        car_spawn(0,frame.local_c,frame.local_8,frame.local_4,frame.local_14,(undefined4)&g_car_801b0c);      /* the position vector is passed by value; its y and z are never written (stack residue) */
      }
      if (g_ai_editor_menu_cursor == 2) {
        g_ai_editor_mode = 0;
        g_ai_editor_menu_cursor = 0;
        return;
      }
      goto LAB_00427417;
    }
    g_ai_editor_step_flag_b = g_ai_editor_step_flag_b - 1 & 1;
  }
  car_current_matrix(&frame.local_10,0);
  iVar2 = g_ai_editor_6642f8;
  g_ai_editor_view_origin_x = *(float *)(frame.local_10 + 0x30);
  g_ai_editor_view_origin_z = *(float *)(frame.local_10 + 0x38);
  if (g_ai_editor_menu_cursor != 5) goto LAB_0042759f;
  if (g_player_inputs[0].menu_back == 0) {
    g_ai_editor_6642f8 = 0;
  }
  else {
    g_ai_editor_664310 = g_ai_editor_664310 + 1;
    if (g_ai_editor_6642f8 == 0) {
      if (g_ai_editor_664310 != 1) goto LAB_0042759f;
      g_ai_editor_6642f8 = 1;
      g_ai_editor_step_flag_a = 1;
    }
    else if (g_ai_editor_6642f8 == 1) {
      if (g_ai_editor_664310 != 0x1e) goto LAB_0042759f;
      g_ai_editor_6642f8 = 2;
      g_ai_editor_step_flag_a = iVar2;
    }
    else {
      if ((g_ai_editor_6642f8 != 2) || (g_ai_editor_664310 != 3)) goto LAB_0042759f;
      g_ai_editor_step_flag_a = 1;
    }
  }
  g_ai_editor_664310 = 0;
LAB_0042759f:
  if (g_ai_editor_mode == 6) {
    if (((*(byte *)((char *)&g_player_inputs[0] + 58)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 58)) == '\0')) {
      g_ai_7f0280 = g_ai_7f0280 + 1;
    }
    if (((*(byte *)((char *)&g_player_inputs[0] + 59)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 59)) == '\0')) {
      g_ai_7f0280 = g_ai_7f0280 + -1;
    }
    if ((int)g_ai_7f0280 < 0) {
      g_ai_7f0280 = *(int *)(iVar3 + 0x200);
    }
    if ((*(int *)(iVar3 + 0x200) < g_ai_7f0280) || (g_ai_7f0280 == 0x40)) {
      g_ai_7f0280 = 0;
    }
    if (((*(byte *)((char *)&g_player_inputs[0] + 61)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 61)) == '\0')) {
      g_ai_editor_route_index = g_ai_editor_route_index + 1;
    }
    if (((*(byte *)((char *)&g_player_inputs[0] + 60)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + 60)) == '\0')) {
      g_ai_editor_route_index = g_ai_editor_route_index + -1;
    }
    if ((int)g_ai_editor_route_index < 0) {
      g_ai_editor_route_index = 2;
      return;
    }
    if (2 < (int)g_ai_editor_route_index) {
      g_ai_editor_route_index = 0;
    }
  }
  return;
}


