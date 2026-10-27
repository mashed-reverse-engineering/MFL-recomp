#include "mfl_tier2.h"
/*004d18d0*/ extern struct RwBBox * RwBBoxCalculate(struct RwBBox *p0, struct RwV3d *p1, RwInt32 p2);
/*004d1a50*/ extern RwBool __cdecl RwBBoxContainsPoint(struct RwBBox *p0, struct RwV3d *p1);
/*004d4490*/ extern RwMatrix * RwMatrixInvert(RwMatrix *p0, RwMatrix *p1);
/*004d5640*/ extern struct RwV3d * RwV3dTransformPoint(struct RwV3d *p0, struct RwV3d *p1, RwMatrix *p2);
/*004d5670*/ extern struct RwV3d * RwV3dTransformPoints(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056f080*/ extern int FUN_0056f080(void *p0);
/*0056f3a0*/ extern void FUN_0056f3a0(void *p0, void *p1, void *p2);
/*005dd340*/ extern float k_f_0_5;
/* course_bbox_transform_and_test */

/* "transforms the 8 corners of a box with two matrices, recomputes the AABB and compares to
   another" */

undefined4 course_bbox_transform_and_test(RwBBox *boundBox,RwMatrix *matrix_b,RwReal *box_in,RwMatrix *matrix_a,RwV3d *out_center)

{
  RwBool inside;
  int ok;
  struct {
    RwBBox corner_box;
    RwV3d corners;   /* point 0 of 8; points 1..7 follow as c1_x..c7_z (24 floats, filled with the box corners) */
    RwReal c1_x;
    RwReal c1_y;
    RwReal c1_z;
    RwReal c2_x;
    RwReal c2_y;
    RwReal c2_z;
    RwReal c3_x;
    RwReal c3_y;
    RwReal c3_z;
    RwReal c4_x;
    RwReal c4_y;
    RwReal c4_z;
    RwReal c5_x;
    RwReal c5_y;
    RwReal c5_z;
    RwReal c6_x;
    RwReal c6_y;
    RwReal c6_z;
    RwReal c7_x;
    RwReal c7_y;
    RwReal c7_z;
    /* three 8-float boxes {sup xyz,w, inf xyz,w} (inferred layout of the FUN_0056f080 / FUN_0056f3a0 argument) */
    RwReal other_sup_x;
    RwReal other_sup_y;
    RwReal other_sup_z;
    undefined4 other_sup_w;
    RwReal other_inf_x;
    RwReal other_inf_y;
    RwReal other_inf_z;
    undefined1 other_inf_w[4];
    RwReal moved_sup_x;
    RwReal moved_sup_y;
    RwReal moved_sup_z;
    undefined1 moved_sup_w[4];
    RwReal moved_inf_x;
    RwReal moved_inf_y;
    RwReal moved_inf_z;
    undefined4 moved_inf_w;
    float overlap_sup_x;
    float overlap_sup_y;
    float overlap_sup_z;
    undefined1 overlap_sup_w[4];
    float overlap_inf_x;
    float overlap_inf_y;
    float overlap_inf_z;
    undefined1 overlap_inf_w[4];
    float inv_matrix_b[16];   /* RwMatrix scratch (as raw floats: the original has no aligned frame) */
  } frame;
  
  /* 8 corners of box_in -> matrix_a -> inverse(matrix_b), then their AABB */
  RwMatrixInvert((RwMatrix *)frame.inv_matrix_b,matrix_b);
  frame.corner_box = *(RwBBox *)box_in;
  frame.corners.x = frame.corner_box.inf.x;
  frame.c1_x = frame.corner_box.sup.x;
  frame.c2_x = frame.corner_box.sup.x;
  frame.corners.y = frame.corner_box.inf.y;
  frame.c1_y = frame.corner_box.inf.y;
  frame.c3_x = frame.corner_box.inf.x;
  frame.c4_x = frame.corner_box.inf.x;
  frame.c2_y = frame.corner_box.sup.y;
  frame.c3_y = frame.corner_box.sup.y;
  frame.c5_x = frame.corner_box.sup.x;
  frame.c6_x = frame.corner_box.sup.x;
  frame.c4_y = frame.corner_box.inf.y;
  frame.c5_y = frame.corner_box.inf.y;
  frame.c7_x = frame.corner_box.inf.x;
  frame.corners.z = frame.corner_box.inf.z;
  frame.c1_z = frame.corner_box.inf.z;
  frame.c2_z = frame.corner_box.inf.z;
  frame.c3_z = frame.corner_box.inf.z;
  frame.c6_y = frame.corner_box.sup.y;
  frame.c7_y = frame.corner_box.sup.y;
  frame.c4_z = frame.corner_box.sup.z;
  frame.c5_z = frame.corner_box.sup.z;
  frame.c6_z = frame.corner_box.sup.z;
  frame.c7_z = frame.corner_box.sup.z;
  RwV3dTransformPoints(&frame.corners,&frame.corners,8,matrix_a);
  RwV3dTransformPoints(&frame.corners,&frame.corners,8,(RwMatrix *)frame.inv_matrix_b);
  RwBBoxCalculate(&frame.corner_box,&frame.corners,8);
  /* other box (boundBox) vs the transformed AABB: both must be valid and the overlap centre must lie inside boundBox */
  frame.other_inf_x = boundBox->inf.x;
  frame.other_inf_y = boundBox->inf.y;
  frame.other_inf_z = boundBox->inf.z;
  frame.other_sup_x = boundBox->sup.x;
  frame.other_sup_w = 0;
  frame.other_sup_y = boundBox->sup.y;
  frame.other_sup_z = boundBox->sup.z;
  frame.moved_inf_w = 0;
  frame.moved_inf_x = frame.corner_box.inf.x;
  frame.moved_sup_x = frame.corner_box.sup.x;
  frame.moved_inf_y = frame.corner_box.inf.y;
  frame.moved_inf_z = frame.corner_box.inf.z;
  frame.moved_sup_y = frame.corner_box.sup.y;
  frame.moved_sup_z = frame.corner_box.sup.z;
  ok = FUN_0056f080(&frame.other_sup_x);
  if (ok != 0) {
    ok = FUN_0056f080(&frame.moved_sup_x);
    if (ok != 0) {
      FUN_0056f3a0(&frame.overlap_sup_x,&frame.other_sup_x,&frame.moved_sup_x);
      ok = FUN_0056f080(&frame.overlap_sup_x);
      if (ok != 0) {
        out_center->x = (frame.overlap_inf_x + frame.overlap_sup_x) * k_f_0_5;
        out_center->y = (frame.overlap_inf_y + frame.overlap_sup_y) * k_f_0_5;
        out_center->z = (frame.overlap_inf_z + frame.overlap_sup_z) * k_f_0_5;
        inside = RwBBoxContainsPoint(boundBox,out_center);
        if (inside != 0) {
          RwV3dTransformPoint(out_center,out_center,matrix_b);
          return 1;
        }
      }
    }
  }
  return 0;
}


