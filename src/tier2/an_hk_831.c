#include "mfl_tier2.h"
/*0047b520*/ extern int car_current_matrix();
/*004b2bbd*/ extern int __cdecl FID_conflict__wprintf(char *p0, ...);
/*004d0f70*/ extern struct RwFrame * RwFrameUpdateObjects(struct RwFrame *p0);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*004d56d0*/ extern struct RwV3d * RwV3dTransformVectors(struct RwV3d *p0, struct RwV3d *p1, RwInt32 p2, RwMatrix *p3);
/*0056b1d0*/ extern void rwp_world_set_bit_5c(int p0, int p1, int p2);
/*0056b300*/ extern void FUN_0056b300(undefined4 p0, undefined4 p1);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*0056e5b0*/ extern void FUN_0056e5b0(undefined4 p0, undefined4 p1);
/*0058c8a0*/ extern int rwp_owner_object_get(int p0);
/*005deb34*/ extern float k_f_300;
/*005dfccc*/ extern float k_f_5000;
/*005dfd8c*/ extern float k_f_0_0033333334;
/*006fd498*/ extern undefined4 g_rw_6fd498;
/*008b06e0*/ extern struct Car player_car[16];

#define LAST_CAR_INDEX (15)
#define BIT_IN_WORD_MASK (0x1f)
#define BODY_RECORD_STRIDE (0x20) /* bytes per body record */
#define BODY_AUX_OFFSET (0x10) /* (inferred) second field of a body record, 3 floats zeroed */
#define RWP_GROUP_OFFSET (2) /* (inferred) added to car_index for the group id passed to FUN_0056b440 */
#define RWP_TIMER_MS (600) /* countdown in ms stored in rwp_timer */

/* car_enter_rwp_mode: switches car_index to RenderWare Physics (rwp) simulation.
   Hands the car's matrix and velocity to its rwp body, moves the body by the car's rwp_body_offset,
   clears the car's velocity and angular velocity, sets rwp_object = 1 and rwp_timer = 600.
   Returns 1 on success, 0 if car_index is out of range or the car is already in rwp mode.
   (inferred from its debug strings: RWP speed / SPEED ADJUSTMENT OCCURRED!) */

undefined4 car_enter_rwp_mode(int car_index)

{
  undefined4 *body_aux;
  int *body_ref;
  RwMatrix *body_matrix;
  float scaled_speed;
  int owner;
  RwReal *body_vel;
  int world;
  float *body_vel_out;
  RwMatrix *matrix_write;
  RwReal body_speed;
  RwReal car_speed;
  struct {
    RwReal *matrix_src;
    int owner_saved;
    RwV3d vec_tmp;
    RwV3d body_offset_world;
  } enter_state;

  if (LAST_CAR_INDEX < car_index) {
    return 0;
  }
  if (player_car[car_index].rwp_object == 0) {
    /* the car's current matrix (16 floats) is read through matrix_src below */
    car_current_matrix(&enter_state.matrix_src, car_index);
    owner = rwp_owner_object_get((&g_rw_6fd498)[car_index]);
    world = *(int *)(owner + 0x24);
    enter_state.owner_saved = owner;
    /* join the owner to the world if it is not in it yet: no world, or its bit is clear in the
       world's bit table at +0x60 (inferred) */
    if ((world == 0) ||
       ((1 << ((byte)*(ushort *)(owner + 0x20) & BIT_IN_WORD_MASK) &
        *(uint *)(*(char **)(world + 0x60) + (uint)(*(ushort *)(owner + 0x20) >> 5) * 4)) == 0)) {
      FUN_0056b300(world, owner);
      rwp_world_set_bit_5c(*(undefined4 *)(owner + 0x24),owner, 0x1);
    }
    FUN_0056b440(*(int *)(owner + 0x24), owner, (car_index + RWP_GROUP_OFFSET));
    body_ref = *(int **)(*(char **)(owner + 0x10) + 4);
    /* body record of the owner: zero its aux vector, then read its velocity into vec_tmp */
    body_aux = (undefined4 *)(*(char **)(*(char **)(*body_ref + 0x10) + 8) + BODY_AUX_OFFSET + body_ref[1] * BODY_RECORD_STRIDE);
    *body_aux = 0;
    body_aux[1] = 0;
    body_aux[2] = 0;
    body_vel = (RwReal *)(*(char **)(*(char **)(*body_ref + 0x10) + 8) + body_ref[1] * BODY_RECORD_STRIDE);
    enter_state.vec_tmp.x = *body_vel;
    enter_state.vec_tmp.y = body_vel[1];
    enter_state.vec_tmp.z = body_vel[2];
    body_speed = RwV3dLength(&enter_state.vec_tmp);
    car_speed = RwV3dLength((RwV3d *)player_car[car_index].velocity);
    scaled_speed = body_speed * k_f_300;
    /* body speed * 300 above 5000 (inferred: speed rescale check): replace the body velocity with
       the car velocity * (1/300) and log it */
    if (k_f_5000 < scaled_speed) {
      enter_state.vec_tmp.x = player_car[car_index].velocity[0] * k_f_0_0033333334;
      enter_state.vec_tmp.y = player_car[car_index].velocity[1] * k_f_0_0033333334;
      enter_state.vec_tmp.z = player_car[car_index].velocity[2] * k_f_0_0033333334;
      body_vel_out = (float *)(*(char **)(*(char **)(*body_ref + 0x10) + 8) + body_ref[1] * BODY_RECORD_STRIDE);
      *body_vel_out = enter_state.vec_tmp.x;
      body_vel_out[1] = enter_state.vec_tmp.y;
      body_vel_out[2] = enter_state.vec_tmp.z;
      FID_conflict__wprintf("RWP speed=%f (%f)\n",(double)body_speed,(double)scaled_speed);
      FID_conflict__wprintf("PW car speed=%f\n",(double)car_speed);
      FID_conflict__wprintf("SPEED ADJUSTMENT OCCURRED!\n");
    }
    body_matrix = *(RwMatrix **)(*(char **)(owner + 0x10) + 8);
    enter_state.vec_tmp.x = (body_matrix->pos).x;
    enter_state.vec_tmp.y = (body_matrix->pos).y;
    enter_state.vec_tmp.z = (body_matrix->pos).z;
    /* copy the 16 car matrix floats into the body matrix (right, up, at, pos; world is reused as
       the loop counter) */
    matrix_write = body_matrix;
    for (world = 0x10; world != 0; world = world + -1) {
      (matrix_write->right).x = *enter_state.matrix_src;
      enter_state.matrix_src = enter_state.matrix_src + 1;
      matrix_write = (RwMatrix *)&(matrix_write->right).y;
    }
    enter_state.body_offset_world.x = player_car[car_index].rwp_body_offset[0];
    enter_state.body_offset_world.y = player_car[car_index].rwp_body_offset[1];
    enter_state.body_offset_world.z = player_car[car_index].rwp_body_offset[2];
    /* body position = body position - rwp_body_offset rotated by the body matrix */
    RwV3dTransformVectors(&enter_state.body_offset_world,&enter_state.body_offset_world,1,body_matrix);
    (body_matrix->pos).x = (body_matrix->pos).x - enter_state.body_offset_world.x;
    (body_matrix->pos).y = (body_matrix->pos).y - enter_state.body_offset_world.y;
    (body_matrix->pos).z = (body_matrix->pos).z - enter_state.body_offset_world.z;
    RwMatrixUpdate(body_matrix);
    RwFrameUpdateObjects(*(RwFrame **)((&g_rw_6fd498)[car_index] + 4));
    world = enter_state.owner_saved;
    FUN_0056e5b0(enter_state.owner_saved,0);
    rwp_world_set_bit_5c(*(undefined4 *)(world + 0x24),world,1);
    /* the physics body now drives the car: zero its angular and linear velocity */
    player_car[car_index].angular_velocity[2] = 0.0;
    player_car[car_index].angular_velocity[1] = 0.0;
    player_car[car_index].angular_velocity[0] = 0.0;
    player_car[car_index].velocity[2] = 0.0;
    player_car[car_index].velocity[1] = 0.0;
    player_car[car_index].velocity[0] = 0.0;
    player_car[car_index].rwp_object = 1;
    player_car[car_index].rwp_timer = RWP_TIMER_MS;
    return 1;
  }
  return 0;
}


