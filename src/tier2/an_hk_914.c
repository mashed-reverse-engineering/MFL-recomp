#include "mfl_tier2.h"
/*004062a0*/ extern int scene_object_select_is_active();
/*0040fdc0*/ extern int race_get_phase();
/*0044a0f0*/ extern int __cdecl camera_debug_adjust_scale(RwCamera *param_2);
/*00451850*/ extern undefined __cdecl camera_race_setup(struct RwCamera *p0, undefined4 p1, undefined4 p2);
/*00475730*/ extern int get_viewport_camera();
/*0047b520*/ extern int car_current_matrix();
/*005dd364*/ extern float k_f_n1;
/*00652f64*/ extern undefined4 g_race_652f64;
/*00652f68*/ extern undefined4 g_race_phase;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/* race_update_cameras */

/* sets up the cameras of the 4 players (split screen aware) */

void race_update_cameras(undefined4 param_1)

{
  int iVar1;
  RwCamera *camera;
  undefined4 *puVar2;
  struct {
    undefined4 local_14;
    undefined4 local_10 [4];
  } frame;
  
  if ((g_race_phase == 3) && (iVar1 = scene_object_select_is_active(), iVar1 != 0)) {
    return;
  }
  iVar1 = 0;
  do {
    car_current_matrix(&frame.local_14,iVar1);
    frame.local_10[iVar1] = frame.local_14;
    iVar1 = iVar1 + 1;
  } while (iVar1 < 4);
  if (((*(byte *)((char *)&g_player_inputs[1] + 29)) == '\0') || ((*(byte *)((char *)&g_player_inputs_previous[1] + 29)) != '\0')) {
    if (g_race_652f64 == 0) goto LAB_00410e4e;
  }
  else {
    if (g_race_652f64 != 0) {
      g_race_652f64 = 0;
LAB_00410e4e:
      puVar2 = frame.local_10;
      camera = (RwCamera *)get_viewport_camera(0);
      camera_race_setup((struct RwCamera *)camera,(undefined4)puVar2,param_1);
      goto LAB_00410e6b;
    }
    g_race_652f64 = 1;
  }
  camera = (RwCamera *)get_viewport_camera(0);
  camera_debug_adjust_scale(camera);
LAB_00410e6b:
  iVar1 = race_get_phase();
  if ((iVar1 != 7) && (g_game_state.option_flags[2] != 0)) {
    iVar1 = get_viewport_camera(0);
    iVar1 = *(int *)(iVar1 + 4);
    *(float *)(iVar1 + 0x10) = *(float *)(iVar1 + 0x10) * k_f_n1;
    *(float *)(iVar1 + 0x14) = *(float *)(iVar1 + 0x14) * k_f_n1;
    *(float *)(iVar1 + 0x18) = *(float *)(iVar1 + 0x18) * k_f_n1;
    *(float *)(iVar1 + 0x20) = *(float *)(iVar1 + 0x20) * k_f_n1;
    *(float *)(iVar1 + 0x24) = *(float *)(iVar1 + 0x24) * k_f_n1;
    *(float *)(iVar1 + 0x28) = *(float *)(iVar1 + 0x28) * k_f_n1;
  }
  return;
}


