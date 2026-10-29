// cluster-entries: weapon_mine_drop
/* Installed cluster weapon_mine_drop (tools/cluster_install.py from output/cluster_pilot8/weapon_mine_drop.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00467e90*/ extern int course_get_rpworld_00467e90();
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00467fd0*/ extern int __cdecl course_surface_id_is_active_check(undefined4 p0);
/*004c4720*/ extern void vec3_lerp(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void rwmatrix_from_three_points(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d43f0*/ extern RwMatrix * RwMatrixRotate(RwMatrix *p0, struct RwV3d *p1, RwReal p2, enum RwOpCombineType p3);
/*004f4570*/ extern struct RpWorld * RpWorldAddClump(struct RpWorld *p0, struct RpClump *p1);
/*005dd340*/ extern float k_f_0_5;
/*005dd364*/ extern float k_f_n1;
/*005de25c*/ extern float k_f_0_005;
/*004726a0*/ extern int sfx_play_at(int sound, float *position);
/* a dropped mine slot (the pool in the weapon state): matrix of the placed mine, position, clump */
typedef struct MineSlot {
    RwMatrix matrix;       /* 0x00 */
    float position[3];     /* 0x40 */
    float radius;          /* 0x4c */
    char unknown_50[8];
    RpClump *clump;        /* 0x58 */
    float timer_5c;
    int state;             /* 0x60: 2 = placed */
    float timer_64;
} MineSlot;

/* weapon_mine_drop */

/* "places a mine at the car (matrix rotate / transform) and adds its clump to the course world" */

void weapon_mine_drop_n(int mine_pool)

{
  MineSlot *mine;
  undefined4 rw_world;
  int tmp_i;
  RpWorld *world;
  RpClump *clump;
  float *seg_start;
  float *hit_buf_ptr;
  struct {
    float mine_x;
    float mine_y;
    float mine_z;
    float car_x;
    float car_y;
    float car_z;
    float probe_end_x;
    float probe_end_y;
    float probe_end_z;
    RwV3d rot_axis;
    undefined4 hit_t;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    int hit_triangle;
    undefined1 hit_aux [36];
    int hit_sector;
    undefined4 hit_record_t;
    int hit_world_obj;
    RwMatrix mine_matrix;
  } frame;
  
  /* (inferred) probe segment from the car matrix at mine_pool+0xc along its negated axis; the mine is placed on the hit point */
  tmp_i = *(int *)(mine_pool + 0xc);
  frame.car_x = *(float *)(tmp_i + 0x30);
  frame.rot_axis.z = *(float *)(tmp_i + 0x18) * k_f_n1;
  frame.car_y = *(float *)(tmp_i + 0x34);
  frame.car_z = *(float *)(tmp_i + 0x38);
  frame.probe_end_x = *(float *)(tmp_i + 0x10) * k_f_n1 * k_f_0_5 + frame.car_x;
  hit_buf_ptr = &frame.hit_normal_x;
  seg_start = &frame.car_x;
  frame.probe_end_y = *(float *)(tmp_i + 0x14) * k_f_n1 * k_f_0_5 + frame.car_y;
  frame.probe_end_z = frame.rot_axis.z * k_f_0_5 + frame.car_z;
  rw_world = weapon_get_rw_801b14();
  tmp_i = world_intersect_line(rw_world,seg_start,hit_buf_ptr);
  if (tmp_i != 0) {
    frame.rot_axis.x = 1.0;
    frame.rot_axis.y = 0.0;
    frame.rot_axis.z = 0.0;
    frame.hit_t = frame.hit_record_t;
    tmp_i = course_surface_id_is_active_check
                      (*(undefined4 *)
                        (*(int *)(frame.hit_world_obj + 0x10) +
                        ((uint)*(ushort *)(*(int *)(frame.hit_sector + 4) + 6 + frame.hit_triangle * 8) +
                        (uint)*(ushort *)(frame.hit_sector + 0x80)) * 4));
    if (tmp_i == 0) {
      vec3_lerp(&frame.mine_x,&frame.car_x,&frame.probe_end_x,frame.hit_t);
      tmp_i = *(int *)(mine_pool + 8) + -1;
      *(int *)(mine_pool + 8) = tmp_i;
      frame.mine_x = frame.hit_normal_x * k_f_0_005 + frame.mine_x;
      frame.mine_y = frame.hit_normal_y * k_f_0_005 + frame.mine_y;
      frame.mine_z = frame.hit_normal_z * k_f_0_005 + frame.mine_z;
      mine = *(MineSlot **)(mine_pool + tmp_i * 4);
      *(undefined4 *)(mine_pool + tmp_i * 4) = 0;
      mine->timer_5c = 0.0;
      mine->state = 2;
      mine->timer_64 = 0.0;
      mine->position[0] = frame.mine_x;
      mine->position[1] = frame.mine_y;
      mine->position[2] = frame.mine_z;
      mine->radius = 0.125;
      rwmatrix_from_three_points(&frame.mine_matrix,frame.hit_aux,&frame.mine_x);
      mine->matrix = frame.mine_matrix;
      RwMatrixRotate(&frame.mine_matrix,&frame.rot_axis,-90.0,rwCOMBINEPRECONCAT);
      RwFrameTransform(*(RwFrame **)((char *)mine->clump + 4),&frame.mine_matrix,rwCOMBINEREPLACE);
      clump = mine->clump;
      world = (RpWorld *)course_get_rpworld_00467e90();
      RpWorldAddClump(world,clump);
    }
  }
  return;
}

/* entry thunk at the original address of weapon_mine_drop: register inputs (eax) -> natural C weapon_mine_drop_n; ret 0 */
__declspec(naked) void weapon_mine_drop(void)
{
    __asm {
        push eax
        call weapon_mine_drop_n
        add esp, 4
        ret
    }
}
