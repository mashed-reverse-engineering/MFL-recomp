#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*00490c60*/ extern int __cdecl physics_body_get_velocity(void *p0, undefined4 p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056bc20*/ extern void FUN_0056bc20(void *p0, void *p1, void *p2);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005dd330*/ extern float float_1_0;
/*005dd340*/ extern float k_f_0_5;
/*005dd62c*/ extern float k_f_100;
/*005e04d8*/ extern float k_f_n6_5;
/*005e04dc*/ extern float k_f_n0_22222222;
/*005e04e0*/ extern float k_f_0_22222222;
/*005e04e4*/ extern float k_f_n0_11111111;
/*005e04e8*/ extern float k_f_0_11111111;
/*005e04ec*/ extern float k_f_n6_6666666en4;
/*0062ac68*/ extern float g_car_62ac68;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];
/* rwp_car_apply_forces */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "applies the physics object forces from the car matrix" */

void rwp_car_apply_forces(int car_or_matrix,undefined4 force_kind)

{
  int *body_ptr;
  int car_index;
  int owner_or_hit;
  float *dir_ptr;
  float speed;
  struct {
    RwV3d offset;
    float at_x;
    float at_y;
    float at_z;
    float force_x;
    float force_y;
    float force_z;
  } frame;
  
  car_index = car_or_matrix;
  car_current_matrix(&car_or_matrix,car_or_matrix);
  owner_or_hit = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
  body_ptr = *(int **)(*(char **)(owner_or_hit + 0x10) + 4);
  if (body_ptr == (int *)0x0) {
    return;
  }
  switch(force_kind) {
  case 0:
    frame.at_x = *(float *)(car_or_matrix + 0x30);
    frame.at_y = *(float *)(car_or_matrix + 0x34);
    frame.at_z = *(float *)(car_or_matrix + 0x38);
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.force_y = 350.0;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    break;
  case 1:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.force_x = 0.0;
    frame.force_y = 350.0;
    frame.force_z = 0.0;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    RwV3dTransformVectors(&frame.offset,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[car_index].matrices)
    ;
    frame.offset.x = frame.offset.x * k_f_0_11111111;
    frame.offset.y = frame.offset.y * k_f_0_11111111;
    frame.offset.z = frame.offset.z * k_f_0_11111111;
    frame.at_x = frame.offset.x + frame.at_x;
    frame.at_y = frame.offset.y + frame.at_y;
    frame.at_z = frame.offset.z + frame.at_z;
    frame.force_x = 0.0;
    frame.force_y = 150.0;
    frame.force_z = 0.0;
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    return;
  case 2:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.force_x = 0.0;
    frame.force_y = 350.0;
    frame.force_z = 0.0;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    RwV3dTransformVectors(&frame.offset,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[car_index].matrices)
    ;
    frame.offset.x = frame.offset.x * k_f_n0_11111111;
    frame.offset.y = frame.offset.y * k_f_n0_11111111;
    frame.offset.z = frame.offset.z * k_f_n0_11111111;
    frame.at_x = frame.offset.x + frame.at_x;
    frame.at_y = frame.offset.y + frame.at_y;
    frame.at_z = frame.offset.z + frame.at_z;
    frame.force_x = 0.0;
    frame.force_y = 150.0;
    frame.force_z = 0.0;
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    return;
  case 3:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    RwV3dTransformVectors(&frame.offset,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[car_index].matrices)
    ;
    frame.offset.x = frame.offset.x * k_f_0_11111111;
    frame.force_x = 0.0;
    frame.force_y = 180.0;
    frame.force_z = 0.0;
    frame.offset.y = frame.offset.y * k_f_0_11111111;
    frame.offset.z = frame.offset.z * k_f_0_11111111;
    frame.at_x = frame.offset.x + frame.at_x;
    frame.at_y = frame.offset.y + frame.at_y;
    frame.at_z = frame.offset.z + frame.at_z;
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    return;
  case 4:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    RwV3dTransformVectors(&frame.offset,(RwV3d *)&g_car_62ac68,1,(RwMatrix *)player_car[car_index].matrices)
    ;
    frame.offset.x = frame.offset.x * k_f_n0_11111111;
    frame.force_x = 0.0;
    frame.force_y = 180.0;
    frame.force_z = 0.0;
    frame.offset.y = frame.offset.y * k_f_n0_11111111;
    frame.offset.z = frame.offset.z * k_f_n0_11111111;
    frame.at_x = frame.offset.x + frame.at_x;
    frame.at_y = frame.offset.y + frame.at_y;
    frame.at_z = frame.offset.z + frame.at_z;
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    return;
  case 5:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    frame.offset.x = player_car[car_index].forward[0] * k_f_0_22222222;
    frame.force_y = 425.0;
    frame.offset.y = player_car[car_index].forward[1] * k_f_0_22222222;
    frame.offset.z = player_car[car_index].forward[2] * k_f_0_22222222;
    goto LAB_004929af;
  case 6:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    frame.offset.x = player_car[car_index].forward[0] * k_f_n0_22222222;
    frame.force_y = 425.0;
    frame.offset.y = player_car[car_index].forward[1] * k_f_n0_22222222;
    frame.offset.z = player_car[car_index].forward[2] * k_f_n0_22222222;
    break;
  case 7:
    dir_ptr = (float *)(*(char **)(*(char **)(*body_ptr + 0x10) + 8) + body_ptr[1] * 0x20);
    frame.offset.x = *dir_ptr * k_f_n6_6666666en4;
    frame.offset.y = dir_ptr[1] * k_f_n6_6666666en4;
    frame.offset.z = dir_ptr[2] * k_f_n6_6666666en4;
    frame.at_x = frame.offset.x + *(float *)(car_or_matrix + 0x30);
    frame.at_y = frame.offset.y + *(float *)(car_or_matrix + 0x34);
    frame.at_z = frame.offset.z + *(float *)(car_or_matrix + 0x38);
    frame.offset.x = player_car[car_index].forward[0] * k_f_n0_22222222;
    frame.force_y = 125.0;
    frame.offset.y = player_car[car_index].forward[1] * k_f_n0_22222222;
    frame.offset.z = player_car[car_index].forward[2] * k_f_n0_22222222;
LAB_004929af:
    frame.at_x = frame.offset.x + frame.at_x;
    frame.at_y = frame.offset.y + frame.at_y;
    frame.force_x = 0.0;
    frame.force_z = 0.0;
    frame.at_z = frame.offset.z + frame.at_z;
    FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    return;
  case 8:
    owner_or_hit = physics_body_get_velocity(&frame.offset,car_index);
    speed = k_f_100;
    if (owner_or_hit != 0) {
      speed = RwV3dLength(&frame.offset);
    }
    if ((6000 < *(int *)&player_car[car_index].field_0xc) || (speed < float_1_0)) {
      frame.at_x = *(float *)(car_or_matrix + 0x30);
      frame.at_y = *(float *)(car_or_matrix + 0x34) + k_f_0_5;
      frame.at_z = *(float *)(car_or_matrix + 0x38);
      frame.force_x = player_car[car_index].up[0] * k_f_n6_5;
      frame.force_y = player_car[car_index].up[1] * k_f_n6_5;
      frame.force_z = player_car[car_index].up[2] * k_f_n6_5;
      FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
    }
  default:
    return;
  }
  frame.at_x = frame.offset.x + frame.at_x;
  frame.at_y = frame.offset.y + frame.at_y;
  frame.force_x = 0.0;
  frame.force_z = 0.0;
  frame.at_z = frame.offset.z + frame.at_z;
  FUN_0056bc20(body_ptr,&frame.force_x,&frame.at_x);
  return;
}


