// cluster-entries: weapon_flamethrower_update
/* Installed cluster weapon_flamethrower_update (tools/cluster_install.py from output/cluster_pilot_j1/weapon_flamethrower_update.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004667d0*/ extern undefined __cdecl weapon_flame_push_cars(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*00475760*/ extern int aux_camera_get_position();
/*00481fd0*/ extern double random_range();
/*00484680*/ extern int object_set_flag_4();
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*0049b870*/ extern int particle_batch_mark_current_active();
/*0049b890*/ extern int particle_batch_add();
/*0049b910*/ extern int particle_batch_set_uv_a();
/*0049b940*/ extern int particle_batch_set_size();
/*0049b960*/ extern int particle_batch_set_uv_b_c();
/*0049bd50*/ extern int fx_system_flush();
/*004b2b48*/ extern int _ftol();
/*004c4720*/ extern void FUN_004c4720(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*005dd330*/ extern float float_1_0;
/*005dd36c*/ extern float k_f_0_4;
/*005dd628*/ extern float k_f_0_25;
/*005de20c*/ extern float k_f_2_5;
/*005de284*/ extern float k_f_1_1111112;
/*005de490*/ extern float k_f_0_02;
/*005df2c8*/ extern float k_f_0_002;
/*0060db20*/ extern undefined g_weapon_flamethrower_60db20;
/*0060db28*/ extern undefined g_weapon_flamethrower_60db28;
/*006a9588*/ extern undefined g_weapon_6a9588[0x1000];
/*006a958c*/ extern undefined g_weapon_flamethrower_6a958c;
/*006a972c*/ extern undefined4 g_weapon_flamethrower_6a972c[0x1000];
/*006a9740*/ extern undefined4 g_weapon_flamethrower_6a9740;
/*007ef7f4*/ extern float g_tick_seconds;
static int weapon_flame_particle_update_r(void *flame_state);
extern void weapon_flame_particle_update(void);
__declspec(naked) int weapon_flame_particle_update_r(void *flame_state)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call weapon_flame_particle_update
        ret
    }
}

static int weapon_flame_slot_update_r(int flame_state, int reg_ecx, float * param_3);
extern void weapon_flame_slot_update(void);
__declspec(naked) int weapon_flame_slot_update_r(int flame_state, int reg_ecx, float * unused_3)
{
    __asm {
        push dword ptr [esp + 12]
        mov eax, dword ptr [esp + 8]
        mov ecx, dword ptr [esp + 12]
        call weapon_flame_slot_update
        add esp, 4
        ret
    }
}

static int weapon_flamethrower_register_trigger_sphere_r(void *flame_state, void *stk0);
extern void weapon_flamethrower_register_trigger_sphere(void);
__declspec(naked) int weapon_flamethrower_register_trigger_sphere_r(void *flame_state, void *stk0)
{
    __asm {
        push dword ptr [esp + 8]
        mov eax, dword ptr [esp + 8]
        call weapon_flamethrower_register_trigger_sphere
        add esp, 4
        ret
    }
}

/* weapon_flamethrower_update */


void weapon_flamethrower_update(void)

{
  float fbits1;
  undefined4 *flame_state;
  float tmp_f;
  float *aux_position;
  int tmp_i;
  undefined *batch;
  undefined4 *flame_cur;
  undefined4 *flame_prev;
  float *particle;
  float10 rand_val;
  RwFrame *frame;
  RwMatrix *m;
  float *seg_start;
  undefined4 tmp_u;
  struct {
    undefined1 color_r;
    undefined1 color_g;
    undefined1 color_b;
    undefined1 color_a;
    int flame_index;
    int particles_left;
    int batch_sub;
    int any_particle_alive;
    float hit_pos_x;
    float hit_pos_y;
    float hit_pos_z;
    undefined *batch_base;
    float seg_a_x;
    float seg_a_y;
    float seg_a_z;
    float seg_b_x;
    float seg_b_y;
    float seg_b_z;
    float sprite_size_u;
    float sprite_size_v;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_7c[4];
    undefined1 hit_basis [40];
    undefined4 hit_basis_tail;
    undefined1 pad_4c[4];
    undefined1 hit_fx_matrix [68];
  } frame_obj;
  
  /* white particle colour; alpha is set per particle below */
  frame_obj.color_r = 0xff;
  frame_obj.color_g = 0xff;
  frame_obj.color_b = 0xff;
  frame_obj.color_a = 0xff;
  /* pass 1: per flamethrower: transform its frame, push cars, update particles, free the slot when finished */
  flame_cur = g_weapon_flamethrower_6a972c;
  do {
    flame_prev = flame_cur + -0x1a;
    object_set_flag_4(flame_cur[-0x12],0);
    flame_state = flame_cur + -0x1b;
    if (flame_cur[-0x1b] != 0) {
      m = (RwMatrix *)*flame_prev;
      frame = *(RwFrame **)(flame_cur[-0x13] + 4);
      tmp_u = 0;
      RwFrameTransform(frame,m,rwCOMBINEREPLACE);
      *(float *)&flame_cur[-0x19] = *(float *)&flame_cur[-0x19] - g_tick_seconds;
      weapon_flame_push_cars((undefined4)flame_state,(undefined4)frame,(undefined4)m,tmp_u);
      weapon_flame_particle_update_r((void *)flame_state);
      if (4 < (int)flame_cur[-0x16]) {
        *flame_state = 0;
        flame_cur[-0x16] = 5;
      }
    }
    flame_cur = flame_prev;
  } while (flame_prev != (undefined4 *)&g_weapon_flamethrower_6a958c);
  /* pass 2: per flamethrower / sub-batch: advance the 5 particles, hit test against the world, draw */
  frame_obj.flame_index = 0;
  particle = (float *)&g_weapon_flamethrower_6a9740;
  batch = g_weapon_6a9588;
  do {
    frame_obj.batch_sub = 0;
    frame_obj.batch_base = batch;
    do {
      tmp_i = frame_obj.batch_sub;
      frame_obj.any_particle_alive = 0;
      aux_position = (float *)aux_camera_get_position(0);
      weapon_flame_slot_update_r(frame_obj.flame_index,tmp_i,aux_position);
      frame_obj.particles_left = 5;
      do {
        /* live particle (age fraction < 1): ground hit makes it stick, else it sinks by 0.002 */
        if ((*(int *)&particle[6] != 0) && (*particle < float_1_0)) {
          frame_obj.any_particle_alive = 1;
          if (*(int *)&particle[5] == 0) {
            frame_obj.seg_a_x = particle[-6];
            frame_obj.seg_a_y = particle[-5];
            frame_obj.seg_a_z = particle[-4];
            frame_obj.seg_b_x = frame_obj.seg_a_x + particle[-3];
            aux_position = &frame_obj.hit_normal_x;
            frame_obj.seg_b_y = frame_obj.seg_a_y + particle[-2];
            seg_start = &frame_obj.seg_a_x;
            frame_obj.seg_b_z = frame_obj.seg_a_z + particle[-1];
            tmp_u = weapon_get_rw_801b14();
            tmp_i = world_intersect_line((RpWorld *)tmp_u,(undefined4)seg_start,(undefined4)aux_position);
            if (tmp_i == 0) {
              particle[-2] = particle[-2] - k_f_0_002;
            }
            else {
              FUN_004c4720((undefined4)&frame_obj.hit_pos_x,(undefined4)&frame_obj.seg_a_x,(undefined4)&frame_obj.seg_b_x,(undefined4)frame_obj.hit_basis_tail);
              frame_obj.hit_pos_x = frame_obj.hit_normal_x * k_f_0_02 + frame_obj.hit_pos_x;
              particle[-1] = 0.0;
              tmp_f = frame_obj.hit_normal_y * k_f_0_02;
              particle[-2] = 0.0;
              particle[-3] = 0.0;
              *(int *)&particle[5] = 1;
              frame_obj.hit_pos_y = tmp_f + frame_obj.hit_pos_y;
              frame_obj.hit_pos_z = frame_obj.hit_normal_z * k_f_0_02 + frame_obj.hit_pos_z;
              FUN_004c51a0((undefined4)frame_obj.hit_fx_matrix,(undefined4)frame_obj.hit_basis,(undefined4)&frame_obj.hit_pos_x);
              rand_val = (float10)random_range(0x3e99999a,0x3fa66666);
              fx_spawn_from_matrix_2(frame_obj.hit_fx_matrix,(fbits1 = (float)rand_val, *(int *)&fbits1));
            }
          }
          tmp_f = g_tick_seconds + particle[2];
          particle[2] = tmp_f;
          tmp_f = tmp_f * k_f_1_1111112;
          *particle = tmp_f;
          if (float_1_0 < tmp_f) {
            *particle = 1.0;
          }
          frame_obj.color_a = ((int)(255.0f - (*(float *)((char *)&particle[2] - 0x8) * 235.0f)));
          rand_val = (float10)random_range(0,0x3f800000);
          if ((float10)k_f_0_4 < rand_val) {
            /* random() > 0.4 (60% of ticks) per particle: register a damaging trigger sphere */
            weapon_flamethrower_register_trigger_sphere_r((void *)(particle + -6),(void *)frame_obj.flame_index);
          }
          aux_position = particle + -6;
          particle_batch_add(*(undefined4 *)(batch + 0x10),aux_position,&frame_obj.color_r);
          tmp_f = k_f_0_25;
          if (*particle < k_f_0_25) {
            tmp_f = *particle;
          }
          frame_obj.sprite_size_u = tmp_f * k_f_2_5 + k_f_0_4;
          frame_obj.sprite_size_v = frame_obj.sprite_size_u;
          particle_batch_set_uv_a(*(undefined4 *)(batch + 0x10),&frame_obj.sprite_size_u);
          particle_batch_set_size(*(undefined4 *)(batch + 0x10),*(int *)&particle[4]);
          tmp_i = ((int)(*(float *)((char *)&particle[2] - 0x8) * 15.0f));
          particle_batch_set_uv_b_c
                    (*(undefined4 *)(batch + 0x10),&g_weapon_flamethrower_60db20 + tmp_i * 0x10,
                     &g_weapon_flamethrower_60db28 + tmp_i * 0x10);
          *aux_position = particle[-3] + *aux_position;
          *(int *)&particle[1] = *(int *)&particle[1] + 1;
          particle[-5] = particle[-2] + particle[-5];
          particle[-4] = particle[-1] + particle[-4];
        }
        particle = particle + 0xd;
        frame_obj.particles_left = frame_obj.particles_left + -1;
      } while (frame_obj.particles_left != 0);
      if (frame_obj.any_particle_alive != 0) {
        particle_batch_mark_current_active(*(undefined4 *)(batch + 0x10));
      }
      frame_obj.batch_sub = frame_obj.batch_sub + 1;
    } while (frame_obj.batch_sub < 5);
    frame_obj.flame_index = frame_obj.flame_index + 1;
    batch = batch + 0x68;
    fx_system_flush(*(undefined4 *)(frame_obj.batch_base + 0x10));
  } while ((int)batch < (int)&g_weapon_flamethrower_6a958c + 0x19c);
  return;
}

