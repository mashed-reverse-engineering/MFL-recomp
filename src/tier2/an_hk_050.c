#include "mfl_tier2.h"
typedef int CourseId;
typedef int WeaponId;
/*0042b110*/ extern int current_course_id();
/*0047b520*/ extern int car_current_matrix();
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056e5b0*/ extern void FUN_0056e5b0(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005deb34*/ extern float k_f_300;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];

#define MAX_CAR_INDEX (0xf)
#define COURSE_HIGHWAY (38)
#define COURSE_HIGHWAYN (12)
#define RW_MATRIX_FLOAT_COUNT (0x10)
#define POS_RECORD_STRIDE (0x20) /* (inferred) bytes per point record in the path table */
#define RWP_TIMER_START_MS (600)


/* car_reset_physics */

/* "(car): resets the physics object of a car when it exists (field 0x14 set), 0 if car >= 16" */

/* Resets the physics object of one car: copies the car's current matrix into the object's matrix,
 * moves the object back by the car's body offset, and drops the car out of the physics library
 * (rwp_object = 0), arming rwp_timer. The velocity is set from the path point (inferred).
 * car_index: car slot, 0..MAX_CAR_INDEX.
 * Returns 1 when the car was reset, 0 when car_index is out of range or the car has no physics object. */
undefined4 car_reset_physics(int car_index)

{
  int *path_cursor;
  RwMatrix *object_matrix;
  int iVar2;
  CourseId course_id;
  float *point_pos;
  RwMatrix *dst;
  RwReal *current_matrix;
  int owner_object;
  RwV3d body_offset;
  float point_x;
  float point_y;
  float point_z;

  if (MAX_CAR_INDEX < car_index) {
    return 0;
  }
  if (player_car[car_index].rwp_object == 0) {
    return 0;
  }
  iVar2 = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
  course_id = current_course_id();
  /* Owner object gets FUN_0056b440 on every course except the two highway courses. */
  if (course_id != COURSE_HIGHWAY) {
    course_id = current_course_id();
    if (course_id != COURSE_HIGHWAYN) {
      FUN_0056b440(*(undefined4 *)(iVar2 + 0x24),iVar2,1);
    }
  }
  path_cursor = *(int **)(*(char **)(iVar2 + 0x10) + 4);
  /* Only when the owner has a path cursor (inferred): velocity = path point * k_f_300. */
  if (path_cursor != (int *)0x0) {
    iVar2 = *(int *)(*(char **)(*path_cursor + 0x10) + 8);
    point_y = *(float *)(path_cursor[1] * POS_RECORD_STRIDE + 4 + iVar2);
    point_pos = (float *)(path_cursor[1] * POS_RECORD_STRIDE + iVar2);
    point_x = *point_pos;
    point_z = point_pos[2];
    player_car[car_index].velocity[0] = point_x * k_f_300;
    player_car[car_index].velocity[1] = point_y * k_f_300;
    player_car[car_index].velocity[2] = point_z * k_f_300;
    car_current_matrix(&current_matrix,car_index);
    owner_object = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
    object_matrix = *(RwMatrix **)(*(char **)(owner_object + 0x10) + 8);
    dst = object_matrix;
    /* Copy the 16 floats of the car's current matrix into the object's matrix. */
    for (iVar2 = RW_MATRIX_FLOAT_COUNT; iVar2 != 0; iVar2 = iVar2 + -1) {
      (dst->right).x = *current_matrix;
      current_matrix = current_matrix + 1;
      dst = (RwMatrix *)&(dst->right).y;
    }
    /* Move the object back by the body offset, transformed by the object's matrix. */
    body_offset.x = player_car[car_index].rwp_body_offset[0];
    body_offset.y = player_car[car_index].rwp_body_offset[1];
    body_offset.z = player_car[car_index].rwp_body_offset[2];
    RwV3dTransformVectors(&body_offset,&body_offset,1,object_matrix);
    (object_matrix->pos).x = (object_matrix->pos).x - body_offset.x;
    (object_matrix->pos).y = (object_matrix->pos).y - body_offset.y;
    (object_matrix->pos).z = (object_matrix->pos).z - body_offset.z;
    RwMatrixUpdate(object_matrix);
    RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[car_index] + 4));
    iVar2 = owner_object;
    FUN_0056e5b0(owner_object,0);
    rwp_world_set_bit_5c(*(undefined4 *)(iVar2 + 0x24),iVar2,1);
  }
  player_car[car_index].rwp_object = 0;
  player_car[car_index].rwp_timer = RWP_TIMER_START_MS;
  return 1;
}
