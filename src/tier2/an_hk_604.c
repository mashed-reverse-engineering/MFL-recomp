#include "mfl_tier2.h"
/*004016c0*/ extern int cup_scene_place_object();
/*004019a0*/ extern int cup_scene_update_object();
/*00401d90*/ extern int cup_scene_render();
/*004041f0*/ extern int movie_present_frame();
/*00404fc0*/ extern int crown_render();
/*00405e40*/ extern int stub_return_zero();
/*00408740*/ extern int fx_render_all_and_highlight();
/*0040d340*/ extern int effects_render();
/*0040fbe0*/ extern int race_get_setup_index();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe00*/ extern double race_get_time_seconds();
/*0040ff70*/ extern int race_get_last_active_player();
/*00414250*/ extern int ghost_render();
/*00416080*/ extern int trail_particles_flush();
/*004160c0*/ extern int trail_particles_render_setup();
/*0041b9a0*/ extern int marker_particles_update_b();
/*00421ca0*/ extern int vehicle_particles_render();
/*00423c90*/ extern undefined __cdecl vehicle_render(int p0, struct RwCamera *p1);
/*00424af0*/ extern int debris_cull_update_all_vehicles();
/*0042a0b0*/ extern int course_signs_render_visible();
/*0042aaf0*/ extern int course_hook10();
/*0042b0b0*/ extern int course_get_split_sector_8();
/*0042b7a0*/ extern int course_render_cb3_dispatch();
/*0042efb0*/ extern int race_status_messages_draw();
/*00451c80*/ extern int camera_frame_ui();
/*00467820*/ extern int weapons_effects_render_all();
/*00474850*/ extern int course_objects_render_all();
/*00475730*/ extern int get_viewport_camera();
/*00480e70*/ extern int gfx_render_state_probe();
/*00481f40*/ extern int player_markers_render();
/*00487a90*/ extern int effect_trails_render_all();
/*0048cfb0*/ extern undefined __cdecl race_render_viewport_forward_4a61a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*00492af0*/ extern int rwp_atomics_render();
/*0049b6b0*/ extern int fx_ptank_render_a();
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005ddc28*/ extern float k_f_0;
/*00652f68*/ extern undefined4 g_race_phase;
/*00652f78*/ extern undefined4 g_race_render_652f78;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*008d8b14*/ extern char *g_race_setup;
/* race_render_viewport */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RwRenderStateSet cull mode, renders the world, cars, effects and HUD of one viewport" */

void race_render_viewport(void)

{
  int race_phase;
  undefined4 camera;
  float10 race_time;
  undefined4 forward_p1;
  undefined4 forward_p2;
  float scene_progress;
  
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLBACK);
  /* phase 7 with a last active player: the cup scene replaces the usual world pass (inferred) */
  race_phase = race_get_phase();
  if (race_phase == 7) {
    race_phase = race_get_last_active_player();
    if (race_phase != -1) {
      camera = race_get_last_active_player();
      /* scene_progress = clamp(race_time * 0.5, 0, 1): the cup scene takes two seconds to run (inferred) */
      race_time = (float10)race_get_time_seconds();
      if ((float10)float_1_0 <= race_time * (float10)k_f_0_5) {
LAB_00412e65:
        race_time = (float10)race_get_time_seconds();
        if (race_time * (float10)k_f_0_5 < (float10)float_1_0) {
          race_time = (float10)race_get_time_seconds();
          scene_progress = (float)(race_time * (float10)k_f_0_5);
        }
        else {
          scene_progress = 1.0;
        }
      }
      else {
        race_time = (float10)race_get_time_seconds();
        if ((float10)k_f_0 <= race_time * (float10)k_f_0_5) goto LAB_00412e65;
        scene_progress = 0.0;
      }
      cup_scene_update_object(*(int *)&scene_progress,camera);
      if (float_1_0 <= scene_progress) {
        cup_scene_place_object(*(int *)&scene_progress,g_game_state.mirror_mode,camera);
        cup_scene_render(*(int *)&scene_progress,camera);
        if (g_game_state.mirror_mode != 0) {
          movie_present_frame();
        }
        race_status_messages_draw();
        return;
      }
    }
  }
  /* main world pass: track, rwp atomics, player markers, course objects (inferred) */
  race_phase = race_get_setup_index();
  course_render_cb3_dispatch(**(undefined4 **)(g_race_setup + 4 + race_phase * 4));
  rwp_atomics_render();
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
  player_markers_render();
  course_objects_render_all();
  nop_stub(RwCameraGetCurrentCamera());
  /* draw the four vehicles unless the g_race_render_652f78 flag is set */
  if (g_race_render_652f78 == 0) {
    race_phase = 0;
    do {
      vehicle_render(race_phase,RwCameraGetCurrentCamera());
      race_phase = race_phase + 1;
    } while (race_phase < 4);
  }
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  /* ghost car only in phase 6 */
  race_phase = race_get_phase();
  if (race_phase == 6) {
    ghost_render();
  }
  vehicle_particles_render(RwCameraGetCurrentCamera());
  race_phase = 0;
  do {
    nop_stub(race_phase,RwCameraGetCurrentCamera());
    race_phase = race_phase + 1;
  } while (race_phase < 4);
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  /* particles, effects and trails, drawn with z-test and z-write on */
  debris_cull_update_all_vehicles();
  effects_render();
  stub_return_zero(0,0x3ff00000);
  fx_ptank_render_a();
  stub_return_zero(0,0);
  effect_trails_render_all();
  marker_particles_update_b();
  fx_render_all_and_highlight();
  weapons_effects_render_all();
  camera = get_viewport_camera(0);
  trail_particles_flush(camera);
  trail_particles_render_setup();
  crown_render();
  RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x1);
  gfx_render_state_probe();
  RwRenderStateSet(rwRENDERSTATEFOGENABLE,(void *)0x0);
  race_phase = race_get_setup_index();
  course_hook10(**(undefined4 **)(g_race_setup + 4 + race_phase * 4));
  if (game_mode != 5) {
    course_signs_render_visible();
  }
  /* forward pass for the viewport camera, when option_flags[4] is set outside phase 7 (inferred) */
  race_phase = race_get_phase();
  if ((race_phase != 7) && (g_game_state.option_flags[4] != 0)) {
    forward_p2 = 0;
    forward_p1 = 0xc0;
    camera = get_viewport_camera(0);
    race_render_viewport_forward_4a61a0(camera,forward_p1,forward_p2);
  }
  RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)rwCULLMODECULLNONE);
  RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
  RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
  /* HUD, then mirror mode is forced on when option_flags[1] is set */
  camera_frame_ui();
  if (g_game_state.option_flags[1] != 0) {
    g_game_state.mirror_mode = 1;
  }
  race_phase = race_get_phase();
  if (race_phase != 7) goto LAB_0041318c;
  race_phase = race_get_last_active_player();
  if (race_phase == -1) goto LAB_0041318c;
  camera = race_get_last_active_player();
  race_time = (float10)race_get_time_seconds();
  if ((float10)float_1_0 <= race_time * (float10)k_f_0_5) {
LAB_00413135:
    race_time = (float10)race_get_time_seconds();
    if (race_time * (float10)k_f_0_5 < (float10)float_1_0) {
      race_time = (float10)race_get_time_seconds();
      scene_progress = (float)(race_time * (float10)k_f_0_5);
    }
    else {
      scene_progress = 1.0;
    }
  }
  else {
    race_time = (float10)race_get_time_seconds();
    if ((float10)k_f_0 <= race_time * (float10)k_f_0_5) goto LAB_00413135;
    scene_progress = 0.0;
  }
  cup_scene_place_object(*(int *)&scene_progress,g_game_state.mirror_mode,camera);
  cup_scene_render(*(int *)&scene_progress,camera);
LAB_0041318c:
  /* present the mirror frame unless the race is in phase 9 or 10 */
  if (((g_race_phase != 9) && (g_race_phase != 10)) && (g_game_state.mirror_mode != 0)) {
    movie_present_frame();
  }
  race_phase = course_get_split_sector_8();
  if (race_phase != 0) {
    race_status_messages_draw();
  }
  nop_stub();
  return;
}


