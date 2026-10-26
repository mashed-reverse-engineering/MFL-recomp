// cluster-entries: race_update_common
/* Installed cluster race_update_common (tools/cluster_install.py from output/cluster_pilot_hb1/race_update_common.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00408140*/ extern int fx_update_effects();
/*0040e590*/ extern int effects_update_frame();
/*0040f550*/ extern int cars_update_per_frame_setup();
/*0040f6a0*/ extern int race_cup_scene_update_players_if_event();
/*0040fbe0*/ extern int race_get_setup_index();
/*00410de0*/ extern int race_update_cameras();
/*00411cc0*/ extern int race_update_by_event_type();
/*004155b0*/ extern int hud_player_markers_update();
/*0041b980*/ extern int marker_particles_update_a();
/*004209b0*/ extern int hudpanel_countdown_update();
/*004246d0*/ extern int car_damage_update_all();
/*00425720*/ extern int debris_update_all_vehicles_twice();
/*004295b0*/ extern undefined __cdecl race_update_cars_and_stats(undefined4 p0);
/*00429e80*/ extern int course_animated_objects_update();
/*0042af80*/ extern int course_update_frame(undefined4 course, int ticks);
/*0042e420*/ extern int race_time_trial_update(void);
/*00467a60*/ extern int weapons_update_frame();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047fda0*/ extern undefined __cdecl hud_labels_age_and_fade(undefined4 p0);
/*004879a0*/ extern int effect_trails_update_all();
/*00496bd0*/ extern int triggers_update_dispatch();
/*0049b330*/ extern int fx_ptank_a_lock_and_fill();
/*005ddb58*/ extern float k_f_1000;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005dda20*/ extern float k_f_4_2949673e9;
/*00652f68*/ extern undefined4 g_race_phase;
/*00652f74*/ extern undefined4 g_race_update_652f74;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7e0*/ extern undefined4 g_race_clock_b;
/*007ef7e8*/ extern undefined4 g_frame_ticks;
/*007ef7f0*/ extern undefined4 g_tick_count;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef7f8*/ extern float g_menu_wave_time;
/*007ef818*/ extern uint g_last_frame_ticks;
/*008d8b14*/ extern undefined4 g_race_setup;
/* race_update_common */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* per tick work shared by all race phases (ESI = ticks): sim time; weapons_update_frame; effects;
   clamps total time at 0xF731400 */

void race_update_common_n(uint in_esi)

{
  float fVar1;
  int iVar2;
  undefined4 camera_matrix;
  
  fVar1 = (float)in_esi;
  g_tick_seconds = (float)in_esi * k_f_3_3333333en4;
  g_tick_count = in_esi;
  if (g_race_update_652f74 == 0) {
    race_update_cars_and_stats(in_esi);
  }
  cars_update_per_frame_setup(in_esi);
  race_update_cameras(0);
  iVar2 = race_get_setup_index();
  course_update_frame(**(undefined4 **)(g_race_setup + 4 + iVar2 * 4),in_esi);
  camera_matrix = get_viewport_camera_matrix(0);
  hud_player_markers_update(camera_matrix);
  camera_matrix = get_viewport_camera_matrix(0);
  fx_ptank_a_lock_and_fill(camera_matrix);
  course_animated_objects_update(in_esi);
  if (game_mode == GAME_MODE_KILL_COPTER) {
    fx_update_effects();
  }
  weapons_update_frame();
  triggers_update_dispatch();
  effect_trails_update_all();
  marker_particles_update_a();
  car_damage_update_all();
  debris_update_all_vehicles_twice();
  race_time_trial_update();
  race_cup_scene_update_players_if_event();
  hud_labels_age_and_fade(0x19);
  hudpanel_countdown_update();
  race_update_by_event_type();
  if (0x3c < (int)g_frame_ticks) {
    g_menu_wave_time = fVar1 * k_f_3_3333333en4 + g_menu_wave_time;
    g_last_frame_ticks = (uint)(fVar1 * k_f_1000 + (float)g_last_frame_ticks);
  }
  g_race_time_ticks = g_race_time_ticks + in_esi;
  if (0xf731400 < (int)g_race_time_ticks) {
    g_race_time_ticks = 0xf731400;
  }
  if (g_race_phase != RACE_PHASE_STANDINGS) {
    g_race_clock_a = g_race_clock_a + in_esi;
    g_race_clock_b = g_race_clock_b + in_esi;
  }
  effects_update_frame();
  return;
}

/* entry thunk at the original address of race_update_common: register inputs (esi) -> natural C race_update_common_n; ret 0 */
__declspec(naked) void race_update_common(void)
{
    __asm {
        push esi
        call race_update_common_n
        add esp, 4
        ret
    }
}
