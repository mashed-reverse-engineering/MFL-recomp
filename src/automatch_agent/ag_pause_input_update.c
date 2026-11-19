#include "mfl_tier2.h"
/*00430b20*/ extern int frontend_stub_return_0_b();
/*00439040*/ extern int frontend_enter_race_menu();
/*0044c540*/ extern int camera_get_car_8c747c();
/*00451c50*/ extern int camera_records_warmup();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*004a2990*/ extern int app_state_set_1();
/*005dd634*/ extern float k_f_2;
/*0064d8b0*/ extern float g_attract_timer;
/*0064d8b4*/ extern float g_scene_64d8b4;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];


/* pause_input_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(dt): player 0 pause / menu select press edge (g_player_inputs vs previous) after 2 s opens the
   pause or quit menu; unequips all weapons" */

void __cdecl pause_input_update(float dt)

{
  int pause_requested;
  int camera_car;
  undefined4 menu_arg;
  
  g_attract_timer = g_attract_timer + dt;
  pause_requested = false;
  camera_car = camera_get_car_8c747c();
  if (camera_car == 0) {
    camera_records_warmup(1,0);
  }
  if ((((g_player_inputs[0].pause != 0) && (g_player_inputs_previous[0].pause == 0)) ||
      ((g_player_inputs[0].menu_select != 0 && (g_player_inputs_previous[0].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[1].pause != 0) && (g_player_inputs_previous[1].pause == 0)) ||
      ((g_player_inputs[1].menu_select != 0 && (g_player_inputs_previous[1].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[2].pause != 0) && (g_player_inputs_previous[2].pause == 0)) ||
      ((g_player_inputs[2].menu_select != 0 && (g_player_inputs_previous[2].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[3].pause != 0) && (g_player_inputs_previous[3].pause == 0)) ||
      ((g_player_inputs[3].menu_select != 0 && (g_player_inputs_previous[3].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[4].pause != 0) && (g_player_inputs_previous[4].pause == 0)) ||
      ((g_player_inputs[4].menu_select != 0 && (g_player_inputs_previous[4].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[5].pause != 0) && (g_player_inputs_previous[5].pause == 0)) ||
      ((g_player_inputs[5].menu_select != 0 && (g_player_inputs_previous[5].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[6].pause != 0) && (g_player_inputs_previous[6].pause == 0)) ||
      ((g_player_inputs[6].menu_select != 0 && (g_player_inputs_previous[6].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((((g_player_inputs[7].pause != 0) && (g_player_inputs_previous[7].pause == 0)) ||
      ((g_player_inputs[7].menu_select != 0 && (g_player_inputs_previous[7].menu_select == 0)))) &&
     (g_attract_timer > k_f_2)) {
    pause_requested = true;
  }
  if ((!(g_attract_timer > g_scene_64d8b4)) && (!pause_requested)) {
    return;
  }
  menu_arg = frontend_stub_return_0_b();
  frontend_enter_race_menu(menu_arg);
  app_state_set_1();
  camera_records_warmup(0,0);
  weapons_unequip_all_a();
  weapons_unequip_all_b();
  return;
}
