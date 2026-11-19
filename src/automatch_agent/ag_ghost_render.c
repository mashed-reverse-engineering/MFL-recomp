#include "mfl_tier2.h"
/*0041d9e0*/ extern int btb_panel_render_all();
/*00434bb0*/ extern int get_event_type();
/*004955c0*/ extern int ghost_draw_markers();
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652fa0*/ extern undefined4 g_ghost_render_652fa0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *name);
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*0040fe20*/ extern int __cdecl race_setup_slot_used(undefined4 p0);
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0040ff70*/ extern int race_get_last_active_player();
/*004131d0*/ extern int game_mode_end_check();
/*004162f0*/ extern int trail_set_param();
/*004166d0*/ extern undefined __cdecl noise_interp_cos(undefined4 value_a, undefined4 value_b, undefined4 t);
/*00416700*/ extern double noise_sample_row();
/*00416e20*/ extern int point_project_onto_line_2d(undefined4 p0);
/*00417b70*/ extern int __cdecl ai_find_leader_a(undefined4 bot_index);
/*0041ba10*/ extern int hudpanel_slot_reset(undefined4 p0);
/*0041be10*/ extern int hudpanel_slots_clear();
/*0041be30*/ extern int hudpanel_values_init();
/*0041bf20*/ extern int marker_particles_destroy();
/*0041c0d0*/ extern int hudpanel_counters_clear(undefined4 p0);
/*0041c510*/ extern int hudpanel_markers_render_all();
/*0041d9e0*/ extern int btb_panel_render_all();
/*004220f0*/ extern int vehicle_get_position_158();
/*00422150*/ extern int vehicle_get_flags_10();
/*00422190*/ extern int vehicle_get_flag_400();
/*00422280*/ extern int vehicle_part_frame();
/*004222a0*/ extern int vehicle_part_matrix();
/*00427d20*/ extern int game_get_game_8c8ae0();
/*00434bb0*/ extern int get_event_type();
/*004353e0*/ extern int menu_markers_enabled_for_event();
/*00435f10*/ extern int get_career_row();
/*0044c140*/ extern int __cdecl player_distance_warning(undefined4 p0);
/*0044c180*/ extern double player_distance_get();
/*0044d1f0*/ extern undefined ai_grid_cell_lookup_0044d1f0(undefined4 p0);
/*0044d2a0*/ extern int ai_nearest_route_point();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047b480*/ extern double car_probe_ground_distance();
/*0047b520*/ extern int car_current_matrix();
/*0047b590*/ extern int car_get_forward();
/*00481fd0*/ extern double random_range();
/*00482070*/ extern int dot_product_3d_clamped();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *dictionary);
/*00486710*/ extern int particles_destroy();
/*00486b00*/ extern int ptank_set_blend_modes();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *bits_a, uint bits_b, uint bits_c, struct RwTexture *texture);
/*004955c0*/ extern int ghost_draw_markers();
/*004967d0*/ extern int triggers_get_array();
/*0049cb60*/ extern int sphere_vs_box_test(undefined4 p0, float *floats_a, float *floats_b);
/*004b2b48*/ extern int _ftol();
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *frame);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *out, struct RwV3d *in);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *vec);
/*004d54d0*/ extern RwReal RwV2dLength(struct RwV2d *v);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *atomic);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *clump);
/*00544e00*/ extern undefined __cdecl RpRandomSeed(undefined4 seed);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005dd36c*/ extern float k_f_0_4;
/*005dd370*/ extern float k_f_0_3;
/*005dd620*/ extern float float_10_0;
/*005dd628*/ extern float k_f_0_25;
/*005dd62c*/ extern float k_f_100;
/*005dd634*/ extern float k_f_2;
/*005dda24*/ extern float k_f_0_75;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb00*/ extern float k_f_0_025;
/*005ddb48*/ extern float k_f_16;
/*005ddb50*/ extern float k_f_8;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de1b0*/ extern float k_f_n0_05;
/*005de1b4*/ extern float k_f_0_0625;
/*005de1b8*/ extern float k_f_31;
/*005de218*/ extern float k_f_0_866;
/*005de230*/ extern float k_f_n0_75;
/*005de234*/ extern float k_f_0_875;
/*0060392c*/ extern float g_hud_player_60392c;
/*00603950*/ extern undefined g_ai_bots_603950;
/*00603954*/ extern undefined g_ai_bots_603954;
/*00603a84*/ extern undefined g_ai_bots_603a84;
/*006089b0*/ extern undefined4 g_ai_bots_6089b0;
/*00652f84*/ extern undefined4 g_ghost_652f84;
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652fa0*/ extern undefined4 g_ghost_render_652fa0;
/*00652fb0*/ extern undefined g_trail_652fb0;
/*00652fb4*/ extern undefined g_trail_652fb4;
/*00652fb8*/ extern undefined g_trail_652fb8;
/*0065314c*/ extern undefined4 g_car_65314c;
/*0065315c*/ extern undefined4 g_trail_65315c;
/*00653160*/ extern undefined4 g_hud_653160;
/*00653164*/ extern undefined4 g_hud_player_653164;
/*00653258*/ extern undefined g_trail_653258;
/*00653298*/ extern undefined4 g_noise_table_653298;
/*00658300*/ extern undefined4 g_load_658300;
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658490*/ extern undefined4 g_particles_658490;
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538;
/*00658b28*/ extern undefined4 g_load_658b28;
/*00658b2c*/ extern undefined4 g_load_658b2c;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7b0*/ extern undefined4 g_car_7ef7b0;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef7c0*/ extern undefined4 g_cars_7ef7c0;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef804*/ extern undefined4 g_tick_number;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f0284*/ extern struct AiData g_ai_data;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008bed20*/ extern struct WeaponSlot player_weapon_slots[4];
/*008c9980*/ extern float g_ai_skill;
/*008c9984*/ extern undefined4 g_ai_last_active_player;
/*008c9988*/ extern undefined4 g_ai_catchup_class;
/*008c998c*/ extern undefined4 g_car_8c998c;
/*008c9990*/ extern float g_car_8c9990;
/*008c9998*/ extern undefined4 g_ai_counter_9998;
/*008c999c*/ extern float g_ai_skill_bias;
/*008c99a0*/ extern float g_ai_skill_score;
/*008c99a4*/ extern undefined4 g_ai_career_skill_table;
/*008c9a40*/ extern undefined4 g_ai_career_counter_table;
/*008c9ae0*/ extern struct AiBotRecord g_ai_bots[4];
/*008c9ec0*/ extern undefined4 g_ai_player_counters;
/*008c9ee0*/ extern float g_noise_table_8c9ee0;
/*008c9ee4*/ extern float g_noise_table_8c9ee4;
/*008c9ee8*/ extern float g_noise_table_8c9ee8;
/*008c9eec*/ extern float g_noise_table_8c9eec;
/*008c9ef0*/ extern undefined4 g_noise_table_8c9ef0;
/*008c9f00*/ extern undefined g_hud_player_8c9f00;
/*008cb500*/ extern undefined g_hud_8cb500;
/*008ccb02*/ extern undefined2 g_trail_indices_8ccb02;
/*008d8b00*/ extern undefined4 g_trail_8d8b00;
/* ghost_render */

/* "event types 0..2: renders the ghost car with additive blending" */

void ghost_render(void)

{
  int event_type;
  
  event_type = get_event_type();
  if ((event_type >= 0) && (event_type <= 2)) {
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)0x5);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)0x6);
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATECULLMODE,(void *)0x2);
    if ((g_replay_652f88 != 0) || (g_ghost_652f84 != 0)) {
      btb_panel_render_all();
    }
    if ((g_ghost_render_652fa0 != 0) && (g_replay_652f88 != 0)) {
      ghost_draw_markers(g_replay_652f88);
    }
  }
  return;
}
