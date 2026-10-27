#include "mfl_tier2.h"
/*00405800*/ extern float bbox_point_penetration(float *camera, float *param_3);
/*00405ae0*/ extern float __cdecl aabb_max_axis_distance_to_point(void *p0, void *p1, void *p2, void *p3, void *p4, void *p5, undefined4 p6, void *p7, undefined4 p8, undefined4 p9, void *p10, void *p11);
/*004817d0*/ extern int atomic_plugin_set_flags();
/*004817f0*/ extern float atomic_plugin_set_float_24();
/*004849b0*/ extern undefined __cdecl atomic_set_material_alpha_value(undefined4 p0, undefined4 p1);
/*004b2b48*/ extern int _ftol();
/*004d0ff0*/ extern RwMatrix * RwFrameGetLTM(struct RwFrame *p0);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0089de0c*/ extern undefined4 g_gfx_89de0c;
/* clump_for_all_atomics_with_camera_matrix_cb_405bb0 */

/* function pointer pushed/stored by clump_for_all_atomics_with_camera_matrix (PUSH 0x405bb0) */

/* (inferred names) per atomic: the camera position (camera+0x30) is moved into the atomic space with the inverse LTM, its penetration into the bounds box of the atomic (bounds_record+8) sets the material alpha (penetration * 127, clamped 0..255) and its max axis distance is stored as the atomic plugin float at +0x24; extra_arg_* / *_ptr2 / bbox_arg2 are register pass-through arguments the original pushes for the callees */
int clump_for_all_atomics_with_camera_matrix_cb_405bb0(int atomic,char *camera)

{
  float fbits1;
  float penetration;
  int bounds_record;
  RwMatrix *atomic_ltm;
  uint alpha_value;
  RwV3d *cam_pos_ptr;
  float *bbox;
  double axis_dist;
  float *bbox_arg2;
  RwV3d *cam_local_ptr2;
  RwV3d *cam_local_ptr;
  undefined4 extra_arg_a;
  RwMatrix *inv_ltm_ptr2;
  int extra_arg_c;
  undefined4 extra_arg_b;
  RwMatrix *inv_ltm_ptr;
  RwV3d cam_local_pos;
  RwMatrix inv_ltm;
  
  atomic_ltm = RwFrameGetLTM(*(RwFrame **)(atomic + 4));
  inv_ltm_ptr = &inv_ltm;
  bounds_record = g_gfx_89de0c + atomic;
  RwMatrixInvert(inv_ltm_ptr,atomic_ltm);
  extra_arg_b = 1;
  extra_arg_c = atomic;
  atomic_plugin_set_flags(atomic, 0x1);
  inv_ltm_ptr2 = &inv_ltm;
  extra_arg_a = 1;
  cam_pos_ptr = (RwV3d *)(camera + 0x30);
  cam_local_ptr = &cam_local_pos;
  RwV3dTransformPoints(cam_local_ptr,cam_pos_ptr,1,inv_ltm_ptr2);
  cam_local_ptr2 = &cam_local_pos;
  bbox = (float *)(bounds_record + 8);
  bbox_arg2 = bbox;
  penetration = bbox_point_penetration(bbox,&cam_local_ptr2->x);
  axis_dist = (double)aabb_max_axis_distance_to_point
                             (bbox,&cam_local_pos,bbox_arg2,cam_local_ptr2,cam_local_ptr,cam_pos_ptr,extra_arg_a,inv_ltm_ptr2,extra_arg_c
                              ,extra_arg_b,inv_ltm_ptr,atomic_ltm);
  atomic_plugin_set_float_24(atomic,(fbits1 = (float)axis_dist, *(int *)&fbits1));
  alpha_value = ((int)(((penetration * 127.0f) < 0.0f) ? 0.0f : (!((penetration * 127.0f) < 0.0f) && !((penetration * 127.0f) > 255.0f) ? (penetration * 127.0f) : 255.0f)));
  atomic_set_material_alpha_value(atomic,alpha_value & 0xff);
  return atomic;
}


