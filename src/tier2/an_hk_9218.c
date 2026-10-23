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

undefined4 course_bbox_transform_and_test(RwBBox *boundBox,RwMatrix *param_2,RwReal *param_3,RwMatrix *matrix,RwV3d *param_5)

{
  RwBool RVar1;
  int iVar2;
  struct {
    RwBBox local_118;
    RwV3d local_100;
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
    RwReal local_a0;
    RwReal local_9c;
    RwReal local_98;
    undefined4 local_94;
    RwReal local_90;
    RwReal local_8c;
    RwReal local_88;
    undefined1 pad_84[4];
    RwReal local_80;
    RwReal local_7c;
    RwReal local_78;
    undefined1 pad_74[4];
    RwReal local_70;
    RwReal local_6c;
    RwReal local_68;
    undefined4 local_64;
    float local_60;
    float local_5c;
    float local_58;
    undefined1 pad_54[4];
    float local_50;
    float local_4c;
    float local_48;
    undefined1 pad_44[4];
    float local_40[16];   /* RwMatrix scratch (as raw floats: the original has no aligned frame) */
  } frame;
  
  RwMatrixInvert((RwMatrix *)frame.local_40,param_2);
  frame.local_118 = *(RwBBox *)param_3;
  frame.local_100.x = frame.local_118.inf.x;
  frame.local_f4 = frame.local_118.sup.x;
  frame.local_e8 = frame.local_118.sup.x;
  frame.local_100.y = frame.local_118.inf.y;
  frame.local_f0 = frame.local_118.inf.y;
  frame.local_dc = frame.local_118.inf.x;
  frame.local_d0 = frame.local_118.inf.x;
  frame.local_e4 = frame.local_118.sup.y;
  frame.local_d8 = frame.local_118.sup.y;
  frame.local_c4 = frame.local_118.sup.x;
  frame.local_b8 = frame.local_118.sup.x;
  frame.local_cc = frame.local_118.inf.y;
  frame.local_c0 = frame.local_118.inf.y;
  frame.local_ac = frame.local_118.inf.x;
  frame.local_100.z = frame.local_118.inf.z;
  frame.local_ec = frame.local_118.inf.z;
  frame.local_e0 = frame.local_118.inf.z;
  frame.local_d4 = frame.local_118.inf.z;
  frame.local_b4 = frame.local_118.sup.y;
  frame.local_a8 = frame.local_118.sup.y;
  frame.local_c8 = frame.local_118.sup.z;
  frame.local_bc = frame.local_118.sup.z;
  frame.local_b0 = frame.local_118.sup.z;
  frame.local_a4 = frame.local_118.sup.z;
  RwV3dTransformPoints(&frame.local_100,&frame.local_100,8,matrix);
  RwV3dTransformPoints(&frame.local_100,&frame.local_100,8,(RwMatrix *)frame.local_40);
  RwBBoxCalculate(&frame.local_118,&frame.local_100,8);
  frame.local_90 = boundBox->inf.x;
  frame.local_8c = boundBox->inf.y;
  frame.local_88 = boundBox->inf.z;
  frame.local_a0 = boundBox->sup.x;
  frame.local_94 = 0;
  frame.local_9c = boundBox->sup.y;
  frame.local_98 = boundBox->sup.z;
  frame.local_64 = 0;
  frame.local_70 = frame.local_118.inf.x;
  frame.local_80 = frame.local_118.sup.x;
  frame.local_6c = frame.local_118.inf.y;
  frame.local_68 = frame.local_118.inf.z;
  frame.local_7c = frame.local_118.sup.y;
  frame.local_78 = frame.local_118.sup.z;
  iVar2 = FUN_0056f080(&frame.local_a0);
  if (iVar2 != 0) {
    iVar2 = FUN_0056f080(&frame.local_80);
    if (iVar2 != 0) {
      FUN_0056f3a0(&frame.local_60,&frame.local_a0,&frame.local_80);
      iVar2 = FUN_0056f080(&frame.local_60);
      if (iVar2 != 0) {
        param_5->x = (frame.local_50 + frame.local_60) * k_f_0_5;
        param_5->y = (frame.local_4c + frame.local_5c) * k_f_0_5;
        param_5->z = (frame.local_48 + frame.local_58) * k_f_0_5;
        RVar1 = RwBBoxContainsPoint(boundBox,param_5);
        if (RVar1 != 0) {
          RwV3dTransformPoint(param_5,param_5,param_2);
          return 1;
        }
      }
    }
  }
  return 0;
}


