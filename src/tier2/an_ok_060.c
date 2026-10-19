#include "mfl_tier2.h"
/*0044c550*/ extern int camera_get_8c7470();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*00487ee0*/ extern undefined __cdecl triangle_normal_cross(struct RwV3d *p0, float *p1, float *p2, float *p3);
/*00487f80*/ extern undefined __cdecl vector_normalize_copy(struct RwV3d *p0, float *p1, RwReal *p2);
/*0048ce30*/ extern int lua_init();
/*0048ce50*/ extern int lua_state_close();
/*0048cea0*/ extern int __cdecl lua_run_piz_script(undefined4 p0, undefined4 p1);
/*00490400*/ extern undefined __cdecl rwp_world_step_pipeline(undefined4 p0, undefined4 p1);
/*00495a20*/ extern int __cdecl rwp_raster_pair_create(undefined4 *p0, RwInt32 p1, RwInt32 p2, uint p3);
/*00495af0*/ extern undefined __cdecl rwp_camera_destroy_frame(void *p0, struct RwTexture *p1);
/*00496620*/ extern int __cdecl trigger_sphere_register_in_grid(undefined4 p0);
/*00496940*/ extern int triggers_update();
/*00496ad0*/ extern int triggers_sort_axis();
/*00496ff0*/ extern undefined __cdecl atomic_world_bbox_update(int p0, struct RwBBox *p1);
/*00497450*/ extern int smplfzx_object_init_bounds();
/*00497590*/ extern int vec3_make_from_param();
/*00497600*/ extern int __cdecl pool_alloc_slot(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*00497750*/ extern int pool_stack_pop_index();
/*00498f40*/ extern int fx_ring_add_point();
/*0049a960*/ extern int RpPTankAtomicLock_00545d10();
/*0049a970*/ extern int RpPTankAtomicUnlock_00545f20();
/*004a5d70*/ extern int object_render();
/*004a5da0*/ extern int object_destroy_frames();
/*004a5ed0*/ extern undefined __cdecl video_raster_create(undefined4 p0);
/*004a61a0*/ extern undefined __cdecl video_present_frame(undefined4 p0, undefined4 p1);
/*004b2b48*/ extern int _ftol();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0);
/*004c7150*/ extern int lua_gettop();
/*004c7220*/ extern int lua_tonumber();
/*004c73e0*/ extern int lua_setglobal();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004d1cd0*/ extern enum RwFrustumTestResult RwCameraFrustumTestSphere(struct RwCamera *p0, struct RwSphere *p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d77e0*/ extern RwBool RwRasterDestroy(struct RwRaster *p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f68f0*/ extern struct RpAtomic * RpAtomicCreate();
/*004f69e0*/ extern struct RpAtomic * RpAtomicSetGeometry(struct RpAtomic *p0, struct RpGeometry *p1, RwUInt32 p2);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*004f8140*/ extern struct RpMaterial * RpMaterialCreate();
/*004f81d0*/ extern RwBool RpMaterialDestroy(struct RpMaterial *p0);
/*004f8800*/ extern struct RpMorphTarget * RpMorphTargetCalcBoundingSphere(struct RpMorphTarget *p0, struct RwSphere *p1);
/*004f8b00*/ extern struct RpGeometry * RpGeometryTriangleSetMaterial(struct RpGeometry *p0, struct RpTriangle *p1, struct RpMaterial *p2);
/*004f8bd0*/ extern struct RpGeometry * RpGeometryUnlock(struct RpGeometry *p0);
/*004f8e30*/ extern struct RpGeometry * RpGeometryCreate(RwInt32 p0, RwInt32 p1, RwUInt32 p2);
/*004f9060*/ extern RwBool RpGeometryDestroy(struct RpGeometry *p0);
/*005d3087*/ extern double _hypot();
/*005dd32c*/ extern float k_f_3;
/*005dd330*/ extern float float_1_0;
/*005dd33c*/ extern float k_f_4;
/*005dd340*/ extern float k_f_0_5;
/*005dd414*/ extern float k_f_4_5;
/*005dd41c*/ extern float float_5_0;
/*005dda20*/ extern float k_f_4_2949673e9;
/*005ddc28*/ extern float k_f_0;
/*005df448*/ extern float k_f_18;
/*005e06e0*/ extern float k_f_2_25_5e06e0;
/*005e06e4*/ extern float k_f_0_93;
/*006f2aec*/ extern struct CourseCounts *g_course_counts;
/*006f2b00*/ extern undefined4 g_lua_state;
/*006f2d88*/ extern undefined4 g_course_collision_6f2d88;
/*006f3588*/ extern undefined4 g_course_collision_6f3588;
/*006f35c8*/ extern undefined4 g_course_6f35c8;
/*006f4b48*/ extern undefined4 g_course_6f4b48;
/*006f60c8*/ extern undefined g_course_6f60c8;
/*006f68c8*/ extern undefined4 g_course_6f68c8;
/*006f6908*/ extern undefined4 g_course_6f6908;
/*006f690c*/ extern undefined4 g_course_6f690c;
/*006f6910*/ extern undefined4 g_data_6f6910;
/*006f6918*/ extern undefined4 g_data_6f6918;
/*006f6920*/ extern undefined4 g_data_6f6920;
/*006f6928*/ extern undefined4 g_data_6f6928;
/*006f6930*/ extern undefined4 g_rw_6f6930;
/*006f6934*/ extern undefined4 g_data_6f6934;
/*006f693c*/ extern undefined4 g_ptank_atomic;
/*006f6944*/ extern undefined4 g_ptank_lock_6f6944;
/*006fa7d0*/ extern undefined4 g_rw_6fa7d0;
/*006fa7d8*/ extern undefined4 g_rw_6fa7d8;
/*006fcdd8*/ extern undefined4 g_rw_6fcdd8;
/*006fd418*/ extern undefined4 g_physics_6fd418;
/*00701c94*/ extern int g_rw_701c94;
/*00701c98*/ extern int g_physics_701c98;
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
/*00771038*/ extern undefined4 g_app_771038;
/*007da6bc*/ extern undefined4 g_ptank_plugin_offset;
/*0088a844*/ extern undefined4 g_rw_88a844;
/* lua_ClumpFade */

/* Lua function ClumpFade (registered by the script engine) */

extern undefined4 lua_ClumpFade(char *param_1);


/* lua_register_function */

/* "(function, name): lua_register on the global state: the helper that every registered Lua
   function of the game goes through (found by lua_api.py)" */

extern void lua_register_function(char *param_1,char *param_2);


/* lua_run_script_file */

/* "lua_init + register callback + run + close" */

extern undefined4 lua_run_script_file(char *param_1,char *param_2,code *param_3);


/* race_render_viewport_forward_4a61a0 */

/* "forwards to race_render_viewport_sub_4a61a0" */

extern void __cdecl race_render_viewport_forward_4a61a0(char *param_1,char *param_2,char *param_3);


/* video_raster_create_forward */

/* "calls video_raster_create" */

extern void __cdecl video_raster_create_forward(char *param_1,char *param_2);


/* raster_destroy_771038 */

/* "destroys g_app_771038 raster" */

extern undefined4 raster_destroy_771038(void);


/* course_edge_side_test */

/* "normalised direction to the point, dot with two edge normals, returns crossing flag" */

extern undefined4 __cdecl course_edge_side_test(char *param_1,char *param_2,float *param_3,uint *param_4);


/* course_collision_object_push_vectors */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "for each face of the object near the point accumulates push direction and writes the face
   normals" */

extern void __cdecl course_collision_object_push_vectors(RwV3d *out,float *param_2,RwV3d *param_3,int *param_4);


/* course_collision_tables_clear */

/* "clears 0x560 dwords at 0x6F35C8" */

extern void course_collision_tables_clear(void);


/* course_collision_objects_cull */

/* "frustum tests every collision object and updates visible ones" */

extern void course_collision_objects_cull(RwCamera *camera);


/* ptank_lock_all */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "RpPTankAtomicLock for the three property sets" */

extern void ptank_lock_all(char *param_1);


/* rwpcar_ring_add_points_for_contact */

/* "for contact types 0x10/0x11/0x19 adds three points to the fx ring and marks the object flag" */

extern void rwpcar_ring_add_points_for_contact(char *param_1,char *param_2);


/* rwpcar_get_rw_6fa7d0 */

/* "return g_rw_6fa7d0;" */

extern undefined4 rwpcar_get_rw_6fa7d0(void);


/* rwp_world_step_forward */

/* "calls rwp_world_step_pipeline" */

extern undefined4 __cdecl rwp_world_step_forward(char *param_1,char *param_2);


/* rwp_atomics_destroy */

/* "destroys the g_rw_701c94 atomics at g_rw_6fa7d8" */

extern void rwp_atomics_destroy(void);


/* rwp_atomics_render */

/* "renders the g_rw_701c94 atomics at g_rw_6fa7d8" */

extern void rwp_atomics_render(void);


/* hsv_phase_to_rgb */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(out, value, range): 6 segment phase to colour" */

extern void hsv_phase_to_rgb(float *param_1,char *param_2,char *param_3);


/* vec2_normalise_to_unit_square */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "out = (v / (2*length)) + 0.5 for the delta of two 2D points" */

extern void vec2_normalise_to_unit_square(float *param_1,float *param_2,float *param_3);


/* rwp_collision_mesh_to_geometry */

/* "converts the Rwp collision mesh (strips/vertex tables) to an RpGeometry with normals and uv" */

extern RpAtomic * rwp_collision_mesh_to_geometry(char *param_1,char *param_2);

#define FLOAT_1_0_BITS (0x3f800000)
#define FLOAT_0_5_BITS (0x3f000000)


/* rwp_object_set_contact_params */

/* "for each contact of the body writes a value, 1.0, 1.0, 0.5" */

/* Sets the contact parameters of the body attached to object (+0x18).
   object: owner whose body is updated; value: dword copied into +4 of every contact (inferred).
   Every contact gets +0xC = 1.0, +0x10 = 1.0, +0x14 = 0.5 (inferred meaning).
   Returns object. */

/* TIER2: validated 2026-10-18 authority=diffemu */
int rwp_object_set_contact_params(int object,undefined4 *value)
{
  char *body;
  char *contact;
  int contact_index;

  body = *(char **)(object + 0x18);
  contact_index = 0;
  /* walk the body's contact array (+0x20, count +0x24) */
  if (0 < *(int *)(body + 0x24)) {
    do {
      contact = *(char **)(*(char **)(body + 0x20) + contact_index * 4);
      *(undefined4 *)(contact + 4) = *value;
      *(undefined4 *)(contact + 0xc) = FLOAT_1_0_BITS;
      *(undefined4 *)(contact + 0x10) = FLOAT_1_0_BITS;
      *(undefined4 *)(contact + 0x14) = FLOAT_0_5_BITS;
      contact_index = contact_index + 1;
    } while (contact_index < *(int *)(body + 0x24));
  }
  return object;
}


#define ARENA_SLOT_SIZE (0x24)
#define ARENA_SLOT_DWORDS (9)
#define ARENA_SLOT_LINK_OFFSET (0x1c)
#define ARENA_EXTRA_SLOTS (2)
#define ARENA_DWORD_COUNT_MASK (0x3fffffff)


/* rwp_world_clear_arena */

/* "clears the working arena and rebuilds the per-body pointers" */

/* Clears the working arena of world and rebuilds its slot ring (inferred from the loops).
   world: owner whose arena is reset. Its arena buffer is at +0x14, the head slot at +0x18,
   the cursor at +0x1c, the tail at +0x20, and the slot count at +0x16c.
   Zeroes (count + ARENA_EXTRA_SLOTS) slots, links count slots through their +0x1c field
   (ARENA_SLOT_LINK_OFFSET) into a ring closed on the head, then clears +0x168, +0x170, +0x194
   and sets +0x198 to 1 (meaning inferred). Returns nothing. */

/* TIER2: unvalidated 2026-10-17 authority=diffemu */
extern void *__cdecl memset(void *, int, unsigned int);
#pragma intrinsic(memset)
void rwp_world_clear_arena(char *world)
{
  int slot;

  *(undefined4 **)(world + 0x18) = *(undefined4 **)(world + 0x14);
  memset(*(void **)(world + 0x14), 0, (*(int *)(world + 0x16c) + ARENA_EXTRA_SLOTS) * ARENA_SLOT_DWORDS * 4);
  slot = *(int *)(world + 0x18);
  *(int *)(world + 0x1c) = slot;
  *(int *)(slot + ARENA_SLOT_LINK_OFFSET) = slot + ARENA_SLOT_SIZE;
  slot = 0;
  if (0 < *(int *)(world + 0x16c)) {
    do {
      *(int *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET) = *(int *)(world + 0x1c) + ARENA_SLOT_SIZE;
      *(undefined4 *)(world + 0x1c) = *(undefined4 *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET);
      slot = slot + 1;
    } while (slot < *(int *)(world + 0x16c));
  }
  *(int *)(world + 0x20) = *(int *)(world + 0x1c);
  *(undefined4 *)(*(char **)(world + 0x1c) + ARENA_SLOT_LINK_OFFSET) = *(undefined4 *)(world + 0x18);
  *(undefined4 *)(world + 0x168) = 0;
  *(undefined4 *)(world + 0x170) = 0;
  *(undefined4 *)(world + 0x194) = 0;
  *(undefined4 *)(world + 0x198) = 1;
  *(undefined4 *)(world + 0x1c) = *(undefined4 *)(world + 0x18);
  return;
}


/* physics_buffer_size */

/* "(float count) * 0x24 + 0x19C" */

extern int physics_buffer_size(void);


/* hermite_interpolate */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "cubic Hermite spline between the two middle points, tangents from the neighbours, parameter t" */

/* Interpolates a 3D point on the segment start_point -> end_point (inferred: Catmull-Rom, a cubic
   Hermite spline whose tangents come from the two neighbouring points).
   t: parameter in [0, 1]; t = 0 gives start_point, t = 1 gives end_point.
   out: receives x, y, z (3 floats). prev_point and next_point only shape the tangents.
   Returns nothing. Per coordinate (inferred, P0..P3 = prev, start, end, next):
   out = 0.5 * (2*P1 + (P2 - P0)*t + (2*P0 - 5*P1 + 4*P2 - P3)*t^2 + (-P0 + 3*P1 - 3*P2 + P3)*t^3) */

void hermite_interpolate(float t,float *out,float *prev_point,float *start_point,float *end_point,
                        float *next_point)
{
  float t_squared;
  float t_cubed;

  t_squared = t * t;
  t_cubed = t * t_squared;
  /* the same formula for x, y and z: out[0] first, then out[1] and out[2] */
  *out = (*start_point + *start_point +
             (*end_point - *prev_point) * t +
             (((*start_point * k_f_3 - *prev_point) - *end_point * k_f_3) + *next_point) * t_cubed +
             ((*end_point * k_f_4 + ((*prev_point + *prev_point) - *start_point * float_5_0)) - *next_point) *
             t_squared) * k_f_0_5;
  out[1] = (start_point[1] + start_point[1] +
               (end_point[1] - prev_point[1]) * t +
               (((start_point[1] * k_f_3 - prev_point[1]) - end_point[1] * k_f_3) + next_point[1]) * t_cubed +
               ((end_point[1] * k_f_4 + ((prev_point[1] + prev_point[1]) - start_point[1] * float_5_0)) -
               next_point[1]) * t_squared) * k_f_0_5;
  out[2] = (start_point[2] + start_point[2] +
               (end_point[2] - prev_point[2]) * t +
               (((start_point[2] * k_f_3 - prev_point[2]) - end_point[2] * k_f_3) + next_point[2]) * t_cubed +
               ((end_point[2] * k_f_4 + ((prev_point[2] + prev_point[2]) - start_point[2] * float_5_0)) -
               next_point[2]) * t_squared) * k_f_0_5;
  return;
}


/* physics_buffer_rewind */

/* "+0x1C = +0x18" */

extern void __cdecl physics_buffer_rewind(char *param_1);


/* replay_record_save_frame */

/* "copies a 0x67 dword record and its array into the record list" */

extern undefined4 replay_record_save_frame(int *param_1,undefined4 *param_2);


/* viewport_raster_create */

/* "(index 0..3, w, h, flags): creates the raster pair of a viewport" */

extern bool viewport_raster_create(char *param_1,RwInt32 width,RwInt32 height,char *param_4);

#define VIEWPORT_LAST_INDEX (3)


/* viewport_raster_destroy */

/* "(index 0..3): destroys the raster pair" */

/* Destroys the raster pair of one viewport (viewport_index: 0..VIEWPORT_LAST_INDEX).
   The frame lives in the g_physics_702244 slot and the texture in the g_physics_702254 slot,
   both 4-byte entries per viewport. Returns 1 if the pair was destroyed, 0 otherwise. */

/* TIER2: unvalidated 2026-10-16 authority=diffemu */
undefined4 viewport_raster_destroy(uint viewport_index)
{
  RwTexture *texture;

  if (viewport_index >= VIEWPORT_LAST_INDEX + 1) {
    return 0;
  }
  if (*(void **)((char *)&g_physics_702244 + viewport_index * 4) == (void *)0x0) {
    return 0;
  }
  texture = *(RwTexture **)((char *)&g_physics_702254 + viewport_index * 4);
  if (texture == (RwTexture *)0x0) {
    return 0;
  }
  rwp_camera_destroy_frame(*(void **)((char *)&g_physics_702244 + viewport_index * 4),texture);
  *(undefined4 *)((char *)&g_physics_702244 + viewport_index * 4) = 0;
  *(undefined4 *)((char *)&g_physics_702254 + viewport_index * 4) = 0;
  return 1;
}


/* trigger_grid_build_visible_cells */

/* "places a 5x5 grid of cells around the camera, keeps the cells whose direction is within 0.93 dot
   of the view" */

extern void trigger_grid_build_visible_cells(void);

#define TRIGGER_STRIDE_FLOATS (0x23)
#define TRIGGER_TAIL_DWORDS (9)
#define TRIGGER_POOL_END (((int)&g_trigger_count))


/* triggers_assign_ids */

/* "stores the index of every trigger in its +0x20 field" */

/* Resets the whole g_triggers pool (walk bounded by TRIGGER_POOL_END): stores each trigger's
   index in Trigger.index (+0x20), zeroes position_radius[0..3], type, owner and callback, and
   clears the tail of the struct (+0x68 .. end). Then zeroes g_particles_71ab00 and
   g_trigger_count. Takes no parameters, returns nothing. */

/* TIER2: validated 2026-10-18 authority=diffemu */
void triggers_assign_ids(void)
{
  int tail_remaining;
  float *trigger_cursor;
  ushort trigger_index;
  undefined4 *tail_ptr;

  trigger_index = 0;
  trigger_cursor = g_triggers[0].position_radius + 1;
  /* one pass per trigger slot: reset the fields, then step to the next slot */
  do {
    /* trigger_cursor points at position_radius[1] of the current trigger */
    ((Trigger *)(trigger_cursor + -1))->index = trigger_index;
    ((Trigger *)(trigger_cursor + -1))->position_radius[0] = 0.0;
    ((Trigger *)(trigger_cursor + -1))->position_radius[1] = 0.0;
    ((Trigger *)(trigger_cursor + -1))->position_radius[2] = 0.0;
    ((Trigger *)(trigger_cursor + -1))->position_radius[3] = 0.0;
    ((Trigger *)(trigger_cursor + -1))->type = 0;
    ((Trigger *)(trigger_cursor + -1))->owner = 0;
    ((Trigger *)(trigger_cursor + -1))->callback = 0;
    tail_ptr = trigger_cursor + 0x19;
    trigger_cursor = trigger_cursor + TRIGGER_STRIDE_FLOATS;
    trigger_index = trigger_index + 1;
    /* clear the TRIGGER_TAIL_DWORDS dwords from +0x68 to the end of the struct */
    for (tail_remaining = TRIGGER_TAIL_DWORDS; tail_remaining != 0; tail_remaining = tail_remaining + -1) {
      *tail_ptr = 0;
      tail_ptr = tail_ptr + 1;
    }
  } while ((LONG_PTR)trigger_cursor < TRIGGER_POOL_END);
  g_particles_71ab00 = 0;
  g_trigger_count = 0;
  return;
}


/* trigger_sphere_register */

/* "registers a collision trigger {x,y,z,radius,type,owner,0,callback} into the pool of 300 Trigger
   structs (g_triggers, 0x8C bytes each) and into the 11 x 11 broad phase grid; returns 0 when full"
    */

extern undefined4 trigger_sphere_register(float *param_1);


/* triggers_update_dispatch */

/* "calls triggers_sort_axis or the other path depending on g_trigger_71aaf0" */

extern void triggers_update_dispatch(void);


/* particles_lookup_by_key */

/* "walks the list at g_particles_71abf4+4 for the key at +8 and returns +0xC" */

extern undefined4 particles_lookup_by_key(char *param_1);


/* smplfzx_object_create_from_atomic */

/* "pops a pool slot, computes the world bbox of the atomic and initialises the smplfzx object" */

extern int __cdecl smplfzx_object_create_from_atomic (RpAtomic *atomic,undefined4 *param_2,undefined4 *param_3,char *param_4,char *param_5, char *param_6);



