#include "mfl_tier2.h"
/*004014d0*/ extern int cup_scene_destroy_lights();
/*00401ee0*/ extern int cup_scene_destroy_clumps();
/*00403f40*/ extern int movie_texture_release();
/*0040d270*/ extern int effects_init();
/*0040e6a0*/ extern int vehicles_any_active();
/*0040e6f0*/ extern int course_unload_current();
/*00413a10*/ extern int replay_buffer_free();
/*00413a30*/ extern int replay_free_buffers();
/*0041b9c0*/ extern int hudpanel_clumps_destroy();
/*0041d450*/ extern int btb_panel_destroy();
/*0041e6e0*/ extern int score_panel_destroy();
/*0041f160*/ extern int team_panel_destroy();
/*0041f340*/ extern int hudpanel_clump_destroy();
/*0041fd70*/ extern int kc_panel_destroy();
/*00420910*/ extern int endpoint_panel_destroy();
/*00420b00*/ extern int team_endpoint_panel_destroy();
/*00420f40*/ extern int stopclock_destroy();
/*004211a0*/ extern int vehicle_atomics_destroy_659b30();
/*00423110*/ extern int vehicles_prepare_all();
/*004246c0*/ extern int car_damage_smoke_destroy_004246c0();
/*0042a140*/ extern int course_texture_dictionary_destroy();
/*0042b0b0*/ extern int course_get_split_sector_8();
/*0042b110*/ extern int current_course_id();
/*0042bba0*/ extern int ui_2d_destroy();
/*0042ca50*/ extern int build_info_textures_destroy();
/*00431b00*/ extern int frontend_world_destroy();
/*004677c0*/ extern int weapons_effects_destroy_all();
/*00473a40*/ extern int sound_update();
/*004755b0*/ extern int aux_frame_destroy();
/*00475600*/ extern int aux_camera_destroy();
/*00475730*/ extern int get_viewport_camera();
/*0047fb90*/ extern int rt2d_text_destroy();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*0048cfe0*/ extern int raster_destroy_771038();
/*00495c90*/ extern int rw_free_with_inner_buffer();
/*0049b710*/ extern int fx_ptank_destroy();
/*004a4030*/ extern int __cdecl app_get_app_76e890_4030(undefined4 p0);
/*004a4060*/ extern undefined __cdecl movie_stop_b(undefined4 p0);
/*004a5b80*/ extern int d3d9_pipeline_destroy_default();
/*004a6960*/ extern int d3d9_pipelines_destroy();
/*004c4950*/ extern void FUN_004c4950(undefined4 p0);
/*0050b5e0*/ extern int nop_stub();
/*005687c0*/ extern int RtCharsetDestroy();
/*0064d888*/ extern undefined4 g_app_64d888;
/*0069d698*/ extern undefined4 g_ui_69d698;
/*0069d69c*/ extern undefined4 g_ui_69d69c;
/*007eeb64*/ extern undefined4 g_ai_7eeb64;
/*007eebb0*/ extern undefined4 g_app_7eebb0;
/* app_shutdown_resources */
/* "releases the movie texture, d3d pipelines, texture dictionaries" */
/*
 * Releases the global resources at app shutdown: movie playback and texture,
 * the RW texture dictionary, D3D9 pipelines, the loaded course, replay buffers,
 * HUD / panel objects, effects, UI, text, sound and the build info textures.
 * Takes no parameters and returns nothing. Call order is kept as-is (inferred).
 */
void app_shutdown_resources(void)
{
  int active;
  undefined4 viewport_camera;

  nop_stub();
  movie_texture_release();
  /* Stop movie playback if the app reports it is active (inferred) */
  active = app_get_app_76e890_4030(0);
  if (active != 0) {
    movie_stop_b(0);
  }
  /* Destroy the texture dictionary and clear the global (inferred) */
  if (g_app_64d888 != (RwTexDictionary *)0x0) {
    RwTexDictionaryDestroy_004d5ac0(g_app_64d888);
    g_app_64d888 = (RwTexDictionary *)0x0;
  }
  d3d9_pipelines_destroy();
  vehicles_any_active();
  /* If a course is loaded, unload it (inferred) */
  active = course_get_split_sector_8();
  if (active != 0) {
    current_course_id();
    course_unload_current();
  }
  /* Replay buffers, then the HUD and panel objects */
  replay_free_buffers();
  replay_buffer_free();
  btb_panel_destroy();
  stopclock_destroy();
  hudpanel_clump_destroy();
  team_endpoint_panel_destroy();
  vehicle_atomics_destroy_659b30();
  endpoint_panel_destroy();
  kc_panel_destroy();
  team_panel_destroy();
  score_panel_destroy();
  fx_ptank_destroy();
  weapons_effects_destroy_all();
  vehicles_prepare_all();
  hudpanel_clumps_destroy();
  car_damage_smoke_destroy_004246c0();
  cup_scene_destroy_lights();
  cup_scene_destroy_clumps();
  effects_init();
  rw_free_with_inner_buffer(g_app_7eebb0);
  nop_stub();
  RtCharsetDestroy(g_ai_7eeb64);
  viewport_camera = get_viewport_camera(0xffffffff);
  aux_frame_destroy(viewport_camera);
  aux_camera_destroy();
  FUN_004c4950(g_ui_69d698);
  FUN_004c4950(g_ui_69d69c);
  frontend_world_destroy();
  rt2d_text_destroy();
  ui_2d_destroy();
  raster_destroy_771038();
  course_texture_dictionary_destroy();
  thunk_FUN_004c6880();
  sound_update();
  d3d9_pipeline_destroy_default();
  build_info_textures_destroy();
  return;
}
