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
/*005ddc28*/ extern float k_f_0;
/*005de490*/ extern float k_f_0_02;
/*00609008*/ extern undefined g_race_setup_609008;
/*006090d8*/ extern undefined g_race_setup_6090d8;
/*006091f8*/ extern undefined g_race_setup_6091f8;
/*006092c8*/ extern undefined g_race_setup_6092c8;
/*00609398*/ extern undefined g_race_setup_609398;
/*00609468*/ extern undefined g_race_setup_609468;
/*0065a9e8*/ extern undefined4 car_damage;
/*0065c0d0*/ extern undefined g_game_65c0d0;
/*0065c90c*/ extern char *g_vehicle_render_65c90c;
/*0065d80c*/ extern char *g_game_65d80c;
/*0065d810*/ extern char *g_game_65d810;
/*00660568*/ extern undefined4 *g_game_660568;
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
#define SKIDMARK_RECORD_SIZE (0xf1c)
#define SKIDMARK_VERTS_PER_SEGMENT (3)
#define FLOAT_7_9999995_BITS (0x40ffffff)
/* vehicle_skidmarks_render */

/* WARNING: Function: __chkstk replaced with injection: alloca_probe */
/* "builds an Im3D vertex list from the skid segments (0xF1C byte records) and renders them"
 * slot_index: vehicle slot whose skid record block to draw (inferred)
 * Returns nothing. Does nothing when the slot has no segments.
 */

void __cdecl vehicle_skidmarks_render(int slot_index)

{
  undefined4 dword_a;
  undefined4 dword_b;
  undefined4 *vertex_out;      /* vertex dwords are copied as bits (the original uses mov; a RwReal destination converted the integer bits with fild) */
  void *transform_result;
  undefined4 *src_segment;
  RwGlobalsRelease *rw_globals;
  int record_offset;
  int segment_count;
  RwIm3DVertex im3d_vertices [191];
  undefined4 frame_pad;

  rw_globals = RwEngineInstance;
  frame_pad = 0x425b2a;
  record_offset = slot_index * SKIDMARK_RECORD_SIZE;
  segment_count = *(int *)((char *)&g_game_65d80c + record_offset);
  /* slot has segments: advance the frame counters and update the fade value (inferred) */
  if (segment_count != 0) {
    g_vehicle_render_7ef74c = g_vehicle_render_7ef74c + float_1_0;
    g_vehicle_render_7ef750 = (float)segment_count;
    *(float *)((char *)&g_game_65d810 + record_offset) =
         ((float_1_0 - *(float *)(*(char **)&RwCameraGetCurrentCamera() + 0x68)) + float_1_0) *
         k_f_0_02;
    if (0 < segment_count) {
      segment_count = *(int *)((char *)&g_game_65d80c + record_offset);
      src_segment = (undefined4 *)((char *)&g_vehicle_render_65c90c + record_offset);
      vertex_out = (undefined4 *)&im3d_vertices[0].objVertex.z;
      /* loop: each segment is 15 dwords of source and becomes SKIDMARK_VERTS_PER_SEGMENT Im3D vertices (27 dwords) */
      do {
        dword_a = src_segment[4];
        vertex_out[-2] = src_segment[-5];
        vertex_out[-1] = src_segment[-4];
        *vertex_out = src_segment[-3];
        dword_b = src_segment[-2];
        vertex_out[5] = dword_a;
        dword_a = src_segment[5];
        vertex_out[7] = dword_b;
        dword_b = src_segment[-1];
        vertex_out[6] = dword_a;
        dword_a = src_segment[6];
        vertex_out[8] = dword_b;
        vertex_out[9] = *src_segment;
        dword_b = src_segment[1];
        vertex_out[0xe] = dword_a;
        dword_a = src_segment[7];
        vertex_out[0x10] = dword_b;
        dword_b = src_segment[2];
        vertex_out[0xf] = dword_a;
        dword_a = src_segment[8];
        vertex_out[4] = FLOAT_7_9999995_BITS;      /* 7.9999995f bit pattern into a dword */
        vertex_out[0x17] = dword_a;
        dword_a = src_segment[9];
        vertex_out[0x11] = dword_b;
        dword_b = src_segment[3];
        vertex_out[0xd] = FLOAT_7_9999995_BITS;      /* 7.9999995f bit pattern into a dword */
        vertex_out[0x18] = dword_a;
        vertex_out[0x12] = dword_b;
        vertex_out[0x16] = FLOAT_7_9999995_BITS;      /* 7.9999995f bit pattern into a dword */
        src_segment = src_segment + 0xf;
        vertex_out = vertex_out + 0x1b;
        segment_count = segment_count + -1;
      } while (segment_count != 0);
    }
    /* bind the skid texture when it is loaded, then set the blend state for the trail */
    if ((g_game_660568 != (undefined4 *)0x0) && (*(void **)g_game_660568 != (void *)0x0)) {
      (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(rw_globals))(rwRENDERSTATETEXTURERASTER,*(void **)g_game_660568);
      rw_globals = RwEngineInstance;
    }
    (*RwGlobalsRelease_dOpenDevice_fpRenderStateSet(rw_globals))(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x1);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,(void *)rwTEXTUREADDRESSCLAMP);
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDONE);
    /* transform the vertex list, then draw it as a triangle list if the transform succeeded */
    transform_result = RwIm3DTransform(im3d_vertices,*(int *)((char *)&g_game_65d80c + record_offset) * SKIDMARK_VERTS_PER_SEGMENT,(RwMatrix *)0x0,(rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA));
    if (transform_result != (void *)0x0) {
      RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
      RwIm3DEnd();
    }
    RwRenderStateSet(rwRENDERSTATESRCBLEND,(void *)rwBLENDSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,(void *)rwBLENDINVSRCALPHA);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,(void *)0x0);
  }
  return;
}
