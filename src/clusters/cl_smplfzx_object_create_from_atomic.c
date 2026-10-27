// cluster-entries: smplfzx_object_create_from_atomic
/* Installed cluster smplfzx_object_create_from_atomic (tools/cluster_install.py from output/cluster_pilot_j1/smplfzx_object_create_from_atomic.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00496ff0*/ extern undefined __cdecl atomic_world_bbox_update(int p0, struct RwBBox *p1);
/*00497450*/ extern int smplfzx_object_init_bounds();
/*00497590*/ extern int vec3_make_from_param();
/*00497600*/ extern int __cdecl pool_alloc_slot(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3, undefined4 p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8);
/*00497750*/ extern int pool_stack_pop_index();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004d0d40*/ extern RwBool RwFrameDestroy(struct RwFrame *p0);
/*004d16e0*/ extern struct RwFrame * RwFrameSetIdentity(struct RwFrame *p0);
/*004f6100*/ extern void _rpAtomicResyncInterpolatedSphere(struct RpAtomic *p0);
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*0071ab10*/ extern undefined g_course_create_71ab10;
/*0071ab14*/ extern undefined g_course_create_71ab14;
/*0071ab18*/ extern undefined g_course_create_71ab18;
/*0071ab1c*/ extern undefined g_course_create_71ab1c;
/*0071ab20*/ extern undefined g_course_create_71ab20;
/*0071abf0*/ extern undefined4 g_smplfzx_71abf0;
/*0071abf4*/ extern undefined4 g_particles_71abf4;
/* smplfzx_object_create_from_atomic */

/* "pops a pool slot, computes the world bbox of the atomic and initialises the smplfzx object" */

int __cdecl smplfzx_object_create_from_atomic(RpAtomic *atomic,undefined4 *offset_in,undefined4 *bbox_in,int style_index,undefined4 unused_5,
          undefined4 unused_6)

{
  void *parent_frame;
  RwFrame *orig_frame;
  undefined4 *xform;
  int *slot;
  RwFrame *identity_frame;
  int tmp;
  int mat_or_slot;
  RwBBox *bbox_ptr;
  RwReal sphere_radius;
  int old_word1;
  int stale_arg_matrix;
  int old_word0;
  struct {
    undefined4 offset_x;
    undefined4 offset_y;
    undefined4 offset_z;
  } offset;
  RwBBox world_bbox;
  
  /* Creates a smplfzx object in a pool slot for an atomic: slot[0..1] = handles from vec3_make_from_param/smplfzx_object_init_bounds, slot[2] = atomic, slot[3] = pool entry, slot[4] = style_index. offset_in = optional xyz offset, bbox_in = optional bbox (else the world bbox of the atomic with an identity frame); returns the pool slot or -1. */
  parent_frame = RpAtomic_object_object_parent(atomic);
  mat_or_slot = (int)parent_frame + 0x10;
  if ((RpAtomic_interpolator_flags(atomic) & 2) != 0) {
    _rpAtomicResyncInterpolatedSphere(atomic);
  }
  sphere_radius = RpAtomic_boundingSphere(atomic).radius;
  slot = (int *)pool_stack_pop_index(g_smplfzx_71abf0);
  if (slot == (int *)0x0) {
    return -1;
  }
  if (offset_in == (undefined4 *)0x0) {
    offset.offset_z = 0;
    offset.offset_y = 0;
    offset.offset_x = 0;
  }
  else {
    offset.offset_x = *offset_in;
    offset.offset_y = offset_in[1];
    offset.offset_z = offset_in[2];
  }
  if (bbox_in == (undefined4 *)0x0) {
    orig_frame = RpAtomic_object_object_parent(atomic);
    identity_frame = RwFrameCreate();
    RwFrameSetIdentity(identity_frame);
    RpAtomicSetFrame(atomic,identity_frame);
    atomic_world_bbox_update((int)atomic,&world_bbox);
    RpAtomicSetFrame(atomic,orig_frame);
    RwFrameDestroy(identity_frame);
  }
  else {
    world_bbox.inf.x = *(float *)&bbox_in[3];
    world_bbox.inf.y = *(float *)&bbox_in[4];
    world_bbox.inf.z = *(float *)&bbox_in[5];
    world_bbox.sup.x = *(float *)&bbox_in[0];
    world_bbox.sup.y = *(float *)&bbox_in[1];
    world_bbox.sup.z = *(float *)&bbox_in[2];
  }
  old_word0 = *slot;
  slot[6] = 0;
  slot[5] = 0;
  slot[7] = 0;
  slot[8] = 0;
  slot[2] = (int)atomic;
  stale_arg_matrix = mat_or_slot;
  tmp = vec3_make_from_param(atomic,mat_or_slot,old_word0);
  old_word1 = slot[1];
  *slot = tmp;
  bbox_ptr = &world_bbox;
  tmp = smplfzx_object_init_bounds(bbox_ptr,*(int *)&sphere_radius,mat_or_slot,old_word1);
  slot[1] = tmp;
  slot[0xb] = 0;
  slot[9] = 0;
  slot[10] = 0;
  slot[0xe] = 0;
  slot[0xc] = 0;
  slot[0xd] = 0;
  slot[0xf] = 1;
  tmp = *(int *)(*slot + 0x60);
  *(undefined4 *)(tmp + 0xc) = offset.offset_x;
  *(undefined4 *)(tmp + 0x10) = offset.offset_y;
  *(undefined4 *)(tmp + 0x14) = offset.offset_z;
  xform = *(undefined4 **)(*slot + 0x60);
  *xform = *(undefined4 *)((int)parent_frame + 0x40);
  xform[1] = *(undefined4 *)((int)parent_frame + 0x44);
  xform[2] = *(undefined4 *)((int)parent_frame + 0x48);
  tmp = style_index * 0x1c;
  *(undefined4 *)(*(int *)(*slot + 0x60) + 0x30) =
       *(undefined4 *)((char *)&g_course_create_71ab10 + tmp);
  *(undefined4 *)(*(int *)(*slot + 0x60) + 0x34) =
       *(undefined4 *)((char *)&g_course_create_71ab14 + tmp);
  *(undefined4 *)(*slot + 0x50) = *(undefined4 *)((char *)&g_course_create_71ab18 + tmp);
  *(undefined4 *)(*slot + 0x54) = *(undefined4 *)((char *)&g_course_create_71ab1c + tmp);
  *(undefined4 *)(*slot + 0x5c) = *(undefined4 *)((char *)&g_course_create_71ab20 + tmp);
  mat_or_slot = pool_alloc_slot(g_particles_71abf4,(undefined4)slot,(undefined4)bbox_ptr,*(int *)&sphere_radius,mat_or_slot,old_word1,(undefined4)atomic,stale_arg_matrix,old_word0);
  slot[4] = style_index;
  slot[3] = mat_or_slot;
  return mat_or_slot;
}

