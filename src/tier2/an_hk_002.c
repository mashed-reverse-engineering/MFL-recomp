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
/*004271e0*/ extern int ai_editor();
/*004276b0*/ extern int __cdecl player_stat_get_crowns(undefined4 p0);
/*004276d0*/ extern int __cdecl player_stat_get_pants(undefined4 p0);
/*004276f0*/ extern int __cdecl player_stat_get_crowns_total(undefined4 p0);
/*00427710*/ extern int __cdecl player_stat_get_pants_total(undefined4 p0);
/*00428a20*/ extern int cars_step_race_phase_and_respawn();
/*00428e30*/ extern undefined __cdecl player_stats_update(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*00434a10*/ extern int is_team_mode();
/*00434bb0*/ extern int get_event_type();
/*00467890*/ extern int player_status_set();
/*0047a640*/ extern int car_deactivate();
/*0047a810*/ extern int car_set_controls_disabled();
/*0047f300*/ extern int cars_step();
/*00481fd0*/ extern int random_range();
/*004a8420*/ extern int debug_printf();
/*004b3430*/ extern int __chkstk();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d19d0*/ extern struct RwBBox * RwBBoxAddPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d3cd0*/ extern RwMatrix * RwMatrixMultiply(RwMatrix *p0, RwMatrix *p1, RwMatrix *p2);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*004f6810*/ extern struct RpClump * RpClumpForAllAtomics(struct RpClump *p0, RpAtomicCallBack p1, void *p2);
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005e64a4*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*00609008*/ extern undefined g_race_setup_609008;
/*006090d8*/ extern undefined g_race_setup_6090d8;
/*006091f8*/ extern undefined g_race_setup_6091f8;
/*006092c8*/ extern undefined g_race_setup_6092c8;
/*00609398*/ extern undefined g_race_setup_609398;
/*00609468*/ extern undefined g_race_setup_609468;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*0065c90c*/ extern undefined g_vehicle_render_65c90c;
/*0065d80c*/ extern undefined g_game_65d80c;
/*0065d810*/ extern undefined g_game_65d810;
/*00660568*/ extern undefined4 g_game_660568;
/*006605b4*/ extern char *g_vehicle_render_6605b4;
/*006614a0*/ extern undefined g_game_6614a0;
/*006614d8*/ extern undefined g_game_6614d8;
/*006614d9*/ extern undefined g_vehicle_render_6614d9;
/*006614da*/ extern undefined g_vehicle_render_6614da;
/*006614db*/ extern undefined g_vehicle_render_6614db;
/*006614e0*/ extern char *g_vehicle_render_6614e0;
/*006614e4*/ extern undefined g_game_6614e4;
/*006614ec*/ extern char *g_vehicle_render_6614ec;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007eebc0*/ extern struct GameState g_game_state;
/*007ef74c*/ extern float g_vehicle_render_7ef74c;
/*007ef750*/ extern float g_vehicle_render_7ef750;
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
#define TYRE_MARK_RECORD_SIZE (0xf54)
#define TYRE_MARK_SEGMENT_DWORDS (0xf)
#define IM3D_VERTS_PER_SEGMENT (3)
#define IM3D_DWORDS_PER_SEGMENT (0x1b)
#define IM3D_TRANSFORM_FLAGS (rwIM3D_VERTEXXYZ | rwIM3D_VERTEXUV | rwIM3D_VERTEXRGBA)
/* vehicle_tyre_marks_render */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* "builds an Im3D vertex list from the tyre mark segments (0xF54 byte records) and renders them" */
/* Draws the tyre marks of one vehicle as an Im3D triangle list.
 * vehicle_index: index of the vehicle; its tyre mark record is TYRE_MARK_RECORD_SIZE bytes (inferred from the stride).
 * vehicle: the vehicle; the float at +0x68 (inferred: wear or damage) sets the tyre mark intensity stored in the record.
 * Returns nothing. Does nothing when the record has no segments. */

void __cdecl vehicle_tyre_marks_render(int vehicle_index,char *vehicle)

{
  undefined4 copy_a;
  undefined4 copy_b;
  undefined4 *vertex_out;      /* vertex dwords are copied as bits (the original uses mov; a RwReal destination converted the integer bits with fild) */
  undefined4 *segment;
  undefined4 packed_colour;
  void *texture_raster;
  int record_offset;
  int segments_left;
  RwIm3DVertex im3d_vertices [191];
  undefined4 frame_marker;

  frame_marker = 0x4263fa;
  record_offset = vehicle_index * TYRE_MARK_RECORD_SIZE;
  texture_raster = (void *)0x0;
  /* segment count != 0: this vehicle has tyre marks to draw */
  if (*(int *)((char *)&g_vehicle_render_6614e0 + record_offset) != 0) {
    /* intensity from the vehicle value at +0x68 (inferred) */
    *(float *)((char *)&g_vehicle_render_6614ec + record_offset) =
         ((float_1_0 - *(float *)(vehicle + 0x68)) + float_1_0) * k_f_0_01;
    /* texture lookup: the texture index in g_game_6614e4 (inferred) selects a raster from the table at g_game_6614a0; NULL means no texture */
    if (*(undefined4 **)((char *)&g_game_6614a0 + *(int *)((char *)&g_game_6614e4 + record_offset) * 4 + record_offset) !=
        (undefined4 *)0x0) {
      texture_raster = **(void ***)
                         ((char *)&g_game_6614a0 + *(int *)((char *)&g_game_6614e4 + record_offset) * 4 + record_offset);
    }
    if (0 < *(int *)((char *)&g_vehicle_render_6614e0 + record_offset)) {
      segments_left = *(int *)((char *)&g_vehicle_render_6614e0 + record_offset);
      packed_colour = CONCAT31(CONCAT21(CONCAT11((&g_vehicle_render_6614db)[record_offset],(&g_game_6614d8)[record_offset]),
                                (&g_vehicle_render_6614d9)[record_offset]),(&g_vehicle_render_6614da)[record_offset]
                      );
      segment = (undefined4 *)((char *)&g_vehicle_render_6605b4 + record_offset);
      vertex_out = (undefined4 *)&im3d_vertices[0].objVertex.z;
      /* loop per segment: 15 dwords in (TYRE_MARK_SEGMENT_DWORDS), 3 Im3D vertices out (one triangle), packed_colour written into the colour dwords */
      do {
        copy_a = segment[4];
        vertex_out[-2] = segment[-5];
        vertex_out[-1] = segment[-4];
        *vertex_out = segment[-3];
        copy_b = segment[-2];
        vertex_out[5] = copy_a;
        copy_a = segment[5];
        vertex_out[7] = copy_b;
        copy_b = segment[-1];
        vertex_out[6] = copy_a;
        copy_a = segment[6];
        vertex_out[8] = copy_b;
        vertex_out[9] = *segment;
        copy_b = segment[1];
        vertex_out[0xe] = copy_a;
        copy_a = segment[7];
        vertex_out[0x10] = copy_b;
        copy_b = segment[2];
        vertex_out[0xf] = copy_a;
        copy_a = segment[8];
        vertex_out[4] = packed_colour;
        vertex_out[0x17] = copy_a;
        copy_a = segment[9];
        vertex_out[0x11] = copy_b;
        copy_b = segment[3];
        vertex_out[0xd] = packed_colour;
        vertex_out[0x18] = copy_a;
        vertex_out[0x12] = copy_b;
        vertex_out[0x16] = packed_colour;
        segment = segment + TYRE_MARK_SEGMENT_DWORDS;
        vertex_out = vertex_out + IM3D_DWORDS_PER_SEGMENT;
        segments_left = segments_left + -1;
      } while (segments_left != 0);
    }
    /* bind the segment texture when one was found */
    if (texture_raster != (void *)0x0) {
      RwRenderStateSet(rwRENDERSTATETEXTURERASTER,texture_raster);
    }
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,(void *)rwTEXTUREADDRESSCLAMP);
    texture_raster = RwIm3DTransform(im3d_vertices,*(int *)((char *)&g_vehicle_render_6614e0 + record_offset) * IM3D_VERTS_PER_SEGMENT,
                             (RwMatrix *)0x0,IM3D_TRANSFORM_FLAGS);
    /* RwIm3DTransform's return value is reused as the flag (inferred: NULL on failure); draw the triangle list only when it succeeded */
    if (texture_raster != (void *)0x0) {
      RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
      RwIm3DEnd();
    }
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
  }
  return;
}
