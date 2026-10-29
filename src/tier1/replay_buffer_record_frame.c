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
/*005e64a4*/ extern float k_f_0;
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
#define REC_POS_X_OFF (0x10)
#define REC_POS_Y_OFF (0x14)
#define REC_POS_Z_OFF (0x18)
#define REC_NEXT_OFF (0x1c)
#define REC_VALUE_OFF (0x20)
#define REC_SIZE (0x24)
#define REC_WORDS (9)
#define BUF_CURRENT_OFF (0x1c)
#define BUF_RECORD_COUNT_OFF (0x168)
#define BUF_LAST_VALUE_OFF (0x174)
#define BUF_FIRST_VALUE_OFF (0x178)
#define BUF_STARTED_OFF (0x194)
/* replay_buffer_record_frame */
/* "(buffer, matrix, value, index): stores position, quaternion and value of one replay record" */
/*
 * Stores one replay sample (position from matrix, orientation as quaternion, and value) in the
 * buffer's current record and moves the buffer's cursor on to the next record (inferred).
 * The sample is written only if sample_index is below the threshold at +0x170 or nothing was
 * recorded yet (inferred). +0x170 is incremented on every call (inferred).
 */
void replay_buffer_record_frame(char *buffer,RwMatrix *matrix,undefined4 value,int sample_index)
{
  char *next_record;
  undefined4 *next_record_words;
  void *new_record;
  char *scratch; /* loop counter, then reused as the current record pointer */
  undefined4 *cursor; /* walks the new record while zeroing, later the current record */

  /* Record the sample when it is due, or when nothing has been recorded yet */
  if ((*(int *)(buffer + 0x170) > sample_index) || (*(int *)(buffer + BUF_STARTED_OFF) == 0)) {
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_X_OFF) = (matrix->pos).x;
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_Y_OFF) = (matrix->pos).y;
    *(RwReal *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_POS_Z_OFF) = (matrix->pos).z;
    RtQuatConvertFromMatrix(*(RtQuat **)(buffer + BUF_CURRENT_OFF),matrix);
    *(undefined4 *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_VALUE_OFF) = value;
    /* First use of this record: allocate the next record and zero its REC_WORDS words */
    if (*(int *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF) == 0) {
      new_record = RwMalloc(REC_SIZE,0);
      *(void **)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF) = new_record;
      cursor = *(undefined4 **)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF);
      for (scratch = REC_WORDS; scratch != 0; scratch = scratch + -1) {
        *cursor = 0;
        cursor = cursor + 1;
      }
    }
    /* Copy the position of the current record into the next record */
    scratch = *(char **)(buffer + BUF_CURRENT_OFF);
    next_record = *(char **)(scratch + REC_NEXT_OFF);
    *(RwV3d *)(next_record + REC_POS_X_OFF) = *(RwV3d *)(scratch + REC_POS_X_OFF);
    /* Copy the orientation (quaternion) of the current record into the next record, then advance the cursor to it */
    cursor = *(undefined4 **)(buffer + BUF_CURRENT_OFF);
    next_record_words = (undefined4 *)cursor[7];
    *(RtQuat *)next_record_words = *(RtQuat *)cursor;
    *(undefined4 *)(buffer + BUF_CURRENT_OFF) = *(undefined4 *)(*(char **)(buffer + BUF_CURRENT_OFF) + REC_NEXT_OFF);
    *(undefined4 *)(buffer + BUF_LAST_VALUE_OFF) = value;
    *(int *)(buffer + BUF_RECORD_COUNT_OFF) = *(int *)(buffer + BUF_RECORD_COUNT_OFF) + 1;
    *(undefined4 *)(buffer + 0x170) = 1;
    if (*(int *)(buffer + BUF_STARTED_OFF) == 0) {
      *(undefined4 *)(buffer + BUF_FIRST_VALUE_OFF) = value;
    }
    *(undefined4 *)(buffer + BUF_STARTED_OFF) = 1;
  }
  *(int *)(buffer + 0x170) = *(int *)(buffer + 0x170) + 1;
  return;
}
