#include "mfl_tier2.h"
extern char UNK_00484940[];
extern char UNK_005df51c[];
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*0042a3f0*/ extern int course_get_rpworld();
/*00452b90*/ extern undefined __cdecl viewport_camera_sync_to_main(undefined4 p0, undefined4 p1, undefined4 p2);
/*00457170*/ extern int spline_object_update();
/*00457430*/ extern int course_spline_objects_create();
/*004574a0*/ extern int course_spline_objects_update();
/*00457620*/ extern int load_shadow_models();
/*004577c0*/ extern int course_tierra_n_rotating_object_update();
/*004577e0*/ extern int course_objects_render();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004817b0*/ extern int atomic_plugin_get_bbox();
/*00481ea0*/ extern int player_marker_set_clump();
/*00481ee0*/ extern int player_marker_set_enabled();
/*00481fd0*/ extern double random_range();
/*00482270*/ extern undefined __stdcall spline_get_length(struct RpSpline *p0);
/*004848b0*/ extern undefined __cdecl clump_for_all_materials(struct RpClump *p0, void *p1);
/*00484ad0*/ extern int pool_list_destroy();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00492d10*/ extern int rwp_object_set_active();
/*00495c30*/ extern int viewport_get_camera();
/*0049a980*/ extern int fx_slot_release();
/*0049a9b0*/ extern int fx_slot_find_free();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*0049c8f0*/ extern int particle_slot_call_render_method();
/*004a1860*/ extern int game_set_option_flags();
/*004a5d10*/ extern int object_frames_create_with_matrices();
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004a5e50*/ extern int object_set_colour();
/*004b4ba7*/ extern int entry();
/*004c3f60*/ extern RpAtomic * clump_first_atomic(RpClump * p0);
/*004c5360*/ extern void atomic_set_render_flag(RpAtomic * p0, int p1);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1ca0*/ extern struct RwCamera * RwCameraSetFarClipPlane(struct RwCamera *p0, RwReal p1);
/*004d1d40*/ extern struct RwCamera * RwCameraClear(struct RwCamera *p0, struct RwRGBA *p1, RwInt32 p2);
/*004d1e10*/ extern struct RwCamera * RwCameraSetViewWindow(struct RwCamera *p0, struct RwV2d *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004f4440*/ extern struct RpWorld * RpWorldAddCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f4470*/ extern struct RpWorld * RpWorldRemoveCamera(struct RpWorld *p0, struct RwCamera *p1);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*0050b5e0*/ extern int nop_stub();
/*005686d0*/ extern undefined __cdecl RpLODAtomicHookRender(undefined4 p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd9c8*/ extern float k_f_0_16666667;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddc28*/ extern float k_f_0;
/*005df5d8*/ extern float k_f_146;
/*005df5dc*/ extern float k_f_n122;
/*0060d8fc*/ extern undefined4 g_course_spline_60d8fc;
/*006a2638*/ extern undefined4 g_course_sandsnow_6a2638;
/*006a263c*/ extern int g_course_storm_6a263c;
/*006a2640*/ extern undefined4 g_course_storm_6a2640;
/*006a2648*/ extern float g_course_tierra_6a2648;
/*006a264c*/ extern float g_course_tierra_6a264c;
/*006a2650*/ extern float g_course_tierra_6a2650;
/*006a2654*/ extern int g_course_tierra_6a2654;
/*006a2658*/ extern undefined4 g_course_tierra_6a2658;
/*006a265c*/ extern undefined4 g_course_tierra_6a265c;
/*006a2660*/ extern undefined4 g_course_tierra_6a2660;
/*006a2664*/ extern undefined4 g_course_tierra_6a2664;
/*006a2680*/ extern undefined4 g_course_objects_6a2680;
/*006a26b8*/ extern undefined4 g_course_set_6a26b8;
/*006a26c0*/ extern undefined4 g_course_6a26c0;
/*006a26c4*/ extern float g_course_6a26c4;
/*006a26c8*/ extern undefined4 g_course_6a26c8;
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a26d0*/ extern undefined4 g_course_clumps_6a26d0;
/*006a26d4*/ extern undefined4 g_course_clumps_6a26d4;
/*006a26fc*/ extern undefined4 g_course_6a26fc;
/*006a2700*/ extern undefined4 g_course_6a2700;
/*006a2704*/ extern undefined4 g_course_6a2704;
/*006a2708*/ extern undefined4 g_course_6a2708;
/*006a270c*/ extern undefined4 g_course_6a270c;
/*006a2710*/ extern undefined4 g_course_6a2710;
/*006a2714*/ extern undefined4 g_course_6a2714;
/*006a2718*/ extern undefined4 g_course_6a2718;
/*006a271c*/ extern undefined4 g_course_6a271c;
/*006a2720*/ extern undefined4 g_course_6a2720;
/*006a2724*/ extern float g_course_6a2724;
/*006a2728*/ extern undefined4 g_course_6a2728;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2730*/ extern undefined4 g_course_clumps_6a2730;
/*006a2734*/ extern undefined4 g_course_clumps_6a2734;
/*006a275c*/ extern undefined4 g_course_6a275c;
/*006a2760*/ extern undefined4 g_course_6a2760;
/*006a2764*/ extern undefined4 g_course_6a2764;
/*006a2768*/ extern undefined4 g_course_6a2768;
/*006a276c*/ extern undefined4 g_course_6a276c;
/*006a2770*/ extern undefined4 g_course_6a2770;
/*006a2774*/ extern undefined4 g_course_6a2774;
/*006a2778*/ extern undefined4 g_course_6a2778;
/*006a277c*/ extern undefined4 g_course_6a277c;
/*006a2780*/ extern undefined4 g_course_sands_6a2780;
/*006a2784*/ extern undefined4 g_course_6a2784;
/*006a2794*/ extern float g_course_superg_6a2794;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*008bf084*/ extern undefined4 g_course_training_8bf084;
/*008bf088*/ extern undefined4 g_course_sands_8bf088;
/*008bf140*/ extern undefined4 g_course_8bf140;
/*008bf150*/ extern undefined4 g_course_8bf150;
/*008bf154*/ extern undefined4 g_course_8bf154;
/*008bf16c*/ extern undefined4 g_course_object_8bf16c;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3fa0*/ extern undefined4 g_course_8c3fa0;
/*008c3fc0*/ extern undefined4 g_course_8c3fc0;
/*008c47d4*/ extern undefined4 g_course_objects_8c47d4;
/*008c4b08*/ extern undefined4 g_course_forest_8c4b08;
/*008c4b20*/ extern undefined4 g_course_tierra_8c4b20;
/*008c5280*/ extern undefined4 g_course_storm_8c5280;
/*008c5284*/ extern undefined4 g_course_storm_8c5284;
/*008c5288*/ extern undefined4 g_course_storm_8c5288;
/*008c528c*/ extern undefined4 g_course_storm_8c528c;
/*008c5290*/ extern undefined4 g_course_storm_8c5290;
/*008c5294*/ extern undefined4 g_course_storm_8c5294;
/*008c5298*/ extern undefined4 g_course_storm_8c5298;
/*008c52a0*/ extern undefined4 g_course_storm_8c52a0;
/*008c52a4*/ extern undefined4 g_course_storm_8c52a4;
/*008c52a8*/ extern undefined4 g_course_storm_8c52a8;
/*008c52b0*/ extern undefined4 g_course_storm_8c52b0;
/*008c52b4*/ extern undefined4 g_course_storm_8c52b4;
/*008c52b8*/ extern undefined4 g_course_storm_8c52b8;
/*008c5500*/ extern undefined g_course_spline_8c5500;
/*008c8000*/ extern RwIm2DVertex g_im2d_vertices[4];
/*008d8b30*/ extern undefined4 player_count;
/* course_sandsnow_cb2 */

extern void course_sandsnow_cb2(char *param_1);


/* course_storm_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_storm_cb0(char *param_1);


/* course_storm_cb2 */

extern void course_storm_cb2(char *param_1);


/* course_storm_cb5 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_storm_cb5(void);


/* course_tierra_n_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_tierra_n_cb0(char *param_1);


/* course_tierra_n_cb6 */

extern void course_tierra_n_cb6(void);


/* course_tierra_n_cb7 */

extern void course_tierra_n_cb7(void);


/* course_tierra_n_cb1 */

extern void course_tierra_n_cb1(char *param_1,char *param_2);


/* course_tierra_n_cb2 */

extern void course_tierra_n_cb2(char *param_1);


/* course_tierra_n_cb5 */

extern void course_tierra_n_cb5(void);


/* course_forest_cb0 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_forest_cb0(char *param_1);


/* course_forest_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern RpClump * course_forest_cb1(char *param_1,char *param_2);


/* course_forest_cb2 */

extern void course_forest_cb2(char *param_1);


/* course_c26_stub_return_0 */

/* "returns without work" */

extern undefined4 course_c26_stub_return_0(void);


/* course_object_get_float_16c */

/* "(index): float of the object record" */

extern float10 course_object_get_float_16c(char *param_1);


/* course_c26_checkpoint_scale_value */

/* "a float table entry * 1/6" */

extern float10 course_c26_checkpoint_scale_value(char *param_1);


/* course_object_offsets_init */

/* "fills the table of 80 float offsets (10, 35, 60 ... 235 then pairs) used to place the course
   objects" */

extern void course_object_offsets_init(void);


/* course_objects_reset_physics */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "clears the 10 object records per group and parks their physics objects" */

extern void course_objects_reset_physics(void);


/* trigger_cb_mark_hit_bit */

/* "type 0x15 trigger vs a car id (0xFF00..0xFF07): sets the bit of the object index in the bitmask
   at 0x6A26A4" */

extern void trigger_cb_mark_hit_bit(char *param_1,char *param_2);


/* course_highway_cb6 */

/* WARNING: Removing unreachable block (ram,0x004590b5) */

extern void course_highway_cb6(void);


/* course_object_offsets_init_b */

/* "duplicate of course_object_offsets_init" */

extern void course_object_offsets_init_b(void);

/* IEEE-754 bits of -1.0f */
#define FLOAT_MINUS_ONE_BITS (0xbf800000)
/* byte offsets in course_state of the clump pointer for each player marker (inferred) */
#define PLAYER_0_MARKER_CLUMP_OFFSET (0xc160)
#define PLAYER_1_MARKER_CLUMP_OFFSET (0xc158)
#define PLAYER_2_MARKER_CLUMP_OFFSET (0xc15c)
#define PLAYER_3_MARKER_CLUMP_OFFSET (0xc12c)


/* course_marker_positions_init */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "initialises six marker position triples and enables the four player markers with their clumps"
    */

/* TIER2: unvalidated 2026-10-11 authority=diffemu */
/* Sets up the course marker data: clears g_course_6a2784, writes the position
   triples (inferred: two (-1, -1, 0) sentinel triples, then six float triples)
   and enables the four player markers, each bound to its clump from course_state. */
/* TIER2: validated 2026-10-18 authority=diffemu */
void course_marker_positions_init(char *course_state)
{
  g_course_6a2784 = 0;
  /* sentinel triple (inferred): -1.0, -1.0, 0 */
  *(undefined4 *)&g_course_6a26c4 = FLOAT_MINUS_ONE_BITS;
  g_course_6a26c0 = FLOAT_MINUS_ONE_BITS;
  g_course_6a26c8 = 0;
  /* second sentinel triple (inferred): -1.0, -1.0, 0 */
  *(undefined4 *)&g_course_6a2724 = FLOAT_MINUS_ONE_BITS;
  g_course_6a2720 = FLOAT_MINUS_ONE_BITS;
  g_course_6a2728 = 0;
  /* position triples (x, y, z) (inferred) */
  g_course_6a26fc = 0xc170978d;
  g_course_6a2700 = 0x409dc28f;
  g_course_6a2704 = 0x42689ba6;
  g_course_6a2708 = 0xc16fa5e3;
  g_course_6a270c = 0x409dc28f;
  g_course_6a2710 = 0x426476c9;
  g_course_6a2714 = 0xc16ec083;
  g_course_6a2718 = 0x409dc28f;
  g_course_6a271c = 0x426079db;
  g_course_6a275c = 0xc1480c4a;
  g_course_6a2760 = 0x409dc28f;
  g_course_6a2764 = 0x42610000;
  g_course_6a2768 = 0xc148f1aa;
  g_course_6a276c = 0x409dc28f;
  g_course_6a2770 = 0x4264fcee;
  g_course_6a2774 = 0xc149e354;
  g_course_6a2778 = 0x409dc28f;
  g_course_6a277c = 0x426521cb;
  /* player markers: player 0 enabled, players 1-3 disabled; each gets its clump */
  player_marker_set_enabled(0,1);
  player_marker_set_clump(0,*(undefined4 *)(course_state + PLAYER_0_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(1,0);
  player_marker_set_clump(1,*(undefined4 *)(course_state + PLAYER_1_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(2,0);
  player_marker_set_clump(2,*(undefined4 *)(course_state + PLAYER_2_MARKER_CLUMP_OFFSET));
  player_marker_set_enabled(3,0);
  player_marker_set_clump(3,*(undefined4 *)(course_state + PLAYER_3_MARKER_CLUMP_OFFSET));
  return;
}


/* course_clumps_destroy */

/* "destroys the 5 clumps g_course_6a26cc.." */

extern void course_clumps_destroy(void);


/* course_sands_cb1 */

extern void course_sands_cb1(char *param_1,char *param_2);


/* course_sands_cb2 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_sands_cb2(char *param_1);


/* course_sands_cb5 */

extern void course_sands_cb5(char *param_1);


/* draw_textured_quad_uv */

/* "(tex, x, y, w, h, colour, uv[4]): fills g_im2d_vertices with the uv rectangle and draws the
   quad" */

extern void draw_textured_quad_uv (void *param_1,float param_2,float param_3,float param_4,float param_5,char *param_6, RwReal *param_7);


/* course_sands_cb0 */

extern void course_sands_cb0(char *param_1);


/* course_superg_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_superg_cb1(void);


/* course_superg_cb4 */

extern void course_superg_cb4(char *param_1);


/* course_training_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_training_cb1(char *param_1,char *param_2);


/* course_training_cb2 */

extern void course_training_cb2(char *param_1);


/* course_training_cb10 */

extern void course_training_cb10(char *param_1);



