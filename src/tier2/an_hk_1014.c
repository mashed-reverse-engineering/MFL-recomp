#include "mfl_tier2.h"
/*00422280*/ extern int vehicle_part_frame();
/*00422320*/ extern int vehicle_get_matrix_local();
/*00422350*/ extern char *vehicle_part_ltm();
/*00424880*/ extern int debris_apply_velocity_a(int emitter, float *velocity);
/*00424930*/ extern undefined __cdecl debris_apply_velocity_b(undefined4 p0, void *p1, undefined4 p2, undefined4 p3, void *p4, undefined4 p5, undefined4 p6, undefined4 p7, undefined4 p8, undefined4 p9, undefined4 p10, undefined4 p11, undefined4 p12, undefined4 p13, undefined4 p14, undefined4 p15, undefined4 p16);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004682c0*/ extern int __cdecl rocket_surface_hit_test(void *p0, void *p1);
/*00475730*/ extern int get_viewport_camera();
/*0047abb0*/ extern int car_get_velocity_any_mode();
/*00485660*/ extern int particle_pool_find_oldest_or_free();
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*00498f40*/ extern int fx_ring_add_point();
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004c4720*/ extern void vec3_lerp(void *p0, void *p1, void *p2, undefined4 p3);
/*004c4c30*/ extern int FUN_004c4c30(undefined4 p0, void *p1, void *p2);
/*004c51a0*/ extern void rwmatrix_from_three_points(void *p0, undefined4 p1, void *p2);
/*004d1460*/ extern struct RwFrame * RwFrameTranslate(struct RwFrame *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*004d46e0*/ extern RwMatrix * RwMatrixScale(RwMatrix *p0, struct RwV3d *p1, enum RwOpCombineType p2);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*0056b440*/ extern void FUN_0056b440(undefined4 p0, undefined4 p1, undefined4 p2);
/*005dd36c*/ extern float k_f_0_4;
/*005dd628*/ extern float k_f_0_25;
/*005ddafc*/ extern float k_f_0_05;
/*005ddb18*/ extern float float_0_8;
/*005e64a4*/ extern float k_f_0;
/*005de1ac*/ extern float k_f_0_125;
/*005de3e4*/ extern float k_f_40;
/*005de47c*/ extern float k_f_n0_25;
/*005de480*/ extern float k_f_n0_4;
static int rwp_world_current_activate_r(undefined4 reg_eax);
extern void rwp_world_current_activate(void);
__declspec(naked) int rwp_world_current_activate_r(undefined4 reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call rwp_world_current_activate
        ret
    }
}

/* vehicle_smoke_emitters_spawn */
#define DEBRIS_PIECE_COUNT (10) /* detachable pieces per debris set */

/* Spawns a smoke emitter at +/-0.4 on one side of the car plus a ground decal, then throws away each of the 10 detachable debris pieces with random velocity and spin, marking them done in the bit set at debris_set+0xf0 (inferred; was: "spawns smoke emitters at +/-0.4 side offsets of the vehicle, with a random size") */

void vehicle_smoke_emitters_spawn(char *debris_set)

{
  float fbits1;
  float fbits2;
  float fbits3;
  float fbits4;
  RwV3d *out;
  byte *done_byte;
  undefined4 rw_world;
  int tmp_i;
  RwMatrix *m;
  int owner_tmp;
  int part_id;
  float *piece;
  double rand_val;
  float side_offset;
  int emitter;
  float *hit_buf_ptr;
  undefined4 extra_arg_9;
  undefined4 extra_arg_10;
  undefined4 extra_arg_11;
  undefined4 extra_arg_12;
  undefined4 extra_arg_13;
  undefined4 extra_arg_14;
  undefined4 extra_arg_15;
  undefined4 extra_arg_16;
  undefined4 extra_arg_17;
  undefined4 extra_arg_18;
  undefined4 extra_arg_19;
  struct {
    RwV3d throw_dir;
    float spin_axis_x;
    float spin_axis_y;
    float spin_axis_z;
    float smoke_pos_x;
    float smoke_pos_y;
    float smoke_pos_z;
    float impact_x;
    float impact_y;
    float impact_z;
    int piece_index;
    float probe_start_x;
    float probe_start_y;
    float probe_start_z;
    float probe_end_x;
    float probe_end_y;
    float probe_end_z;
    int owner_saved;
    RwV3d decal_scale;
    undefined1 car_local_matrix [32];
    float car_up_x;
    float car_up_y;
    float car_up_z;
    undefined1 pad_a0[4];
    float car_pos_x;
    float car_pos_y;
    float car_pos_z;
    undefined1 pad_90[4];
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_80[4];
    undefined1 hit_buf [40];
    undefined4 hit_tail;
    undefined1 pad_50[4];
    undefined1 car_velocity [12];
    RwMatrix decal_matrix;
  } frame;
  
  /* part 1: smoke emitter on the left/right side (+-0.4) of the car, plus a ground decal under it when the probe hits the world */
  owner_tmp = *(int *)(debris_set + 0xfc);
  frame.throw_dir.x = 0.0;
  frame.throw_dir.y = 0.0;
  frame.throw_dir.z = 0.0;
  frame.owner_saved = owner_tmp;
  vehicle_get_matrix_local(*(undefined4 *)(owner_tmp + 0x204),frame.car_local_matrix);
  car_get_velocity_any_mode(frame.car_velocity,*(undefined4 *)(*(char **)(debris_set + 0xfc) + 0x204));
  tmp_i = *(int *)(debris_set + 0xf8);
  frame.spin_axis_x = 0.0;
  frame.spin_axis_y = 1.0;
  frame.spin_axis_z = 0.0;
  side_offset = k_f_n0_4;
  if (tmp_i == 0) {
    side_offset = k_f_0_4;
  }
  frame.smoke_pos_x = side_offset * frame.car_up_x + frame.car_pos_x;
  side_offset = k_f_n0_4;
  if (tmp_i == 0) {
    side_offset = k_f_0_4;
  }
  frame.smoke_pos_y = side_offset * frame.car_up_y + frame.car_pos_y;
  side_offset = k_f_n0_4;
  if (tmp_i == 0) {
    side_offset = k_f_0_4;
  }
  frame.smoke_pos_z = side_offset * frame.car_up_z + frame.car_pos_z;
  rand_val = (double)rp_random_float_range(0x3f4ccccd,0x3f800000);
  fx_emitter_spawn_b_default(&frame.smoke_pos_x,&frame.spin_axis_x,0x41f00000,(fbits1 = (float)rand_val, *(int *)&fbits1));
  frame.probe_start_x = frame.spin_axis_x * k_f_0_125 + frame.smoke_pos_x;
  frame.probe_start_y = frame.spin_axis_y * k_f_0_125 + frame.smoke_pos_y;
  hit_buf_ptr = &frame.hit_normal_x;
  piece = &frame.probe_start_x;
  frame.probe_start_z = frame.spin_axis_z * k_f_0_125 + frame.smoke_pos_z;
  frame.probe_end_x = frame.spin_axis_x * k_f_n0_25 + frame.smoke_pos_x;
  frame.probe_end_y = frame.spin_axis_y * k_f_n0_25 + frame.smoke_pos_y;
  frame.probe_end_z = frame.spin_axis_z * k_f_n0_25 + frame.smoke_pos_z;
  rw_world = weapon_get_rw_801b14();
  tmp_i = FUN_004c4c30(rw_world,piece,hit_buf_ptr);
  if ((tmp_i != 0) && (tmp_i = rocket_surface_hit_test(&frame.hit_normal_x,&frame.smoke_pos_x), tmp_i == 0)) {
    vec3_lerp(&frame.impact_x,&frame.probe_start_x,&frame.probe_end_x,(undefined4)frame.hit_tail);
    frame.impact_x = frame.hit_normal_x * k_f_0_05 + frame.impact_x;
    frame.impact_y = frame.hit_normal_y * k_f_0_05 + frame.impact_y;
    frame.impact_z = frame.hit_normal_z * k_f_0_05 + frame.impact_z;
    rwmatrix_from_three_points(&frame.decal_matrix,(undefined4)frame.hit_buf,&frame.impact_x);
    rand_val = (double)rp_random_float_range(0x3f333333,0x3f800000);
    frame.decal_scale.y = (RwReal)rand_val;
    frame.decal_scale.x = (RwReal)rand_val;
    frame.decal_scale.z = 1.0;
    RwMatrixScale(&frame.decal_matrix,&frame.decal_scale,rwCOMBINEPRECONCAT);
    particle_pool_find_oldest_or_free(0,&frame.decal_matrix,0);
    rand_val = (double)rp_random_float_range(0x3ecccccd,0x3f19999a);
    fx_spawn_from_matrix_2(&frame.decal_matrix,(fbits2 = (float)rand_val, *(int *)&fbits2));
  }
  /* part 2: for each of the 10 detachable pieces: move its frame to the car part, throw it away from the car/camera with a random spin, mark it done */
  frame.piece_index = 0;
  piece = (float *)(debris_set + 8);
  do {
    out = (RwV3d *)(piece + -2);
    if (*(int *)&piece[1] != 0) {
      part_id = (-(uint)(*(int *)(debris_set + 0xf8) != 0) & 10) + 0x50 + frame.piece_index;
      tmp_i = vehicle_part_frame(*(undefined4 *)(owner_tmp + 0x204),part_id);
      if (tmp_i == 0) {
        return;
      }
      m = (RwMatrix *)vehicle_part_ltm(*(undefined4 *)(owner_tmp + 0x204),part_id);
      RwFrameTransform(*(RwFrame **)(*(int *)&piece[1] + 4),m,rwCOMBINEREPLACE);
      out->x = (m->pos).x - frame.car_pos_x;
      piece[-1] = (m->pos).y - frame.car_pos_y;
      *piece = (m->pos).z - frame.car_pos_z;
      RwV3dNormalize(out,out);
      emitter = *(int *)&piece[1];
      out->x = out->x * k_f_0_25;
      piece[-1] = piece[-1] * k_f_0_25;
      *piece = *piece * k_f_0_25;
      RwFrameTranslate(*(RwFrame **)(emitter + 4),out,rwCOMBINEPOSTCONCAT);
      rwp_world_current_activate_r(*(undefined4 *)&piece[2]);
      FUN_0056b440(*(undefined4 *)(*(int *)&piece[2] + 0x24),*(int *)&piece[2],6);
      owner_tmp = get_viewport_camera(0);
      owner_tmp = *(int *)(owner_tmp + 4);
      frame.impact_x = *(float *)(owner_tmp + 0x30);
      frame.throw_dir.x = *(float *)(owner_tmp + 0x40) - frame.car_pos_x;
      frame.impact_y = *(float *)(owner_tmp + 0x34);
      frame.impact_z = *(float *)(owner_tmp + 0x38);
      frame.throw_dir.y = *(float *)(owner_tmp + 0x44) - frame.car_pos_y;
      frame.throw_dir.z = *(float *)(owner_tmp + 0x48) - frame.car_pos_z;
      RwV3dNormalize(&frame.throw_dir,&frame.throw_dir);
      extra_arg_19 = 0x3e99999a;
      extra_arg_18 = 0xbe99999a;
      frame.throw_dir.x = frame.impact_x * float_0_8 + frame.throw_dir.x;
      frame.throw_dir.z = frame.impact_z * float_0_8 + frame.throw_dir.z;
      rand_val = (double)rp_random_float_range(0xbe99999a, 0x3e99999a);
      extra_arg_17 = 0x3fc00000;
      extra_arg_16 = 0x3f800000;
      frame.throw_dir.x = (RwReal)(rand_val + (double)frame.throw_dir.x);
      rand_val = (double)rp_random_float_range(0x3f800000, 0x3fc00000);
      extra_arg_15 = 0x3e99999a;
      frame.throw_dir.y = (RwReal)rand_val;
      extra_arg_14 = 0xbe99999a;
      rand_val = (double)rp_random_float_range(0xbe99999a, 0x3e99999a);
      extra_arg_13 = 0x43480000;
      frame.throw_dir.x = frame.throw_dir.x * k_f_40;
      extra_arg_12 = 0x42c80000;
      frame.throw_dir.y = frame.throw_dir.y * k_f_40;
      frame.throw_dir.z = (RwReal)((rand_val + (double)frame.throw_dir.z) * (double)k_f_40);
      frame.spin_axis_x = *piece - piece[-1] * k_f_0;
      frame.spin_axis_y = out->x * k_f_0 - *piece * k_f_0;
      frame.spin_axis_z = piece[-1] * k_f_0 - out->x;
      rand_val = (double)rp_random_float_range(0x42c80000, 0x43480000);
      extra_arg_11 = 0x43480000;
      extra_arg_10 = 0x42c80000;
      frame.spin_axis_x = (float)(rand_val * (double)frame.spin_axis_x);
      rand_val = (double)rp_random_float_range(0x42c80000, 0x43480000);
      extra_arg_9 = 0x43480000;
      rw_world = 0x42c80000;
      frame.spin_axis_y = (float)(rand_val * (double)frame.spin_axis_y);
      rand_val = (double)rp_random_float_range(0x42c80000, 0x43480000);
      emitter = *(int *)&piece[1];
      hit_buf_ptr = &frame.spin_axis_x;
      frame.spin_axis_z = (float)(rand_val * (double)frame.spin_axis_z);
      debris_apply_velocity_a(emitter,hit_buf_ptr);
      debris_apply_velocity_b
                (*(int *)&piece[1],&frame.throw_dir,0,emitter,hit_buf_ptr,rw_world,extra_arg_9,extra_arg_10,extra_arg_11,extra_arg_12,extra_arg_13,extra_arg_14,
                 extra_arg_15,extra_arg_16,extra_arg_17,extra_arg_18,extra_arg_19);
      owner_tmp = 4;
      do {
        rand_val = (double)rp_random_float_range(0x3ecccccd,0x3f4ccccd);
        fx_ring_add_point(*(int *)(*(int *)&piece[1] + 4) + 0x40,(fbits3 = (float)rand_val, *(int *)&fbits3));
        owner_tmp = owner_tmp + -1;
      } while (owner_tmp != 0);
      owner_tmp = 5;
      do {
        rand_val = (double)rp_random_float_range(0x3ecccccd,0x3f4ccccd);
        fx_ring_add_point(&m->pos,(fbits4 = (float)rand_val, *(int *)&fbits4));
        owner_tmp = owner_tmp + -1;
      } while (owner_tmp != 0);
      done_byte = (byte *)((frame.piece_index >> 3) + 0xf0 + debris_set);
      *done_byte = *done_byte | '\x01' << ((byte)frame.piece_index & 7);
      piece[3] = 0.0;
      owner_tmp = frame.owner_saved;
    }
    frame.piece_index = frame.piece_index + 1;
    piece = piece + 6;
  } while (frame.piece_index < DEBRIS_PIECE_COUNT);
  *(undefined4 *)(debris_set + 0xf4) = 1;
  return;
}


