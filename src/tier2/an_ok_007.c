#include "mfl_tier2.h"
/*00408e60*/ extern int player_record_get_value();
/*00409f30*/ extern double player_progress_b();
/*0040c820*/ extern int player_add_points();
/*0040d0b0*/ extern undefined __cdecl atomic_set_envmap(int p0, void *p1);
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040ea10*/ extern undefined __cdecl atomic_instance_filter(struct RpAtomic *p0);
/*004138a0*/ extern int time_split();
/*004146c0*/ extern int trail_clumps_call_update();
/*00414f00*/ extern int trail_update_00414f00();
/*00416230*/ extern int trail_particles_destroy();
/*004165a0*/ extern int trail_reset();
/*004165d0*/ extern int trail_particles_render();
/*0041da00*/ extern int btb_panel_destroy_extra();
/*00421d50*/ extern int vehicle_prepare_parts();
/*00422380*/ extern int __cdecl vehicle_get_frame_16c(undefined4 p0);
/*00422400*/ extern int __cdecl vehicle_get_active_flag(undefined4 p0);
/*004231b0*/ extern int vehicle_model_path();
/*00423320*/ extern int vehicle_load();
/*004241b0*/ extern int car_damage_smoke_reset();
/*00424690*/ extern int car_damage_smoke_render();
/*004254c0*/ extern int debris_any_active_all_vehicles();
/*00426b40*/ extern int car_kill();
/*00429620*/ extern int race_setup_struct_1();
/*00429630*/ extern int race_setup_struct_0();
/*00429640*/ extern int race_setup_struct_2();
/*00429650*/ extern int race_setup_struct_3();
/*00429660*/ extern int race_setup_struct_4();
/*00429670*/ extern int race_setup_struct_5();
/*0042a3f0*/ extern int course_get_rpworld();
/*0042ab20*/ extern undefined __cdecl course_camera_add_to_collision_world(struct RwCamera *p0);
/*0042fa80*/ extern int course_content_path_build();
/*00434bb0*/ extern int get_event_type();
/*00463270*/ extern int weapon_particles_render();
/*00475730*/ extern int get_viewport_camera();
/*00475860*/ extern int aux_camera_get_matrix();
/*0047a830*/ extern int car_is_active();
/*0047ac30*/ extern int car_get_mode();
/*0047b520*/ extern int car_current_matrix();
/*00481fd0*/ extern double random_range();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00485610*/ extern int particle_pool_counts_reset();
/*00485900*/ extern int particles_render_all_no_zwrite();
/*00485960*/ extern int particles_destroy_all();
/*00485a20*/ extern int particles_render_all_systems();
/*00485a90*/ extern int particle_systems_destroy_all();
/*00486160*/ extern int particle_templates_init_b();
/*00487620*/ extern int sprite_slots_clear();
/*004877e0*/ extern int effect_sprites_render_all();
/*004878f0*/ extern int sprite_slots_destroy();
/*00494520*/ extern undefined __cdecl replay_buffer_record_frame(int p0, RwMatrix *p1, undefined4 p2, int p3);
/*00498250*/ extern int fx_tables_clear();
/*00498330*/ extern int fx_render();
/*00498370*/ extern int fx_atomics_destroy();
/*00498960*/ extern int fx_systems_destroy();
/*00498d70*/ extern int fx_systems_render();
/*00498dc0*/ extern int effects_reset_four_systems();
/*00498e20*/ extern int fx_particles_destroy_two();
/*00499a40*/ extern int fx_particles_render_two();
/*00499a80*/ extern int fx_ring_clear();
/*00499c80*/ extern int fx_dust_destroy();
/*0049a840*/ extern int fx_dust_reset_count();
/*0049a930*/ extern int fx_dust_render_0049a930();
/*0049cb00*/ extern int fx_objects_reset_all();
/*0049d350*/ extern int fx_systems_destroy_all();
/*0049ded0*/ extern int fx_systems_render_all();
/*0049df50*/ extern int fx_slots_clear_8();
/*0049dff0*/ extern int fx_texture_destroy();
/*0049f6d0*/ extern int fx_styles_init_all();
/*0049feb0*/ extern int fx_clumps_destroy();
/*004a0370*/ extern int clumps_render_all_886024();
/*004a03b0*/ extern int fx_clump_slots_clear();
/*004a43d0*/ extern undefined __cdecl game_open_piz(char *p0);
/*004a4440*/ extern int game_close_piz();
/*004a8420*/ extern int debug_printf();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004b2ccd*/ extern undefined __cdecl start(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4);
/*004b322b*/ extern int exit();
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*0050b5e0*/ extern int nop_stub();
/*005dd330*/ extern float float_1_0;
/*005dd370*/ extern float k_f_0_3;
/*005dd62c*/ extern float k_f_100;
/*005dd7f4*/ extern float k_f_80;
/*005dda1c*/ extern float k_f_3_3333333en4;
/*005ddc28*/ extern float k_f_0;
/*005ddea0*/ extern float k_f_20;
/*005ddea8*/ extern float k_f_320;
/*005ddeac*/ extern float k_f_n320;
/*005ddeb0*/ extern float k_f_700;
/*006037c0*/ extern undefined4 g_phase_timer;
/*0064d890*/ extern undefined4 g_load_64d890;
/*00652dc0*/ extern undefined4 g_effects_reset_652dc0;
/*00652dc4*/ extern undefined4 g_effects_reset_652dc4;
/*00652dc8*/ extern undefined4 g_effects_reset_652dc8;
/*00652dcc*/ extern undefined4 g_car_652dcc;
/*00652dd0*/ extern undefined4 g_effects_652dd0;
/*00652dd4*/ extern undefined4 g_effects_652dd4;
/*00652dd8*/ extern undefined4 g_load_652dd8;
/*00652ddc*/ extern undefined4 g_effects_652ddc;
/*00652de0*/ extern undefined4 g_effects_652de0;
/*00652de4*/ extern undefined4 g_effects_652de4;
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
/*00652f88*/ extern undefined4 g_replay_652f88;
/*00652f8c*/ extern undefined4 g_replay_652f8c;
/*00652f98*/ extern undefined4 g_replay_652f98;
/*007ef758*/ extern float g_race_setup_7ef758;
/*007ef7d4*/ extern undefined4 g_race_time_ticks;
/*007ef7d8*/ extern undefined4 g_camera_7ef7d8;
/*007ef7dc*/ extern undefined4 g_race_clock_a;
/*007ef7f4*/ extern float g_tick_seconds;
/*008c8860*/ extern undefined4 g_course_8c8860;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88b0*/ extern undefined4 g_course_8c88b0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8900*/ extern undefined4 g_course_active_8c8900;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8950*/ extern undefined4 g_course_active_8c8950;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89a0*/ extern undefined4 g_course_active_8c89a0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c89f0*/ extern undefined4 g_course_active_8c89f0;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a40*/ extern undefined4 g_course_active_8c8a40;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8a90*/ extern undefined4 g_course_active_8c8a90;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;
/*008d8b14*/ extern undefined4 g_race_setup;
/*008d8b30*/ extern undefined4 player_count;
/* effects_init */

/* "initialises the particle / effect subsystems (many particle systems)" */

extern void effects_init(void);


/* effects_reset */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "resets the particle systems between races" */

extern void effects_reset(void);


/* effects_render */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "renders the particle systems when 0x652DD4 is set" */

extern void effects_render(void);


/* color_code_to_index */

/* "maps 0xFFA08080 -> 7, 0xFF1A3380 -> ... (the car colour constants) to a small index" */

extern undefined4 color_code_to_index(char *param_1);


/* course_slots_active_count */

/* "counts the four course slot pairs where both g_game_* and g_course_* are set" */

extern int course_slots_active_count(void);


/* load_shine_texture */

/*  */

extern void load_shine_texture(RpClump *clump,char *param_2,char *param_3);


/* race_setup_use_0 */

/* "g_race_setup = race_setup_struct_0()" */

extern void race_setup_use_0(void);


/* race_setup_use_1 */

/*  */

extern void race_setup_use_1(void);


/* race_setup_use_2 */

/*  */

extern void race_setup_use_2(void);


/* race_setup_use_3 */

/*  */

extern void race_setup_use_3(void);


/* race_setup_use_4 */

/*  */

extern void race_setup_use_4(void);


/* race_setup_use_5 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "also advances the float at 0x7EF758 by 1.0" */

extern void race_setup_use_5(void);


/* vehicles_any_active */

/* "true when any vehicle visual record is active" */

extern void vehicles_any_active(void);


/* vehicle_prepare_parts_0 */

/* "vehicle_prepare_parts(0)" */

extern void vehicle_prepare_parts_0(void);


/* vehicle_load_from_piz */

/* "(model): opens the vehicle piz of vehicle_model_path and loads the vehicle (FUN_00423320)" */

extern void vehicle_load_from_piz(char *param_1);


/* camera_world_attach */

/* "adds the main camera to the current world" */

extern void camera_world_attach(void);


/* race_get_race_setup */

/* "return g_race_setup;" */

extern undefined4 race_get_race_setup(void);


/* player_progress_difference */

/* "difference of two player_progress_b values with wrap around at 100 (inferred)" */

extern void __cdecl player_progress_difference(char *param_1,char *param_2,char *param_3,char *param_4);


/* race_get_intro_652df0 */

/* "return g_intro_652df0;" */

extern undefined4 race_get_intro_652df0(void);


/* intro_banner_get_x */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "float at 0x652DF4" */

extern double intro_banner_get_x(void);


/* intro_banner_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "state 1: slides the banner at 700 per second down to 320, then state 2 with a 5 s timer" */

extern void intro_banner_update(void);


/* camera_save */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(camera): copies the camera frame matrix into 0x652E08 and sets the 0.6 / 0.45 view window" */

extern void camera_save(RwCamera *camera);


/* race_set_setup_index */

/* "0x652F58 = index of the current race in the race setup" */

extern void race_set_setup_index(char *param_1);


/* race_get_setup_index */

/* "returns 0x652F58" */

extern undefined4 race_get_setup_index(void);


/* race_get_focus_player */

/* "(out player, out ?): active car with mode 0 selected by position, used for camera / AI focus
   (inferred)" */

extern void race_get_focus_player(int *param_1,int *param_2);


/* get_player_count */

/* "player_count" */

extern undefined4 get_player_count(void);


/* race_get_phase */

/* returns g_race_phase */

extern undefined4 race_get_phase(void);


/* race_get_time_seconds */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "g_race_time_ticks / 3000 minus the phase start time" */

extern double race_get_time_seconds(void);


/* team_color_rgba */

/* "(team, out): switch giving the RGBA of a team colour" */

extern void team_color_rgba(char *param_1,undefined1 *param_2);


/* race_get_race_652dec */

/* "return g_race_652dec;" */

extern undefined4 race_get_race_652dec(void);


/* race_set_view_player */

/* "0x652DEC = player index shown by the camera" */

extern void race_set_view_player(char *param_1);


/* race_get_phase_timer */

/* "return g_phase_timer;" */

extern undefined4 race_get_phase_timer(void);


/* race_course_loaded_event_get */

/* "returns g_race_course_loaded_event" */

extern undefined4 race_course_loaded_event_get(void);


/* race_course_loaded_event_clear */

/* "clears g_race_course_loaded_event" */

extern void race_course_loaded_event_clear(void);


/* race_time_trial_record */

/* "'Time trial recording error time %d'" */

extern void race_time_trial_record(char *param_1);



