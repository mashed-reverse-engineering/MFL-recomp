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
    RwV3d corners;
    RwReal local_f4;
    RwReal local_f0;
    RwReal local_ec;
    RwReal local_e8;
    RwReal local_e4;
    RwReal local_e0;
    RwReal local_dc;
    RwReal local_d8;
    RwReal local_d4;
    RwReal local_d0;
    RwReal local_cc;
    RwReal local_c8;
    RwReal local_c4;
    RwReal local_c0;
    RwReal local_bc;
    RwReal local_b8;
    RwReal local_b4;
    RwReal local_b0;
    RwReal local_ac;
    RwReal local_a8;
    RwReal local_a4;
    RwReal other_box;
    RwReal local_9c;
    RwReal local_98;
    undefined4 local_94;
    RwReal local_90;
    RwReal local_8c;
    RwReal local_88;
    undefined1 pad_84[4];
    RwReal moved_box;
    RwReal local_7c;
    RwReal local_78;
    undefined1 pad_74[4];
    RwReal local_70;
    RwReal local_6c;
    RwReal local_68;
    undefined4 local_64;
    float overlap_box;
    float local_5c;
    float local_58;
    undefined1 pad_54[4];
    float local_50;
    float local_4c;
    float local_48;
    undefined1 pad_44[4];
    float inv_matrix_b[16];   /* RwMatrix scratch (as raw floats: the original has no aligned frame) */
  } frame;
  
  /* 8 corners of box_in -> matrix_a -> inverse(matrix_b), then their AABB */
  RwMatrixInvert((RwMatrix *)frame.inv_matrix_b,matrix_b);
  frame.corner_box = *(RwBBox *)box_in;
  frame.corners.x = frame.corner_box.inf.x;
  frame.local_f4 = frame.corner_box.sup.x;
  frame.local_e8 = frame.corner_box.sup.x;
  frame.corners.y = frame.corner_box.inf.y;
  frame.local_f0 = frame.corner_box.inf.y;
  frame.local_dc = frame.corner_box.inf.x;
  frame.local_d0 = frame.corner_box.inf.x;
  frame.local_e4 = frame.corner_box.sup.y;
  frame.local_d8 = frame.corner_box.sup.y;
  frame.local_c4 = frame.corner_box.sup.x;
  frame.local_b8 = frame.corner_box.sup.x;
  frame.local_cc = frame.corner_box.inf.y;
  frame.local_c0 = frame.corner_box.inf.y;
  frame.local_ac = frame.corner_box.inf.x;
  frame.corners.z = frame.corner_box.inf.z;
  frame.local_ec = frame.corner_box.inf.z;
  frame.local_e0 = frame.corner_box.inf.z;
  frame.local_d4 = frame.corner_box.inf.z;
  frame.local_b4 = frame.corner_box.sup.y;
  frame.local_a8 = frame.corner_box.sup.y;
  frame.local_c8 = frame.corner_box.sup.z;
  frame.local_bc = frame.corner_box.sup.z;
  frame.local_b0 = frame.corner_box.sup.z;
  frame.local_a4 = frame.corner_box.sup.z;
  RwV3dTransformPoints(&frame.corners,&frame.corners,8,matrix_a);
  RwV3dTransformPoints(&frame.corners,&frame.corners,8,(RwMatrix *)frame.inv_matrix_b);
  RwBBoxCalculate(&frame.corner_box,&frame.corners,8);
  /* other box (boundBox) vs the transformed AABB: both must be valid and the overlap centre must lie inside boundBox */
  frame.local_90 = boundBox->inf.x;
  frame.local_8c = boundBox->inf.y;
  frame.local_88 = boundBox->inf.z;
  frame.other_box = boundBox->sup.x;
  frame.local_94 = 0;
  frame.local_9c = boundBox->sup.y;
  frame.local_98 = boundBox->sup.z;
  frame.local_64 = 0;
  frame.local_70 = frame.corner_box.inf.x;
  frame.moved_box = frame.corner_box.sup.x;
  frame.local_6c = frame.corner_box.inf.y;
  frame.local_68 = frame.corner_box.inf.z;
  frame.local_7c = frame.corner_box.sup.y;
  frame.local_78 = frame.corner_box.sup.z;
  ok = FUN_0056f080(&frame.other_box);
  if (ok != 0) {
    ok = FUN_0056f080(&frame.moved_box);
    if (ok != 0) {
      FUN_0056f3a0(&frame.overlap_box,&frame.other_box,&frame.moved_box);
      ok = FUN_0056f080(&frame.overlap_box);
      if (ok != 0) {
        out_center->x = (frame.local_50 + frame.overlap_box) * k_f_0_5;
        out_center->y = (frame.local_4c + frame.local_5c) * k_f_0_5;
        out_center->z = (frame.local_48 + frame.local_58) * k_f_0_5;
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


