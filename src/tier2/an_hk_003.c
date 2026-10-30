#include "mfl_tier2.h"
extern undefined1 g_time_trial_658d04[];
/*004042c0*/ extern double scene_get_bomb_5fb0a8();
/*00408c40*/ extern double fx_current_object_get_float();
/*0040fdc0*/ extern int race_get_phase();
/*00414050*/ extern int time_trial_record_step();
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
/*0041dce0*/ extern undefined time_trial_panel_step_part_b(undefined4 p0);
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
/*004b2b48*/ extern int _ftol();
/*004b4ba7*/ extern int entry();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d1500*/ extern struct RwFrame * RwFrameScale(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
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
/*005e64a4*/ extern float k_f_0;
/*005de1d0*/ extern float k_f_1_25;
/*005de288*/ extern float k_f_1_125;
/*005de2f8*/ extern float k_f_n90;
/*005de398*/ extern float k_f_0_35;
/*005de39c*/ extern float k_f_2_4;
/*00603f4c*/ extern undefined4 g_kc_panel_origin_table;
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
/*00658b34*/ extern undefined4 g_hudpanel_658b34;
/*00658b40*/ extern undefined4 g_btb_658b40;
/*00658b44*/ extern undefined4 g_hudpanel_658b44;
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
/*00658b94*/ extern undefined g_btb_panel_658b94;
/*00658b98*/ extern undefined g_btb_panel_658b98;
/*00658b9c*/ extern undefined g_btb_panel_658b9c;
/*00658ba0*/ extern undefined g_btb_panel_658ba0;
/*00658ba4*/ extern undefined g_btb_panel_658ba4;
/*00658ba8*/ extern undefined g_btb_panel_658ba8;
/*00658bd0*/ extern undefined g_btb_panel_658bd0;
/*00658bf0*/ extern undefined g_btb_panel_658bf0;
/*00658bf4*/ extern undefined g_btb_panel_658bf4;
/*00658bf8*/ extern undefined g_btb_panel_658bf8;
/*00658c00*/ extern undefined g_btb_panel_658c00;
/*00658c10*/ extern undefined g_btb_panel_658c10;
/*00658cd8*/ extern undefined4 g_btb_658cd8;
/*00658ce0*/ extern undefined4 g_hudpanel_658ce0;
/*00658ce4*/ extern undefined4 g_time_trial_658ce4;
/*00658d00*/ extern undefined4 g_time_trial_658d00;
/*00658d18*/ extern undefined4 g_hudpanel_658d18;
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
#define OBJ_AXIS_X (0x20)
#define OBJ_AXIS_Y (0x24)
#define OBJ_AXIS_Z (0x28)
#define OBJ_POS_X (0x30)
#define OBJ_POS_Y (0x34)
#define OBJ_POS_Z (0x38)
#define REF_STRIDE (0x10)
#define PARTS_PER_SLOT (4)
/* time_trial_panel_wheel_spin */

/* "per slot spin angle of four parts from distance*direction, wrapped at 360"
 *
 * For each slot, advances the four spin angles of its parts (kept in [0, 360)) by
 * distance * heading_dot, where distance is the length of the vector from the slot's
 * reference point (ref_pos, one 0x40-byte record per slot) to the slot object's position,
 * and heading_dot is that vector dotted with the object's axis (inferred).
 * Then passes each slot's angles to btb_panel_slot_set_scale.
 *
 * slot_objs: array of pointers to slot objects (one per slot, 4 bytes each).
 */

void time_trial_panel_wheel_spin(char *slot_objs)

{
  char *slot_obj;
  volatile float angle;   /* rounds the running sum to float like the original's stack temp */
  float *ref_pos;
  int slot;
  float *spin_ptr;
  float spin_step;
  undefined4 *slot_angles;
  float heading_dot;
  RwV3d delta;

  slot = 0;
  slot_angles = &g_hudpanel_658ce0;
  ref_pos = (float *)&g_hudpanel_658d4c;
  spin_ptr = (float *)&g_time_trial_658ce4;
  /* one iteration per slot; spin_ptr starts at the second float of slot 0's angles */
  do {
    slot_obj = *(char **)(slot_objs + slot * 4);
    /* delta = object position - reference point */
    delta.x = *(float *)(slot_obj + OBJ_POS_X) - ref_pos[-1];
    delta.y = *(float *)(slot_obj + OBJ_POS_Y) - *ref_pos;
    delta.z = *(float *)(slot_obj + OBJ_POS_Z) - ref_pos[1];
    /* object at the reference point: no spin; otherwise normalize delta (returns its length)
       and dot it with the object's axis */
    if (((delta.x == k_f_0) && (delta.y == k_f_0)) && (delta.z == k_f_0)) {
      heading_dot = 0.0;
      spin_step = k_f_0;
    }
    else {
      spin_step = RwV3dNormalize(&delta,&delta);
      heading_dot = *(float *)(slot_obj + OBJ_AXIS_Y) * delta.y;
      heading_dot = heading_dot + *(float *)(slot_obj + OBJ_AXIS_X) * delta.x;
      heading_dot = heading_dot + *(float *)(slot_obj + OBJ_AXIS_Z) * delta.z;
    }
    /* spin step for this slot, in degrees; each part adds it and wraps at 360 */
    spin_step = spin_step * heading_dot;
    spin_step = spin_step * k_f_360;
    /* the four parts: add the step, subtract 360 once if it reached 360 */
    angle = spin_step + spin_ptr[-1];
    spin_ptr[-1] = angle;
    if (k_f_360 <= angle) {
      spin_ptr[-1] = angle - k_f_360;
    }
    angle = spin_step + *spin_ptr;
    *spin_ptr = angle;
    if (k_f_360 <= angle) {
      *spin_ptr = angle - k_f_360;
    }
    angle = spin_step + spin_ptr[1];
    spin_ptr[1] = angle;
    if (k_f_360 <= angle) {
      spin_ptr[1] = angle - k_f_360;
    }
    angle = spin_step + spin_ptr[2];
    spin_ptr[2] = angle;
    if (k_f_360 <= angle) {
      spin_ptr[2] = angle - k_f_360;
    }
    /* push this slot's four angles to the panel */
    btb_panel_slot_set_scale(slot,slot_angles);
    spin_ptr = spin_ptr + PARTS_PER_SLOT;
    slot_angles = slot_angles + PARTS_PER_SLOT;
    slot = slot + 1;
    ref_pos = ref_pos + REF_STRIDE;
  } while (spin_ptr < (float *)&g_time_trial_658d04);
  return;
}
