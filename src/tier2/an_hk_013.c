#include "mfl_tier2.h"
/*0044c550*/ extern int camera_get_8c7470();
/*0045f5a0*/ extern int explosion_queue();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0048e650*/ extern int rwp_object_set_in_world();
/*00492d10*/ extern int rwp_object_set_active();
/*00495a20*/ extern int __cdecl rwp_raster_pair_create(undefined4 *p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00496620*/ extern int __cdecl trigger_sphere_register_in_grid(undefined4 p0);
/*00496940*/ extern int triggers_update();
/*00496ad0*/ extern int triggers_sort_axis();
/*00496ea0*/ extern int __cdecl smplfzx_get_object(undefined4 p0);
/*00496ee0*/ extern int smplfzx_current_object_get_field(undefined4 p0);
/*00496ff0*/ extern undefined __cdecl atomic_world_bbox_update(int p0, struct RwBBox *p1);
/*00497070*/ extern int smplfzx_object_bounds_update();
/*00497590*/ extern int vec3_make_from_param();
/*00497600*/ extern int __cdecl pool_alloc_slot(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*00497750*/ extern int pool_stack_pop_index();
/*0049c9a0*/ extern int fx_object_break_into_particles();
/*004a8420*/ extern int debug_printf();
/*004b2b48*/ extern int _ftol();
/*004b322b*/ extern int exit();
/*004c5b10*/ extern void FUN_004c5b10(undefined4 p0, undefined4 p1, undefined4 p2);
/*004c5c40*/ extern void FUN_004c5c40(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1860*/ extern void _rwObjectHasFrameSetFrame(void *p0, struct RwFrame *p1);
/*004d1e80*/ extern void FUN_004d1e80(undefined4 p0);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d59f0*/ extern struct RwTexture * RwTextureSetRaster(struct RwTexture *p0, struct RwRaster *p1);
/*004d5bf0*/ extern RwBool RwTextureDestroy(struct RwTexture *p0);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f68f0*/ extern struct RpAtomic * RpAtomicCreate();
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);
/*004f9060*/ extern RwBool RpGeometryDestroy(struct RpGeometry *p0);
/*00557140*/ extern RwBool RtQuatConvertFromMatrix(struct RtQuat *p0, RwMatrix *p1);
/*0056a710*/ extern void FUN_0056a710(undefined4 p0, undefined4 p1);
/*0056e8c0*/ extern int FUN_0056e8c0(undefined4 p0);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005d3087*/ extern double _hypot();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd338*/ extern float k_f_0_01;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd378*/ extern float k_f_0_1;
/*005dd414*/ extern float k_f_4_5;
/*005dd41c*/ extern float float_5_0;
/*005dd628*/ extern float k_f_0_25;
/*005dd9c4*/ extern float k_f_0_15;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005ddb60*/ extern float k_f_n8;
/*005ddc1c*/ extern float k_f_0_33333334;
/*005ddc28*/ extern float k_f_0;
/*005de25c*/ extern float k_f_0_005;
/*005df448*/ extern float k_f_18;
/*005e06e0*/ extern float k_f_2_25_5e06e0;
/*005e06e4*/ extern float k_f_0_93;
/*0062ac74*/ extern float g_car_62ac74;
/*006fa190*/ extern undefined g_rw_6fa190;
/*006fa4b0*/ extern undefined4 g_rw_6fa4b0;
/*006fab78*/ extern undefined4 g_rw_6fab78;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
/*00702244*/ extern undefined g_physics_702244;
/*00702254*/ extern undefined g_physics_702254;
/*00703890*/ extern undefined4 g_trigger_sphere_703890;
/*007106d8*/ extern undefined4 g_trigger_sphere_7106d8;
/*007106e0*/ extern struct Trigger g_triggers[300];
/*0071aaf0*/ extern undefined4 g_trigger_71aaf0;
/*0071aaf4*/ extern undefined4 g_trigger_count;
/*0071aafc*/ extern undefined4 g_particles_71aafc;
/*0071ab00*/ extern undefined4 g_particles_71ab00;
/*0071ab10*/ extern undefined g_course_create_71ab10;
/*0071ab14*/ extern undefined g_course_create_71ab14;
/*0071ab18*/ extern undefined g_course_create_71ab18;
/*0071ab1c*/ extern undefined g_course_create_71ab1c;
/*0071ab20*/ extern undefined g_course_create_71ab20;
/*0071abf0*/ extern undefined4 g_smplfzx_71abf0;
/*0071abf4*/ extern undefined4 g_particles_71abf4;
/*0071c1e0*/ extern undefined g_smplfzx_get_71c1e0;
/*0072db20*/ extern undefined4 g_particles_get_72db20;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define POOL_HEADER_SIZE (12)
#define POOL_SLOT_SIZE (4)
#define POOL_CAPACITY_MASK (0xffff)

/* pool_create */

/* Allocates a pool header and a slot array of capacity entries (capacity is masked to 16 bits).
 * capacity: number of slots wanted. Returns the new pool, or NULL if either allocation fails.
 * Pool header layout (inferred): [0] = used count, set to 0; [1] = capacity; [2] = slots pointer.
 * The header is then passed to pool_clear before it is returned.
 */

/* "(capacity): allocates a 0xC byte pool header" */

/* TIER2: validated 2026-10-19 authority=shadow */
undefined4 * __cdecl pool_create(uint capacity)
{
  undefined4 *pool;
  void *slots;

  pool = RwMalloc(POOL_HEADER_SIZE,0);
  if (pool == (undefined4 *)0x0) {
    return (undefined4 *)0x0;
  }
  slots = RwMalloc((capacity & POOL_CAPACITY_MASK) * POOL_SLOT_SIZE,0);
  pool[2] = slots;
  if (slots == (void *)0x0) {
    return (undefined4 *)0x0;
  }
  pool[1] = capacity & POOL_CAPACITY_MASK;
  *pool = 0;
  pool_clear(pool);
  return pool;
}
