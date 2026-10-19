#include "mfl_tier2.h"
/*0040fdb0*/ extern int get_player_count();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0041b810*/ extern int race_mark_ai_slots();
/*0041b9d0*/ extern int hudpanel_slot_init_player();
/*004222e0*/ extern int vehicle_get_matrix_ltm_b();
/*00423fe0*/ extern undefined damage_add_clamped(undefined4 p0, undefined4 p1, undefined4 p2, float p3);
/*00424190*/ extern undefined debris_slot_clear(undefined4 p0);
/*004246f0*/ extern int car_set_damage();
/*00424ab0*/ extern int rwp_world_current_deactivate();
/*00424db0*/ extern int debris_vehicle_destroy_all();
/*004266c0*/ extern int race_events_process_log();
/*00426bb0*/ extern int ai_editor_paint_input();
/*00426de0*/ extern int ai_editor_cursor_move();
/*00426e90*/ extern int ai_editor_brush_select();
/*004271e0*/ extern int ai_editor();
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00428a20*/ extern int cars_step_race_phase_and_respawn();
/*00428e30*/ extern undefined __cdecl player_stats_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00467890*/ extern int player_status_set();
/*0047a640*/ extern int car_deactivate();
/*0047a810*/ extern int car_set_controls_disabled();
/*0047f300*/ extern int cars_step();
/*00481fd0*/ extern int random_range();
/*004b3430*/ extern int __chkstk();
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005ddc28*/ extern float k_f_0;
/*00609008*/ extern undefined g_race_setup_609008;
/*006090d8*/ extern undefined g_race_setup_6090d8;
/*006091f8*/ extern undefined g_race_setup_6091f8;
/*006092c8*/ extern undefined g_race_setup_6092c8;
/*00609398*/ extern undefined g_race_setup_609398;
/*00609468*/ extern undefined g_race_setup_609468;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*006605b4*/ extern undefined g_vehicle_render_6605b4;
/*006614a0*/ extern undefined g_game_6614a0;
/*006614d8*/ extern undefined g_game_6614d8;
/*006614d9*/ extern undefined g_vehicle_render_6614d9;
/*006614da*/ extern undefined g_vehicle_render_6614da;
/*006614db*/ extern undefined g_vehicle_render_6614db;
/*006614e0*/ extern undefined g_vehicle_render_6614e0;
/*006614e4*/ extern undefined g_game_6614e4;
/*006614ec*/ extern undefined g_vehicle_render_6614ec;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f0258*/ extern undefined4 g_ai_editor_step_flag_a;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0260*/ extern undefined4 g_ai_editor_step_counter;
/*00801b0c*/ extern undefined4 g_car_801b0c;
/*008c88a0*/ extern undefined4 g_game_8c88a0;
/*008c88a4*/ extern undefined4 g_game_8c88a4;
/*008c88f0*/ extern undefined4 g_game_8c88f0;
/*008c88f4*/ extern undefined4 g_game_8c88f4;
/*008c8940*/ extern undefined4 g_vehicle_slots_8c8940;
/*008c8944*/ extern undefined4 g_game_8c8944;
/*008c8990*/ extern undefined4 g_vehicle_slots_8c8990;
/*008c8994*/ extern undefined4 g_game_8c8994;
/*008c89e0*/ extern undefined4 g_vehicle_slots_8c89e0;
/*008c89e4*/ extern undefined4 g_game_8c89e4;
/*008c8a30*/ extern undefined4 g_vehicle_slots_8c8a30;
/*008c8a34*/ extern undefined4 g_game_8c8a34;
/*008c8a80*/ extern undefined4 g_vehicle_slots_8c8a80;
/*008c8a84*/ extern undefined4 g_game_8c8a84;
/*008c8ad0*/ extern undefined4 g_vehicle_slots_8c8ad0;
/*008c8ad4*/ extern undefined4 g_game_8c8ad4;
/*008c8ae0*/ extern struct PlayerStatRecord g_player_stats_race[4];
/*008d8b30*/ extern undefined4 player_count;


extern void __cdecl car_damage_at_local_point (char *param_1,char *param_2,char *param_3,char *param_4,char *param_5, char *param_6,char *param_7,char *param_8,char *param_9, char *param_10,char *param_11);


extern void car_damage_at_world_point(char *param_1,char *param_2,RwV3d *pointsIn);


extern void debris_slot_clear_forward(char *param_1);


extern void __cdecl debris_slot_list_update_visible(char *param_1);


extern void debris_any_active_all_vehicles(void);


extern void __cdecl vehicle_tyre_marks_render(char *param_1,char *param_2);


extern void __cdecl player_stat_add_turbo_used(char *param_1,char *param_2);


extern undefined4 car_kill(char *param_1);


extern void ai_editor_update(void);


/* player_stat_get_place_team */

/* "result table place (team sum over controllers in use)" */

extern int player_stat_get_place_team(char *param_1);


/* game_get_game_8c8ae0 */

/* "return g_game_8c8ae0;" */

extern int game_get_game_8c8ae0(void);


/* player_stat_get_team_l */

/* "0x8C94A0" */

extern int player_stat_get_team_l(char *param_1);


/* player_stat_get_by_kind */

/* "(player, kind): switch over the statistic kinds" */

extern void __cdecl player_stat_get_by_kind(int *param_1,char *param_2,char *param_3);


/* player_stats_copy_to_totals */

/* "copies the per race statistic tables into the totals (0x8C90B4 / 0x8C9060 ...)" */

extern void player_stats_copy_to_totals(void);


/* player_stats_accumulate */

/* "adds the per race values to the running totals (0x8C9540 / 0x8C9594 ...)" */

extern void player_stats_accumulate(void);


/* race_update_cars_and_stats */

/* "ai_editor_update, race_mark_ai_slots, cars_step, race_events_process_log, player_stats_update"
    */

extern undefined4 __cdecl race_update_cars_and_stats(char *param_1);


/* race_setup_struct_1 */

/*  */

extern undefined * race_setup_struct_1(void);


/* race_setup_struct_0 */

/*  */

extern undefined * race_setup_struct_0(void);


/* race_setup_struct_2 */

/*  */

extern undefined * race_setup_struct_2(void);


/* race_setup_struct_3 */

/*  */

extern undefined * race_setup_struct_3(void);


/* race_setup_struct_4 */

/*  */

extern undefined * race_setup_struct_4(void);


/* race_setup_struct_5 */

/*  */

extern undefined * race_setup_struct_5(void);


/* best_times_init_defaults */

/* fills the 3 x 45 best time tables (9 min 59.xx s) */

extern void best_times_init_defaults(void);


/* vehicle_slots_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears 8 records of 0x50 bytes at 0x8C88A0" */

extern void vehicle_slots_clear(void);


/* vehicle_slots_clear_flags */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/*
 * Sets the eight vehicle slot / game flag globals (g_game_8c88a0 .. g_vehicle_slots_8c8ad0) to 0.
 * No parameters, no return value (purpose inferred from the name and the body).
 */

/* TIER2: validated 2026-10-18 authority=diffemu */
void vehicle_slots_clear_flags(void)
{
  g_game_8c88a0 = 0;
  g_game_8c88f0 = 0;
  g_vehicle_slots_8c8940 = 0;
  g_vehicle_slots_8c8990 = 0;
  g_vehicle_slots_8c89e0 = 0;
  g_vehicle_slots_8c8a30 = 0;
  g_vehicle_slots_8c8a80 = 0;
  g_vehicle_slots_8c8ad0 = 0;
  return;
}

extern undefined4 atomic_lookup_error(undefined4 atomic,int *lookup_state);


/* clump_check_atomics_lookup */

/* "RpClumpForAllAtomics(clump, atomic_lookup_error)" */

extern void clump_check_atomics_lookup(undefined4 *param_1);
#undef local_8
#undef local_4


