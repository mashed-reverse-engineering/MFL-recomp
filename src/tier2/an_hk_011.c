#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*0040ff20*/ extern int race_setup_get_slot_type();
/*0041b810*/ extern int race_mark_ai_slots();
/*0041b9d0*/ extern int hudpanel_slot_init_player();
/*004220c0*/ extern int vehicle_get_position_148();
/*004246f0*/ extern int car_set_damage();
/*00424ab0*/ extern int rwp_world_current_deactivate();
/*004266c0*/ extern int race_events_process_log();
/*00426bb0*/ extern int ai_editor_paint_input();
/*00426de0*/ extern int ai_editor_cursor_move();
/*004271e0*/ extern int ai_editor();
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00428a20*/ extern int cars_step_race_phase_and_respawn();
/*00428e30*/ extern undefined __cdecl player_stats_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*0042a440*/ extern int course_get_directional_light();
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00467890*/ extern int player_status_set();
/*0047a640*/ extern int car_deactivate();
/*0047a810*/ extern int car_set_controls_disabled();
/*0047f300*/ extern int cars_step();
/*00481fd0*/ extern int random_range();
/*004a8420*/ extern int debug_printf();
/*004b3430*/ extern int __chkstk();
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d3cd0*/ extern RwMatrix * RwMatrixMultiply(RwMatrix *p0, RwMatrix *p1, RwMatrix *p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0058ca00*/ extern void FUN_0058ca00(undefined4 p0);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*00607868*/ extern undefined4 g_vehicle_load_607868;
/*00609008*/ extern undefined g_race_setup_609008;
/*006090d8*/ extern undefined g_race_setup_6090d8;
/*006091f8*/ extern undefined g_race_setup_6091f8;
/*006092c8*/ extern undefined g_race_setup_6092c8;
/*00609398*/ extern undefined g_race_setup_609398;
/*00609468*/ extern undefined g_race_setup_609468;
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*0065c8f8*/ extern undefined g_game_65c8f8;
/*0065d808*/ extern undefined g_load_groundbeam_65d808;
/*00660568*/ extern undefined4 g_game_660568;
/*006605a0*/ extern undefined g_game_6605a0;
/*006605b4*/ extern undefined g_vehicle_render_6605b4;
/*006614a0*/ extern undefined g_game_6614a0;
/*006614a4*/ extern undefined g_game_6614a4;
/*006614a8*/ extern undefined g_game_6614a8;
/*006614ac*/ extern undefined g_game_6614ac;
/*006614c0*/ extern undefined g_vehicle_load_6614c0;
/*006614c4*/ extern undefined g_vehicle_load_6614c4;
/*006614c8*/ extern undefined g_vehicle_load_6614c8;
/*006614cc*/ extern undefined g_vehicle_load_6614cc;
/*006614d0*/ extern undefined g_vehicle_load_6614d0;
/*006614d4*/ extern undefined g_vehicle_load_6614d4;
/*006614d8*/ extern undefined g_game_6614d8;
/*006614d9*/ extern undefined g_vehicle_render_6614d9;
/*006614da*/ extern undefined g_vehicle_render_6614da;
/*006614db*/ extern undefined g_vehicle_render_6614db;
/*006614dc*/ extern undefined g_vehicle_load_6614dc;
/*006614e0*/ extern undefined g_vehicle_render_6614e0;
/*006614e4*/ extern undefined g_game_6614e4;
/*006614e8*/ extern undefined g_game_6614e8;
/*006614ec*/ extern undefined g_vehicle_render_6614ec;
/*006614f0*/ extern undefined g_game_6614f0;
/*006642f0*/ extern undefined4 g_game_6642f0;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef7b8*/ extern undefined4 game_mode;
/*007ef820*/ extern struct PlayerInput g_player_inputs[12];
/*007efce0*/ extern struct PlayerInput g_player_inputs_previous[12];
/*007f01fc*/ extern struct PlayerSlot g_player_slots[4];
/*007f023c*/ extern undefined4 g_ai_editor_mode;
/*007f024c*/ extern undefined4 g_ai_editor_menu_cursor;
/*007f0258*/ extern undefined4 g_ai_editor_step_flag_a;
/*007f025c*/ extern undefined4 g_ai_editor_step_flag_b;
/*007f0260*/ extern undefined4 g_ai_editor_step_counter;
/*007f0278*/ extern undefined4 g_ai_editor_brush;
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
/*008c8fc0*/ extern struct PlayerStatRecord g_player_stats_result[4];
/*008c94a0*/ extern struct PlayerStatRecord g_player_stats_total[4];
/*008d8b30*/ extern undefined4 player_count;

#define AI_EDITOR_BRUSH_MENU_ROW (2) /* g_ai_editor_menu_cursor value that enables brush input (inferred) */
#define BRUSH_MAX (0x7f) /* highest brush value (inferred) */
#define BRUSH_GROUP_SIZE (4) /* brush values step in groups of four (inferred) */
#define BRUSH_GROUP_MASK (3) /* low two bits: position inside a group */
/* PlayerInput gap bytes (offsets 58..61 are unnamed in mfl_tier2.h), set for one frame per press (inferred) */
#define PLAYER_INPUT_BRUSH_GROUP_DOWN (58)
#define PLAYER_INPUT_BRUSH_GROUP_UP (59)
#define PLAYER_INPUT_BRUSH_STEP_DOWN (60)
#define PLAYER_INPUT_BRUSH_STEP_UP (61)

/* ai_editor_brush_select */

/*
 * AI editor brush selector: moves g_ai_editor_brush from the player 0 input bytes.
 * Takes no parameters, returns nothing. Brush values are 0..BRUSH_MAX; the low two bits
 * pick a position inside a group of four (inferred).
 * Each edge-triggered input (pressed this frame, not the previous one) moves the brush:
 *   GROUP_DOWN / GROUP_UP: jump by a whole group of four, clamped to the range.
 *   STEP_DOWN / STEP_UP: move by one, wrapping inside the group (inferred).
 * Group jumps are ignored unless g_ai_editor_menu_cursor == AI_EDITOR_BRUSH_MENU_ROW.
 * Original quote: "AI editor: changes the brush value (g_ai_editor_brush & 3) with the input bytes"
 */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
void ai_editor_brush_select(void)
{
  uint next_brush;
  
  next_brush = g_ai_editor_brush;
  /* group jumps: only on the brush menu row (the step inputs below work on any row) */
  if (g_ai_editor_menu_cursor != AI_EDITOR_BRUSH_MENU_ROW) {
    /* group down, unless it would go below zero */
    if (((*(byte *)((char *)&g_player_inputs[0] + PLAYER_INPUT_BRUSH_GROUP_DOWN)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + PLAYER_INPUT_BRUSH_GROUP_DOWN)) == '\0')) {
      next_brush = g_ai_editor_brush - BRUSH_GROUP_SIZE;
      if ((int)(g_ai_editor_brush - BRUSH_GROUP_SIZE) < 0) {
        next_brush = g_ai_editor_brush;
      }
    }
    g_ai_editor_brush = next_brush;
    next_brush = g_ai_editor_brush;
    /* group up, unless it would go past BRUSH_MAX */
    if (((*(byte *)((char *)&g_player_inputs[0] + PLAYER_INPUT_BRUSH_GROUP_UP)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + PLAYER_INPUT_BRUSH_GROUP_UP)) == '\0')) {
      next_brush = g_ai_editor_brush + BRUSH_GROUP_SIZE;
      if (BRUSH_MAX < (int)(g_ai_editor_brush + BRUSH_GROUP_SIZE)) {
        next_brush = g_ai_editor_brush;
      }
    }
  }
  g_ai_editor_brush = next_brush;
  next_brush = g_ai_editor_brush;
  /* step down by one; from a group's first value (low bits 0) wrap to the group's last (+3) */
  if ((((*(byte *)((char *)&g_player_inputs[0] + PLAYER_INPUT_BRUSH_STEP_DOWN)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + PLAYER_INPUT_BRUSH_STEP_DOWN)) == '\0')) &&
     (next_brush = g_ai_editor_brush - 1, ((byte)(g_ai_editor_brush - 1) & BRUSH_GROUP_MASK) == 3)) {
    g_ai_editor_brush = g_ai_editor_brush + 3;
    next_brush = g_ai_editor_brush;
  }
  g_ai_editor_brush = next_brush;
  next_brush = g_ai_editor_brush;
  /* step up by one; from a group's last value (low bits 3) wrap back to the group's first (-3) */
  if (((*(byte *)((char *)&g_player_inputs[0] + PLAYER_INPUT_BRUSH_STEP_UP)) != '\0') && ((*(byte *)((char *)&g_player_inputs_previous[0] + PLAYER_INPUT_BRUSH_STEP_UP)) == '\0')) {
    next_brush = g_ai_editor_brush + 1;
    if ((g_ai_editor_brush + 1 & BRUSH_GROUP_MASK) == 0) {
      next_brush = g_ai_editor_brush - 3;
    }
  }
  g_ai_editor_brush = next_brush;
  return;
}
