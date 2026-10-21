#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*0048e7e0*/ extern int rwp_object_set_rotation_degrees(int obj, int a, float x, float y, float z);
/*00494230*/ extern int rwp_object_sample_and_lock();
/*00494290*/ extern int rwp_object_set_contact_params();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056bf10*/ extern float FUN_0056bf10(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056c080*/ extern void FUN_0056c080(undefined4 p0, undefined4 p1);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cdf0*/ extern void FUN_0056cdf0(undefined4 p0, undefined4 p1);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, undefined4 p1);
/*0058c990*/ extern int FUN_0058c990(undefined4 p0);
/*0058cb90*/ extern int FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(undefined4 p0);
/*005dda20*/ extern float k_f_4_2949673e9;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_cars_setup_bodies_from_bounds */
#define COURSE_HIGHWAYN (12)
#define COURSE_HIGHWAY (38)

/* "creates the car physics shape from player_car[0].bounds_points and copies the centre offset to
   all cars" */

void __cdecl rwp_cars_setup_bodies_from_bounds(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  float fVar2;
  undefined4 *puVar3;
  int iVar4;
  undefined4 uVar5;
  RwFrame *frame;
  CourseId course_id;
  int fVar6;
  volatile float hz;
  float10 hx;
  float10 hy;
  int iVar7;
  struct {
    undefined1 local_20;
    undefined1 local_1f;
    undefined1 local_1e;
    undefined1 local_1d;
    float local_1c;
    float local_18;
    float local_14;
    float local_10;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame_obj;
  
  frame_obj.local_20 = 0x40;
  frame_obj.local_1f = 0x40;
  frame_obj.local_1e = 0x60;
  frame_obj.local_1d = 0xff;
  frame_obj.local_18 = 0.200000003f;
  frame_obj.local_14 = 0.0649999976f;
  frame_obj.local_10 = 0.400000006f;
  puVar3 = (undefined4 *)FUN_0058cb90();
  puVar3[0xd] = 0x3f800000;
  puVar3[0xe] = 0x40200000;
  frame_obj.local_c = 0;
  puVar3[1] = 0;
  frame_obj.local_8 = 0xbdcccccd;
  puVar3[2] = 0xbdcccccd;
  frame_obj.local_4 = 0;
  puVar3[3] = 0;
  hx = (player_car[0].bounds_points[0] - player_car[0].bounds_points[3]) * 0.5f;
  hy = (player_car[0].bounds_points[0xd] - player_car[0].bounds_points[1]) * 0.5f;
  hz = (player_car[0].bounds_points[2] - player_car[0].bounds_points[8]) * 0.5f;
  frame_obj.local_18 = hx * 0.95f;
  frame_obj.local_14 = hy * 0.95f;
  frame_obj.local_10 = hz * 0.95f;
  player_car[0].rwp_body_offset[0] =
       (-player_car[0].bounds_points[3] - player_car[0].bounds_points[0]) * 0.5f;
  player_car[0].rwp_body_offset[1] = -(player_car[0].bounds_points[1] + frame_obj.local_14);
  player_car[0].rwp_body_offset[2] =
       (-player_car[0].bounds_points[8] - player_car[0].bounds_points[2]) * 0.5f;
  uVar1 = *puVar3;
  player_car[1].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[1].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[1].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  player_car[2].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[2].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[2].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  player_car[3].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[3].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[3].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  frame_obj.local_1c = player_car[0].rwp_body_offset[2];
  FUN_0056cdf0(uVar1,(undefined4)&frame_obj.local_18);
  FUN_0056cac0(uVar1,0x3ecccccd);
  FUN_0056ca70(uVar1,0x3f000000);
  FUN_0056c080(uVar1,0x3c23d70a);
  FUN_0056bf10((undefined4)puVar3,0x3fc00000,1);
  fVar6 = 0;
  do {
    iVar4 = FUN_0058c990((undefined4)puVar3);
    uVar5 = rwp_object_sample_and_lock(uVar1,1);
    (&g_rw_6fd498)[fVar6] = uVar5;
    rwp_object_set_contact_params(uVar5,&frame_obj.local_20);
    frame = RwFrameCreate();
    RpAtomicSetFrame((RpAtomic *)(&g_rw_6fd498)[fVar6],frame);
    FUN_0058c8b0((&g_rw_6fd498)[fVar6],iVar4);
    fVar2 = (float)fVar6;
    if (fVar6 < 0) {
      fVar2 = fVar2 + k_f_4_2949673e9;
    }
    frame_obj.local_1c = fVar6;
    rwp_object_set_rotation_degrees(fVar6,0,0.5f,-10.0f,fVar2 * 1.25f);
    *(uint *)(iVar4 + 8) = *(uint *)(iVar4 + 8) & 0xfffffff3;
    uVar5 = rw_first_field((int *)param_1);
    FUN_0056a210(uVar5, iVar4, 0xffffffff, 6);
    course_id = current_course_id();
    if (course_id == COURSE_HIGHWAY) {
LAB_0048ebbc:
      uVar5 = *(undefined4 *)(iVar4 + 0x24);
      iVar7 = fVar6 + 2;
    }
    else {
      course_id = current_course_id();
      if (course_id == COURSE_HIGHWAYN) goto LAB_0048ebbc;
      uVar5 = *(undefined4 *)(iVar4 + 0x24);
      iVar7 = 1;
    }
    FUN_0056b440(uVar5,iVar4,iVar7);
    fVar6 = (fVar6 + 1);
    if (3 < (uint)fVar6) {
      FUN_0058cbe0((undefined4)puVar3);
      return;
    }
  } while( true );
}


