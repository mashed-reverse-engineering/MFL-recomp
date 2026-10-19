#include "mfl_tier2.h"
/*00408c40*/ extern double fx_current_object_get_float();
/*0040fdc0*/ extern int race_get_phase();
/*0041b970*/ extern int hudpanel_reset_all();
/*0041b9c0*/ extern int hudpanel_clumps_destroy();
/*0041ba10*/ extern int hudpanel_slot_reset(undefined4 p0);
/*0041ba50*/ extern undefined hudpanel_marker_calc_pixel_ray(undefined4 p0);
/*0041bd90*/ extern int hudpanel_render_all_slots();
/*0041be10*/ extern int hudpanel_slots_clear();
/*0041c0d0*/ extern int hudpanel_counters_clear(undefined4 p0);
/*0041c460*/ extern undefined hudpanel_render_parts_if_active(undefined4 p0);
/*0041d480*/ extern int btb_panel_slot_init();
/*0041d580*/ extern undefined __cdecl btb_panel_slot_set_position(undefined4 p0, undefined4 p1);
/*0041d5d0*/ extern int btb_panel_slot_set_scale();
/*0041f350*/ extern undefined hudpanel_render_group(undefined4 p0);
/*0041fa20*/ extern int kc_panel_entry_render();
/*0041fb80*/ extern int kc_panel_entry_destroy();
/*00420b30*/ extern undefined stopclock_render_timed_parts(undefined4 p0);
/*00420cb0*/ extern undefined ai_counters_snapshot(undefined4 p0);
/*00420d20*/ extern undefined stopclock_set_digit_count(undefined4 p0);
/*00420f50*/ extern undefined stopclock_render_parts_by_team(undefined4 p0, int p1);
/*004212b0*/ extern undefined time_trial_panel_times_update(undefined4 p0);
/*004215a0*/ extern undefined hudpanel_set_icon_texture_per_part(undefined4 p0);
/*00421720*/ extern undefined vehicle_markers_render_parts(undefined4 p0);
/*0042f780*/ extern int __cdecl clump_load_by_name(char *p0);
/*00434bb0*/ extern int get_event_type();
/*004353e0*/ extern int menu_markers_enabled_for_event();
/*00475730*/ extern int get_viewport_camera();
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/*00486710*/ extern int particles_destroy();
/*00486ef0*/ extern int particles_render();
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1da0*/ extern struct RwCamera * RwCameraSetProjection(struct RwCamera *p0, enum RwCameraProjection p1);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004f6a60*/ extern RwBool RpAtomicDestroy(struct RpAtomic *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd40c*/ extern float k_f_1_5;
/*005dd628*/ extern float k_f_0_25;
/*005dd634*/ extern float k_f_2;
/*005ddacc*/ extern float k_f_360;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc28*/ extern float k_f_0;
/*005de1d0*/ extern float k_f_1_25;
/*005de288*/ extern float k_f_1_125;
/*005de2f8*/ extern float k_f_n90;
/*005de398*/ extern float k_f_0_35;
/*005de39c*/ extern float k_f_2_4;
/*00603f4c*/ extern undefined4 g_hudpanel_603f4c;
/*00603fc8*/ extern struct CourseEntry g_course_table[28];
/*006047a8*/ extern int g_course_table_count;
/*00658300*/ extern undefined4 g_load_658300;
/*00658450*/ extern undefined g_hudpanel_658450;
/*00658490*/ extern undefined4 g_particles_658490;
/*006584f8*/ extern undefined g_hudpanel_6584f8;
/*00658538*/ extern undefined g_hudpanel_658538;
/*006586e8*/ extern undefined g_load_6586e8;
/*00658a28*/ extern undefined4 g_particles_658a28;
/*00658b28*/ extern undefined4 g_load_658b28;
/*00658b2c*/ extern undefined4 g_load_658b2c;
/*00658b30*/ extern undefined g_load_658b30;
/*00658b40*/ extern undefined4 g_btb_658b40;
/*00658b48*/ extern undefined4 g_hudpanel_658b48;
/*00658b50*/ extern undefined g_btb_panel_658b50;
/*00658b54*/ extern undefined g_btb_panel_658b54;
/*00658b58*/ extern undefined g_btb_panel_658b58;
/*00658b5c*/ extern undefined g_btb_panel_658b5c;
/*00658b60*/ extern undefined g_btb_panel_658b60;
/*00658b64*/ extern undefined g_btb_panel_658b64;
/*00658b68*/ extern undefined g_btb_panel_658b68;
/*00658b70*/ extern undefined g_btb_panel_658b70;
/*00658b74*/ extern undefined g_btb_panel_658b74;
/*00658b78*/ extern undefined g_btb_panel_658b78;
/*00658b80*/ extern undefined g_btb_panel_658b80;
/*00658b84*/ extern undefined g_btb_panel_658b84;
/*00658b88*/ extern undefined g_btb_panel_658b88;
/*00658b90*/ extern undefined g_btb_panel_658b90;
/*00658ba0*/ extern undefined g_btb_panel_658ba0;
/*00658ba4*/ extern undefined g_btb_panel_658ba4;
/*00658ba8*/ extern undefined g_btb_panel_658ba8;
/*00658c10*/ extern undefined g_btb_panel_658c10;
/*00658cd8*/ extern undefined4 g_btb_658cd8;
/*00658ce0*/ extern undefined4 g_hudpanel_658ce0;
/*00658ce4*/ extern undefined4 g_time_trial_658ce4;
/*00658d00*/ extern undefined4 g_time_trial_658d00;
/*00658d2c*/ extern undefined4 g_load_ghost_658d2c;
/*00658d4c*/ extern undefined4 g_hudpanel_658d4c;
/*00658fd0*/ extern undefined4 g_score_panel_658fd0;
/*006592c4*/ extern undefined g_load_6592c4;
/*006592c8*/ extern undefined4 g_hudpanel_6592c8;
/*006592d4*/ extern undefined4 g_hudpanel_6592d4;
/*006592d8*/ extern undefined4 g_hudpanel_6592d8;
/*006592dc*/ extern undefined4 g_hudpanel_6592dc;
/*006592f0*/ extern undefined4 g_load_6592f0;
/*00659340*/ extern undefined g_kc_659340;
/*00659790*/ extern undefined4 g_kc_659790;
/*00659a78*/ extern undefined4 g_endpoint_659a78;
/*00659a7c*/ extern undefined4 g_load_659a7c;
/*00659a9c*/ extern undefined4 g_load_659a9c;
/*00659aa0*/ extern undefined4 g_hudpanel_659aa0;
/*00659aa4*/ extern undefined4 g_hudpanel_659aa4;
/*00659aa8*/ extern float g_hudpanel_659aa8;
/*00659ac0*/ extern undefined4 g_hudpanel_659ac0;
/*00659ac8*/ extern undefined4 g_race_update_659ac8;
/*00659b00*/ extern undefined4 g_stopclock_659b00;
/*00659b04*/ extern undefined4 g_ai_659b04;
/*00659b08*/ extern undefined4 g_stopclock_659b08;
/*00659b0c*/ extern float g_race_update_659b0c;
/*00659b30*/ extern undefined4 g_load_659b30;
/*00659cb8*/ extern undefined4 g_game_659cb8;
/*00659cbc*/ extern undefined4 g_hud_game_659cbc;
/*00659cc0*/ extern undefined4 g_hud_game_659cc0;
/*00659cd0*/ extern undefined4 g_hud_game_659cd0;
/*00659cd4*/ extern undefined4 g_hud_game_659cd4;
/*00659cd8*/ extern undefined4 g_hud_game_659cd8;
/*00659cdc*/ extern undefined4 g_game_659cdc;
/*00659ce0*/ extern undefined4 g_load_659ce0;
/*00659ce4*/ extern undefined4 g_hud_game_659ce4;
/*00659cec*/ extern undefined4 g_hud_game_659cec;
/*00659cf0*/ extern undefined4 g_hud_game_659cf0;
/*00659cf4*/ extern undefined4 g_hud_game_659cf4;
/*00659cf8*/ extern undefined4 g_game_659cf8;
/*00659cfc*/ extern undefined4 g_game_659cfc;
/*00659d00*/ extern undefined4 g_game_659d00;
/*00659d04*/ extern undefined4 g_current_course_entry;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7f4*/ extern float g_tick_seconds;
/*007ef804*/ extern undefined4 g_tick_number;
/* hudpanel_values_init */

/* "fills the 0x40 float values at 0x658A28 with 1.75" */

extern void hudpanel_values_init(void);


/* marker_particles_render */

/* "particles_render(0x6586E8)" */

extern void marker_particles_render(void);


/* marker_particles_destroy */

/* "particles_destroy(0x6586E8)" */

extern void marker_particles_destroy(void);


/* hudpanel_reset_all_b */

/* "second variant of hudpanel_reset_all" */

extern void hudpanel_reset_all_b(void);


/* hudpanel_markers_render_all */

/* "renders all marker slots with z write off plus the particle layer with the camera frame setup"
    */

extern void hudpanel_markers_render_all(void);

#define CLUMP_SLOT_STRIDE (0x15) /* in undefined4 units, 0x54 bytes per clump slot */
#define CLUMP_TABLE_END (((LONG_PTR)&g_hudpanel_658538)) /* first address after the clump slot table */
#define ATOMIC_SLOT_STRIDE (0x19) /* in undefined4 units, 0x64 bytes per atomic slot */
#define ATOMIC_TABLE_END (((LONG_PTR)&g_particles_658490)) /* first address after the atomic slot table */
#define TEAM_COLOUR_FLOATS (12) /* four team colours of three floats each */
#define PANEL_MATRIX_FLAGS (rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEMASK) /* identity + type mask bits (inferred) */
#define COUNTDOWN_REST_Y (0.35) /* countdown panel y while the slide has not started */

/* hudpanel_clumps_destroy_b */

/* "second variant of hudpanel_clumps_destroy" */

/*
 * Destroys the clumps and atomics this HUD panel owns (inferred from the callees):
 * the clump slots at 0x658490, the atomic slots at 0x658300 (each atomic's frame is
 * destroyed before the atomic), the load clump g_load_658b28, the texture dictionary
 * g_load_658b2c, the particle system and the marker particles.
 * Takes no parameters and returns nothing.
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void hudpanel_clumps_destroy_b(void)
{
  struct { undefined4 * clump_slot; int * atomic_slot; } cursors;
#define clump_slot cursors.clump_slot
#define atomic_slot cursors.atomic_slot

  /* Walk the clump slot table and destroy each slot's clump. */
  clump_slot = &g_particles_658490;
  do {
    RpClumpDestroy((RpClump *)*clump_slot);
    clump_slot = clump_slot + CLUMP_SLOT_STRIDE;
  } while ((LONG_PTR)clump_slot < CLUMP_TABLE_END);
  /* Walk the atomic slot table: destroy each atomic's frame (+4 is its parent), then the atomic. */
  atomic_slot = &g_load_658300;
  do {
    RwFrameDestroy(*(RwFrame **)(*atomic_slot + 4));
    RpAtomicDestroy((RpAtomic *)*atomic_slot);
    atomic_slot = atomic_slot + ATOMIC_SLOT_STRIDE;
  } while ((LONG_PTR)atomic_slot < ATOMIC_TABLE_END);
  RpClumpDestroy(g_load_658b28);
  RwTexDictionaryDestroy_004d5ac0(g_load_658b2c);
  particles_destroy(&g_hudpanel_6584f8);
  marker_particles_destroy();
  return;
}
#undef clump_slot
#undef atomic_slot


/* btb_panel_destroy */

/* "RpClumpDestroy(0x658B40) (BTBPanel.dff, inferred)" */

extern void btb_panel_destroy(void);


/* hudpanel_render_if_loaded_b */

/* "hudpanel_render_group when g_hudpanel_658b48 is set" */

extern void hudpanel_render_if_loaded_b(void);


/* btb_panel_slot_set_matrix */

/* "(slot, matrix*): copies 16 floats" */

extern void btb_panel_slot_set_matrix(char *param_1,undefined4 *param_2);


/* btb_panel_slot_render */

/* "calls the clump object's method +0x48 when visible" */

extern void __cdecl btb_panel_slot_render(char *param_1);


/* load_ghost_model */

/* ghost.dff */

extern void load_ghost_model(char *param_1,char *param_2);


/* time_trial_panel_step_part_a */

/* "per slot direction vector between car and panel point, normalised length" */

extern void __cdecl time_trial_panel_step_part_a(char *param_1,char *param_2);


/* time_trial_panel_wheel_spin */

/* "per slot spin angle of four parts from distance*direction, wrapped at 360" */

extern void time_trial_panel_wheel_spin(char *param_1);


/* score_panel_clear */

/* "g_score_658fd0 = 0" */

extern void score_panel_clear(void);


/* hudpanel_fuel_meter_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the panel frame (-0.48,-0.35) scale 1.25 and scales the bar by 1 - fx value" */

extern void hudpanel_fuel_meter_update(void);


/* hudpanel_clump_destroy */

/* "RpClumpDestroy(g_hudpanel_6592d4)" */

extern void hudpanel_clump_destroy(void);


/* hudpanel_render_if_loaded */

/* "hudpanel_render_group when g_hudpanel_6592dc is set" */

extern void hudpanel_render_if_loaded(void);


/* team_colour_table_init_4 */

/* "fills 12 floats (four team colours)" */

/*
 * Fills the team colour table at g_hudpanel_603f4c with TEAM_COLOUR_FLOATS floats
 * (four team colours of three floats each), then clears g_kc_659790.
 * Takes no parameters and returns nothing. The floats decode as
 * (0.48, 0.25, 1.0), (0.48, 0.15, 1.0), (0.48, 0.05, 1.0), (0.48, -0.05, 1.0)
 * (meaning inferred).
 */
/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void team_colour_table_init_4(void)
{
  int remaining;
  undefined4 *src;
  undefined4 *dst;
  float colour_table [TEAM_COLOUR_FLOATS];

  /* Build the float values in the local table. */
  colour_table[0] = 0.48f;
  colour_table[1] = 0.25f;
  colour_table[2] = 1.0f;
  colour_table[3] = 0.48f;
  colour_table[4] = 0.15f;
  colour_table[5] = 1.0f;
  colour_table[6] = 0.48f;
  colour_table[7] = 0.05f;
  colour_table[8] = 1.0f;
  colour_table[9] = 0.48f;
  colour_table[10] = -0.05f;
  colour_table[0xb] = 1.0f;
  src = (undefined4 *)colour_table;
  dst = &g_hudpanel_603f4c;
  /* Copy TEAM_COLOUR_FLOATS floats from the local table into the global table. */
  for (remaining = TEAM_COLOUR_FLOATS; remaining != 0; remaining = remaining + -1) {
    *dst = *src;
    src = src + 1;
    dst = dst + 1;
  }
  g_kc_659790 = 0;
  return;
}


/* kc_panel_render: uninstalled, see output/tier2_candidates2/reg_kc_panel_render.c.txt */
extern void kc_panel_render(void);


/* hudpanel_endpoint_659a78_clear */

/* "g_endpoint_659a78 = 0; return;" */

extern void hudpanel_endpoint_659a78_clear(void);


/* hudpanel_countdown_update */

/* "slides the countdown panel at y 0.35 after 2.4s and advances its timers" */

/*
 * Per-frame update of the countdown HUD panel (inferred from the callees).
 * Rebuilds the panel's transform from an identity matrix at (0, COUNTDOWN_REST_Y, 1),
 * slides it upwards once g_hudpanel_659aa8 (elapsed seconds) passes 2.4, scales the
 * frame by 1.125, then bumps the frame counter g_hudpanel_659aa4 and adds g_tick_seconds
 * to the elapsed-seconds timer g_hudpanel_659aa8. Takes no parameters and returns nothing.
 */
/* TIER2: validated 2026-10-18 authority=diffemu */
void hudpanel_countdown_update(void)
{
  RwV3d panel_scale;
  RwV3d panel_offset;
  RwMatrix panel_matrix;

  /* Start from an identity matrix (inferred), then set the translation and scale vectors. */
  panel_matrix.flags = panel_matrix.flags | PANEL_MATRIX_FLAGS;
  panel_offset.x = 0.0;
  panel_offset.y = COUNTDOWN_REST_Y;
  panel_offset.z = 1.0;
  panel_scale.x = 1.0;
  panel_scale.y = 1.0;
  panel_scale.z = 1.0;
  panel_matrix.at.z = 1.0;
  panel_matrix.up.y = 1.0;
  panel_matrix.right.x = 1.0;
  panel_matrix.up.x = 0.0;
  panel_matrix.right.z = 0.0;
  panel_matrix.right.y = 0.0;
  panel_matrix.at.y = 0.0;
  panel_matrix.at.x = 0.0;
  panel_matrix.up.z = 0.0;
  panel_matrix.pos.z = 0.0;
  panel_matrix.pos.y = 0.0;
  panel_matrix.pos.x = 0.0;
  /* After 2.4 s the panel slides up from the rest height at 0.25 units per second (inferred). */
  if (k_f_2_4 < g_hudpanel_659aa8) {
    panel_offset.y = (g_hudpanel_659aa8 - k_f_2_4) * k_f_0_25 + k_f_0_35;
  }
  /* Apply the translation to the matrix, then set the panel frame's transform. */
  RwMatrixTranslate(&panel_matrix,&panel_offset,rwCOMBINEREPLACE);
  RwFrameTransform(g_hudpanel_659aa0,&panel_matrix,rwCOMBINEREPLACE);
  /* Scale the panel frame by 1.125 (k_f_1_125) on top of its current transform. */
  panel_scale.x = panel_scale.x * k_f_1_125;
  panel_scale.y = panel_scale.y * k_f_1_125;
  panel_scale.z = panel_scale.z * k_f_1_125;
  RwFrameScale(g_hudpanel_659aa0,&panel_scale,rwCOMBINEPRECONCAT);
  /* Advance the update counter and the elapsed-seconds timer. */
  g_hudpanel_659aa4 = g_hudpanel_659aa4 + 1;
  g_hudpanel_659aa8 = g_hudpanel_659aa8 + g_tick_seconds;
  return;
}


/* team_endpoint_panel_destroy */

/* "RpClumpDestroy(0x659A9C)" */

extern void team_endpoint_panel_destroy(void);


/* endpoint_panel_get_value */

/* "(out): g_hudpanel_659aa8" */

extern void endpoint_panel_get_value(undefined4 *param_1);


/* stopclock_render_timed_parts_forward */

/* "calls stopclock_render_timed_parts" */

extern void stopclock_render_timed_parts_forward(void);


/* hudpanel_score_digits_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "positions the panel (0.48,0.35) and scales the bar to 0, 1/3, 2/3, 1 by the counter" */

extern void hudpanel_score_digits_update(void);


/* stopclock_destroy */

/* "RpClumpDestroy(0x659B00)" */

extern void stopclock_destroy(void);


/* stopclock_render */

/* "renders the stop clock clump when 0x659B08 is set" */

extern void __cdecl stopclock_render(char *param_1);


/* hud_game_vars_clear */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the counters at 0x659CB8..0x659CF8" */

extern void hud_game_vars_clear(void);


/* vehicle_atomics_destroy_659b30 */

/* "destroys the atomics of the 0x659B30 table" */

extern void vehicle_atomics_destroy_659b30(void);


/* game_set_game_659d00 */

/* "g_game_659d00 = param_1; return;" */

extern void game_set_game_659d00(undefined4 param_1);


/* game_get_game_659d00 */

/* "return g_game_659d00;" */

extern undefined4 game_get_game_659d00(void);


/* hudpanel_load_indicator_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "places the load indicator for phase 7 (0.35,0.2, scale 1.5) or otherwise (-0.375,-0.3, 1.125)"
    */

extern void hudpanel_load_indicator_update(void);


/* vehicle_markers_render_if_loaded */

/* "game_fn_421720 when g_game_659d00 is set" */

extern void vehicle_markers_render_if_loaded(void);


/* course_table_select */

/* "(course id): g_current_course_entry = the g_course_table record with that id" */

extern void __cdecl course_table_select(char *param_1,char *param_2,char *param_3,char *param_4, char *param_5,char *param_6,char *param_7,char *param_8, char *param_9,char *param_10,char *param_11);


/* course_call_cb0 */

/* "jmp [entry+0x14]: post load, called by course_open_and_run_scripts" */

extern void __cdecl course_call_cb0(char *param_1);


/* course_call_cb1 */

/* "jmp [entry+0x18]: per frame update, called by course_update" */

extern void __cdecl course_call_cb1(char *param_1,char *param_2);



