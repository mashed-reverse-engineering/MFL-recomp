#include "mfl_tier2.h"
/*0040d0b0*/ extern undefined __cdecl atomic_set_envmap(int clump, void *tex);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *name);
/*00475860*/ extern int aux_camera_get_matrix();
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *clump, RpAtomicCallBack atomic, void *data);
/*0064d890*/ extern undefined4 g_load_64d890;
/*00406270*/ extern int scene_object_select_clear();
/*00408e60*/ extern int player_record_get_value();
/*00409a70*/ extern undefined __cdecl player_time_update(undefined4 car, undefined4 collision_world, undefined4 player_index);
/*00409f30*/ extern double player_progress_b();
/*0040c820*/ extern int player_add_points();
/*0040d0b0*/ extern undefined __cdecl atomic_set_envmap(int clump, void *tex);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *name);
/*0040f3f0*/ extern int car_boost_flame_effects_update();
/*004138a0*/ extern int time_split();
/*004154b0*/ extern undefined __cdecl hud_player_marker_position_calc(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10);
/*0041d9e0*/ extern int btb_panel_render_all();
/*0041da00*/ extern int btb_panel_destroy_extra();
/*00421d50*/ extern int vehicle_prepare_parts();
/*00421f30*/ extern int vehicle_set_flag_40();
/*00422400*/ extern int __cdecl vehicle_get_active_flag(undefined4 v);
/*004231b0*/ extern int vehicle_model_path();
/*00423320*/ extern int vehicle_load();
/*004237d0*/ extern int vehicle_setup_references();
/*004254c0*/ extern int debris_any_active_all_vehicles();
/*00426b40*/ extern int car_kill();
/*00429620*/ extern int race_setup_struct_1();
/*00429630*/ extern int race_setup_struct_0();
/*00429640*/ extern int race_setup_struct_2();
/*00429650*/ extern int race_setup_struct_3();
/*00429660*/ extern int race_setup_struct_4();
/*00429670*/ extern int race_setup_struct_5();
/*0042a410*/ extern int course_get_ai_world();
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *camera);
/*0042b030*/ extern undefined __cdecl course_unload(undefined4 course_id);
/*0042b110*/ extern int current_course_id();
/*0042b450*/ extern undefined __cdecl course_open_and_select(undefined4 p0);
/*0042fa80*/ extern int course_content_path_build();
/*00434bb0*/ extern int get_event_type();
/*00467d90*/ extern int weapons_unequip_all_a();
/*00467df0*/ extern int weapons_unequip_all_b();
/*00475860*/ extern int aux_camera_get_matrix();
/*0047a7f0*/ extern int __cdecl car_get_controls_disabled(undefined4 car);
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*00480af0*/ extern int physics_scenarios_reset();
/*00481f70*/ extern int car_wheel_contacts_clear_all();
/*00482070*/ extern int dot_product_3d_clamped();
/*0048a610*/ extern int spring_grid_init();
/*00494520*/ extern undefined __cdecl replay_buffer_record_frame(int buffer, RwMatrix *matrix, undefined4 value, int sample_index);
/*004955c0*/ extern int ghost_draw_markers();
/*004967f0*/ extern int triggers_assign_ids();
/*0049fee0*/ extern int materials_clear_textures_88602c();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *path);
/*004a4440*/ extern int game_close_piz();
/*004a8420*/ extern int debug_printf();
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *str);
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004b322b*/ extern int exit();
/*004b4ba7*/ extern int entry();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *frame, RwMatrix *matrix, enum RwOpCombineType combine_op);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *camera, enum RwCameraProjection projection);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *camera, struct RwV2d *view_window);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *vec);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *clump, RpAtomicCallBack atomic, void *data);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *clump);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/*005e64a4*/ extern float k_f_0;
/*005ddea0*/ extern float k_f_20;
/*005ddea8*/ extern float k_f_320;
/*005ddeac*/ extern float k_f_n320;
/*005ddeb0*/ extern float k_f_700;
/*006037c0*/ extern undefined4 g_phase_timer;
/*0064d890*/ extern undefined4 g_load_64d890;
/*00652de8*/ extern float g_race_652de8;
/*00652dec*/ extern undefined4 g_race_652dec;
/*00652df0*/ extern undefined4 g_intro_652df0;
/*00652df4*/ extern float g_intro_652df4;
/*00652dfc*/ extern float g_intro_652dfc;
/*00652e04*/ extern undefined4 g_race_course_loaded_event;
/*00652e08*/ extern undefined4 g_camera_652e08;
/*00652e88*/ extern undefined4 g_camera_652e88;
/*00652e8c*/ extern undefined4 g_camera_652e8c;
/*00652e90*/ extern undefined4 g_camera_save_652e90;
/*00652f58*/ extern undefined4 g_race_652f58;
/*00652f68*/ extern undefined4 g_race_phase;
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652f98*/ extern undefined4 g_replay_652f98;
/*00652fa0*/ extern undefined4 g_ghost_render_652fa0;
/*00652fb0*/ extern undefined g_trail_652fb0;
/*00652fb4*/ extern undefined g_trail_652fb4;
/*00652fb8*/ extern undefined g_trail_652fb8;
/*00653040*/ extern undefined4 g_trail_653040;
/*00653148*/ extern undefined4 g_trail_653148;
/*0065314c*/ extern undefined4 g_car_65314c;
/*0065315c*/ extern undefined4 g_trail_65315c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef758*/ extern float g_race_setup_7ef758;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*00801b14*/ extern undefined4 g_rw_801b14;
/*008ccb02*/ extern undefined2 g_trail_indices_8ccb02;
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b30*/ extern undefined4 player_count;
/* load_shine_texture */

/*  */

void load_shine_texture(RpClump *clump,undefined4 param_2,int texture_dict)

{
  undefined4 envmap_data[3];
  
  if (g_load_64d890 == 0) {
    envmap_data[2] = aux_camera_get_matrix(0);
    if (texture_dict == 0) {
      texture_dict = find_texture_dict_b("shine");
    }
    envmap_data[1] = texture_dict;
    envmap_data[0] = param_2;
    RpClumpForAllAtomics(clump,atomic_set_envmap,envmap_data);
  }
  return;
}
