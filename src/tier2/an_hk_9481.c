#include "mfl_tier2.h"
typedef int CourseId;
/*0042b110*/ extern int current_course_id();
/*0048e7e0*/ extern int rwp_object_set_rotation_degrees(int obj, int a, float x, float y, float z);
/*00494230*/ extern int rwp_object_sample_and_lock();
/*00494290*/ extern int rwp_object_set_contact_params();
/*004d0c50*/ extern struct RwFrame * RwFrameCreate();
/*004f7f70*/ extern struct RpAtomic * RpAtomicSetFrame(struct RpAtomic *p0, struct RwFrame *p1);
/*005543d0*/ extern int rw_first_field(int * p0);
/*0056a210*/ extern void FUN_0056a210(undefined4 p0, void *p1, undefined4 p2, undefined4 p3);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, void *p1, undefined4 p2);
/*0056bf10*/ extern float FUN_0056bf10(void *p0, undefined4 p1, undefined4 p2);
/*0056c080*/ extern void FUN_0056c080(undefined4 p0, undefined4 p1);
/*0056ca70*/ extern void FUN_0056ca70(undefined4 p0, undefined4 p1);
/*0056cac0*/ extern void FUN_0056cac0(undefined4 p0, undefined4 p1);
/*0056cdf0*/ extern void FUN_0056cdf0(undefined4 p0, void *p1);
/*0058c8b0*/ extern void FUN_0058c8b0(undefined4 p0, void *p1);
/*0058c990*/ extern char *FUN_0058c990(void *p0);
/*0058cb90*/ extern char *FUN_0058cb90(void);
/*0058cbe0*/ extern void FUN_0058cbe0(void *p0);
/*005dda20*/ extern float k_f_4_2949673e9;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_cars_setup_bodies_from_bounds */
#define COURSE_HIGHWAYN (12)
#define COURSE_HIGHWAY (38)

/* "creates the car physics shape from player_car[0].bounds_points and copies the centre offset to
   all cars" */

void __cdecl rwp_cars_setup_bodies_from_bounds(char *world,undefined4 param_2)

{
  undefined4 shape;
  float index_f;
  undefined4 *body_def;
  char *body;
  undefined4 tmp_w;
  RwFrame *frame;
  CourseId course_id;
  int car_index;
  volatile float hz;
  double hx;
  double hy;
  int shape_index;
  struct {
    undefined1 contact_r;
    undefined1 contact_g;
    undefined1 contact_b;
    undefined1 contact_a;
    float box_tmp;
    float box_x;
    float box_y;
    float box_z;
    undefined4 local_c;
    undefined4 local_8;
    undefined4 local_4;
  } frame_obj;
  
  frame_obj.contact_r = 0x40;
  frame_obj.contact_g = 0x40;
  frame_obj.contact_b = 0x60;
  frame_obj.contact_a = 0xff;
  frame_obj.box_x = 0.200000003f;
  frame_obj.box_y = 0.0649999976f;
  frame_obj.box_z = 0.400000006f;
  body_def = (undefined4 *)FUN_0058cb90();
  body_def[0xd] = 0x3f800000;
  body_def[0xe] = 0x40200000;
  frame_obj.local_c = 0;
  body_def[1] = 0;
  frame_obj.local_8 = 0xbdcccccd;
  body_def[2] = 0xbdcccccd;
  frame_obj.local_4 = 0;
  body_def[3] = 0;
  hx = (player_car[0].bounds_points[0] - player_car[0].bounds_points[3]) * 0.5f;
  hy = (player_car[0].bounds_points[0xd] - player_car[0].bounds_points[1]) * 0.5f;
  hz = (player_car[0].bounds_points[2] - player_car[0].bounds_points[8]) * 0.5f;
  frame_obj.box_x = hx * 0.95f;
  frame_obj.box_y = hy * 0.95f;
  frame_obj.box_z = hz * 0.95f;
  player_car[0].rwp_body_offset[0] =
       (-player_car[0].bounds_points[3] - player_car[0].bounds_points[0]) * 0.5f;
  player_car[0].rwp_body_offset[1] = -(player_car[0].bounds_points[1] + frame_obj.box_y);
  player_car[0].rwp_body_offset[2] =
       (-player_car[0].bounds_points[8] - player_car[0].bounds_points[2]) * 0.5f;
  shape = *body_def;
  player_car[1].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[1].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[1].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  player_car[2].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[2].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[2].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  player_car[3].rwp_body_offset[0] = player_car[0].rwp_body_offset[0];
  player_car[3].rwp_body_offset[1] = player_car[0].rwp_body_offset[1];
  player_car[3].rwp_body_offset[2] = player_car[0].rwp_body_offset[2];
  frame_obj.box_tmp = player_car[0].rwp_body_offset[2];
  FUN_0056cdf0(shape,&frame_obj.box_x);
  FUN_0056cac0(shape,0x3ecccccd);
  FUN_0056ca70(shape,0x3f000000);
  FUN_0056c080(shape,0x3c23d70a);
  FUN_0056bf10(body_def,0x3fc00000,1);
  car_index = 0;
  do {
    body = FUN_0058c990(body_def);
    tmp_w = rwp_object_sample_and_lock(shape,1);
    (&g_rw_6fd498)[car_index] = tmp_w;
    rwp_object_set_contact_params(tmp_w,&frame_obj.contact_r);
    frame = RwFrameCreate();
    RpAtomicSetFrame((RpAtomic *)(&g_rw_6fd498)[car_index],frame);
    FUN_0058c8b0((&g_rw_6fd498)[car_index],body);
    index_f = (float)car_index;
    if (car_index < 0) {
      index_f = index_f + k_f_4_2949673e9;
    }
    frame_obj.box_tmp = car_index;
    rwp_object_set_rotation_degrees(car_index,0,0.5f,-10.0f,index_f * 1.25f);
    *(uint *)(body + 8) = *(uint *)(body + 8) & 0xfffffff3;
    tmp_w = rw_first_field((int *)world);
    FUN_0056a210(tmp_w, body, 0xffffffff, 6);
    course_id = current_course_id();
    if (course_id == COURSE_HIGHWAY) {
LAB_0048ebbc:
      tmp_w = *(undefined4 *)(body + 0x24);
      shape_index = car_index + 2;
    }
    else {
      course_id = current_course_id();
      if (course_id == COURSE_HIGHWAYN) goto LAB_0048ebbc;
      tmp_w = *(undefined4 *)(body + 0x24);
      shape_index = 1;
    }
    FUN_0056b440(tmp_w,body,shape_index);
    car_index = (car_index + 1);
    if (3 < (uint)car_index) {
      FUN_0058cbe0(body_def);
      return;
    }
  } while( true );
}


