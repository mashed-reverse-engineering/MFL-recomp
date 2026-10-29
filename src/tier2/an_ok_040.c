#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*0040fdb0*/ extern int get_player_count();
/*0040fdc0*/ extern int race_get_phase();
/*00449700*/ extern undefined __cdecl course_sky_atomic_create(struct RwTexture *p0, int p1, undefined4 p2);
/*00456b20*/ extern int course_rotating_object_update();
/*00457010*/ extern undefined __cdecl spline_object_register_trigger_sphere(undefined4 p0);
/*004574a0*/ extern int course_spline_objects_update();
/*004577c0*/ extern int course_tierra_n_rotating_object_update();
/*004577e0*/ extern int course_objects_render();
/*004584c0*/ extern int course_race_phase_hook();
/*00459e50*/ extern int course_objects_reset_physics();
/*0045b510*/ extern int course_marker_positions_init();
/*00475730*/ extern int get_viewport_camera();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047a470*/ extern int car_set_wheel_contact();
/*0047b360*/ extern int car_wheel_get_field_88_a();
/*00481fd0*/ extern double random_range();
/*00482270*/ extern undefined __stdcall spline_get_length(struct RpSpline *p0);
/*0048e470*/ extern int rwp_object_sync_and_reset();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*00492d10*/ extern int rwp_object_set_active();
/*0049a980*/ extern int fx_slot_release();
/*0049a9b0*/ extern int fx_slot_find_free();
/*0049c5f0*/ extern undefined __cdecl fx_snow_ptank_update(int p0, struct RwCamera *p1, undefined1 *p2, float p3, float p4);
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004b4ba7*/ extern int entry();
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d1640*/ extern struct RwFrame * RwFrameRotate(struct RwFrame *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004d1a50*/ extern RwBool __cdecl RwBBoxContainsPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d4870*/ extern RwMatrix * RwMatrixTranslate(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*004dc920*/ extern void * RwIm3DTransform(RwIm3DVertex *p0, RwUInt32 p1, RwMatrix *p2, RwUInt32 p3);
/*004dc9f0*/ extern RwBool RwIm3DEnd();
/*004dcb80*/ extern RwBool RwIm3DRenderPrimitive(enum RwPrimitiveType p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f67c0*/ extern struct RpClump * RpClumpRender(struct RpClump *p0);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*0050b5e0*/ extern int nop_stub();
/*00556360*/ extern RwReal RpSplineFindMatrix(struct RpSpline *p0, RwInt32 p1, RwReal p2, struct RwV3d *p3, RwMatrix *p4);
/*00557860*/ extern int __cdecl RtIntersectionLineTriangle(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5);
/*005686d0*/ extern undefined __cdecl RpLODAtomicHookRender(undefined4 p0);
/*005dd330*/ extern float float_1_0;
/*005dd374*/ extern float k_f_0_2;
/*005dd61c*/ extern float k_f_0_001;
/*005dd9c8*/ extern float k_f_0_16666667;
/*005ddb10*/ extern float k_f_0_99;
/*005e64a4*/ extern float k_f_0;
/*005de6a8*/ extern float k_f_n0_5;
/*005df5d8*/ extern float k_f_146;
/*005df5dc*/ extern float k_f_n122;
/*0060d8fc*/ extern undefined4 g_course_spline_60d8fc;
/*006a2164*/ extern undefined4 g_course_rouabout_6a2164;
/*006a2168*/ extern undefined4 g_course_6a2168;
/*006a2188*/ extern undefined4 g_course_6a2188;
/*006a218c*/ extern undefined4 g_course_rouabout_6a218c;
/*006a2190*/ extern undefined4 g_course_rouabout_6a2190;
/*006a2194*/ extern undefined4 g_course_rouabout_6a2194;
/*006a21a8*/ extern undefined4 g_course_rouabout_6a21a8;
/*006a23c8*/ extern undefined4 g_course_rouabout_6a23c8;
/*006a23cc*/ extern float g_course_rouabout_6a23cc;
/*006a23e0*/ extern undefined g_course_6a23e0;
/*006a2614*/ extern float g_course_6a2614;
/*006a262c*/ extern undefined4 g_course_rouabout_6a262c;
/*006a2630*/ extern undefined4 g_course_rouabout_6a2630;
/*006a2634*/ extern undefined4 g_course_6a2634;
/*006a2638*/ extern undefined4 g_course_sandsnow_6a2638;
/*006a263c*/ extern int g_course_storm_6a263c;
/*006a2640*/ extern undefined4 g_course_storm_6a2640;
/*006a2654*/ extern int g_course_tierra_6a2654;
/*006a2658*/ extern undefined4 g_course_tierra_6a2658;
/*006a265c*/ extern undefined4 g_course_tierra_6a265c;
/*006a2660*/ extern undefined4 g_course_tierra_6a2660;
/*006a2664*/ extern undefined4 g_course_tierra_6a2664;
/*006a26b8*/ extern undefined4 g_course_set_6a26b8;
/*006a26cc*/ extern undefined4 g_course_clumps_6a26cc;
/*006a26d0*/ extern undefined4 g_course_clumps_6a26d0;
/*006a26d4*/ extern undefined4 g_course_clumps_6a26d4;
/*006a272c*/ extern undefined4 g_course_clumps_6a272c;
/*006a2730*/ extern undefined4 g_course_clumps_6a2730;
/*006a2734*/ extern undefined4 g_course_clumps_6a2734;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;
/*007ef7f4*/ extern float g_tick_seconds;
/*008bf140*/ extern undefined4 g_course_8bf140;
/*008bf150*/ extern undefined4 g_course_8bf150;
/*008bf154*/ extern undefined4 g_course_8bf154;
/*008bf16c*/ extern undefined4 g_course_object_8bf16c;
/*008c3f64*/ extern undefined4 g_course_8c3f64;
/*008c3fa0*/ extern undefined4 g_course_8c3fa0;
/*008c3fc0*/ extern undefined4 g_course_8c3fc0;
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
/*008c53e0*/ extern undefined4 g_course_8c53e0;
/*008c5500*/ extern undefined g_course_spline_8c5500;
/*008d8b30*/ extern undefined4 player_count;
/* course_rotating_object_integrate */

/* "rotates the frame about x/z by the stored angles and integrates damped angular velocities" */

extern void course_rotating_object_integrate(char *param_1,float param_2);


/* course_quad_render_a */

/* "transforms 4 points and draws a textured Im3D triangle strip with the object's texture" */

extern void course_quad_render_a(char *param_1);


/* course_quad_render_b */

/* "same quad draw for the second quad set (+0xE8 points, texture at +0x118)" */

extern void __cdecl course_quad_render_b(char *param_1,char *param_2,char *param_3);


/* course_get_matrix_pos_8c53e0 */

/* "frame position (+0x40) of the object at g_course_8c53e0" */

extern int course_get_matrix_pos_8c53e0(void);


/* spline_object_update */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RpSplineFindMatrix of the spline object and sets its frame transform" */

extern void spline_object_update(undefined4 *param_1,float param_2);


/* course_rouabout_cb7 */

extern void course_rouabout_cb7(void);


/* course_rouabout_cb1 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

extern void course_rouabout_cb1(char *param_1,char *param_2);


/* course_point_in_water_box_test */

/* "RwBBoxContainsPoint on g_course_6a2614 then scans the sphere list" */

extern undefined4 __cdecl course_point_in_water_box_test(RwV3d *vertex);


/* course_6a2634_clear */

/* "g_course_6a2634 = 0" */

extern void course_6a2634_clear(void);


/* course_rouabout_cb5 */

extern void course_rouabout_cb5(void);


/* course_sandsnow_destroy_frames */

/* "object_destroy_frames(g_course_sandsnow_6a2638)" */

extern void course_sandsnow_destroy_frames(void);


/* course_sandsnow_cb0 */

extern void course_sandsnow_cb0(char *param_1);


/* course_race_phase_hook_b */

/* "duplicate of course_race_phase_hook" */

extern void course_race_phase_hook_b(void);


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


/* course_forest_cb2 */

extern void course_forest_cb2(char *param_1);


/* course_c26_stub_return_0 */

/* "returns without work" */

extern undefined4 course_c26_stub_return_0(void);


/* course_object_get_float_16c */

/* "(index): float of the object record" */

extern double course_object_get_float_16c(char *param_1);


/* course_c26_checkpoint_scale_value */

/* "a float table entry * 1/6" */

extern double course_c26_checkpoint_scale_value(char *param_1);


/* course_object_offsets_init */

/* "fills the table of 80 float offsets (10, 35, 60 ... 235 then pairs) used to place the course
   objects" */

extern void course_object_offsets_init(void);
extern char g_course_highway_6a26a0[];

#define TRIGGER_TYPE_CAR_HIT (0x15)
#define CAR_ID_RANGE_LO_EXCL (0xfeff)
#define CAR_ID_RANGE_HI_EXCL (0xff08)
#define HIT_BITMASK_BASE (((char *)&g_course_highway_6a26a0 + 0x4))


/* trigger_cb_mark_hit_bit */

/* "type 0x15 trigger vs a car id (0xFF00..0xFF07): sets the bit of the object index in the bitmask
   at 0x6A26A4" */

/* Marks a trigger as hit by a car: if trigger is a TRIGGER_TYPE_CAR_HIT trigger and car's id (+0x10)
   is strictly inside the car id range (0xFF00..0xFF07), sets the bit for trigger's object index
   (trigger->+0x14->+0x9c, inferred) in the hit bitmask at HIT_BITMASK_BASE.
   trigger: the trigger object (inferred). car: the car object (inferred). Returns nothing. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void trigger_cb_mark_hit_bit(char *trigger,char *car)
{
  byte *hit_byte;
  int object_index;

  /* Only car-hit triggers, and only for a car id inside the car id range */
  if (((*(int *)(trigger + 0x10) == TRIGGER_TYPE_CAR_HIT) && (CAR_ID_RANGE_LO_EXCL < *(int *)(car + 0x10))) &&
     (*(int *)(car + 0x10) < CAR_ID_RANGE_HI_EXCL)) {
    object_index = *(int *)(*(char **)(trigger + 0x14) + 0x9c);
    hit_byte = (byte *)((object_index >> 3) + HIT_BITMASK_BASE);
    *hit_byte = *hit_byte | '\x01' << ((byte)object_index & 7);
  }
  return;
}


/* course_highway_cb6 */

/* WARNING: Removing unreachable block (ram,0x004590b5) */

extern void course_highway_cb6(void);


/* course_object_offsets_init_b */

/* "duplicate of course_object_offsets_init" */

extern void course_object_offsets_init_b(void);


/* course_clumps_destroy */

/* "destroys the 5 clumps g_course_6a26cc.." */

extern void course_clumps_destroy(void);


/* course_sands_cb6 */

extern void course_sands_cb6(void);



