// cluster-entries: event_log_dispatch
/* Installed cluster event_log_dispatch (tools/cluster_install.py from output/cluster_pilot_k/event_log_dispatch.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern struct EventLogArgs g_event_log_args[256];
extern struct EventLogData g_event_log_data[256];
/* the event log records hold floats at these offsets (the shared header types them as uint) */
#define EVLOG_F(index, offset) (*(float *)((char *)&g_event_log_data[index] + (offset)))
/*00421e20*/ extern int vehicle_set_active();
/*00421e60*/ extern int vehicle_set_flag_10();
/*004246f0*/ extern int car_set_damage();
/*00467890*/ extern int player_status_set();
/*004757a0*/ extern int get_viewport_camera_matrix();
/*0047ac90*/ extern int car_get_int_from_float_field();
/*0047b520*/ extern int car_current_matrix();
/*0047b6e0*/ extern int car_get_velocity();
/*00481fd0*/ extern double random_range();
/*00484b60*/ extern int __cdecl particle_spawn_at_weapon_pos(undefined4 p0, undefined4 p1);
/*004866d0*/ extern int fx_emitter_spawn_b_default();
/*0048e6f0*/ extern int rwp_object_get_matrix();
/*00499100*/ extern int fx_spawn_from_matrix_2();
/*00499b40*/ extern int explosion_decal();
/*0049d090*/ extern int fx_spawn_burst_spread();
/*0049d610*/ extern int fx_explosion_spawn_ring();
/*004a1680*/ extern int player_rumble();
/*004c45e0*/ extern float rp_random_float_range(undefined4 p0, undefined4 p1);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*004d53a0*/ extern RwReal RwV3dLength(struct RwV3d *p0);
/*005dd340*/ extern float k_f_0_5;
/*005ddaf0*/ extern float k_f_1_0en5;
/*005dde2c*/ extern float k_f_1300000;
/*005dde30*/ extern float k_f_2_5en5;
/*005dde34*/ extern float k_f_n0_001;
/*005dde38*/ extern float k_f_85;
/*005dde3c*/ extern float k_f_3000;
/*005dde40*/ extern float k_f_70;
/*005dde44*/ extern float k_f_750;
/*0062ac74*/ extern float g_car_62ac74;
/*00632ae8*/ extern undefined4 one;
/*007e7f64*/ extern undefined4 g_event_log_type;
/*007e8764*/ extern undefined4 g_event_log_param;
/*007ef778*/ extern float g_event_log_7ef778;
/*007ef77c*/ extern float g_event_log_7ef77c;
/* event_log_dispatch */

/* "(index): plays the effects and sounds for one event log entry (switch on the event type: 3 world
   collision, ...)" */

void event_log_dispatch_n(int log_index)

{
  float fbits1;
  float fbits2;
  float tmp_a;
  float tmp_b;
  int cam_matrix;
  uint car_or_count;
  float *pt_out;
  int car_index;
  float *pt_in;
  float10 rand_val;
  RwReal speed;
  struct {
    RwV3d vec_a;
    RwV3d vec_b;
    float burst_pos_x;
    float burst_pos_y;
    float burst_pos_z;
    float scalar_tmp;
    float scalar_tmp2;
    RwV3d burst_dir;
    undefined4 gravity_x;
    undefined4 gravity_y;
    undefined4 gravity_z;
    RwV3d fx_pos;
    undefined4 fx_pos_w;
    /* fx_mat.. : 4x4 matrix scratch, right row first (inferred); the 4-car case also reads fx_mat_x/y/z and mat_at_y/z/pad as two corner points */
    float fx_mat;
    float fx_mat_y;
    float fx_mat_z;
    uint fx_mat_flags;
    undefined4 mat_up_x;
    undefined4 mat_up_y;
    undefined4 mat_up_z;
    undefined1 mat_up_pad[4];
    undefined4 mat_at_x;
    float mat_at_y;
    float mat_at_z;
    float mat_at_pad;
    undefined4 mat_pos_x;
    undefined4 mat_pos_y;
    undefined4 mat_pos_z;
  } frame;
  
  /* car index = event parameter - 0xff00; gravity for the sparks is (0, -0.0002, 0) */
  car_index = (&g_event_log_param)[log_index] + -0xff00;
  frame.gravity_x = 0;
  frame.gravity_y = 0xb951b717;
  frame.gravity_z = 0;
  switch((&g_event_log_type)[log_index]) {
  /* world collision: sparks along the reflected velocity, and a skid / spark decal above 3000 */
  case 3:
    rand_val = (float10)random_range(0xbd4ccccd,0x3d4ccccd);
    frame.burst_pos_x = (float)(rand_val + (float10)g_event_log_data[log_index].w04);
    rand_val = (float10)random_range(0xbd4ccccd,0x3d4ccccd);
    frame.burst_pos_y = (float)(rand_val + EVLOG_F(log_index, 0x08));
    rand_val = (float10)random_range(0xbd4ccccd,0x3d4ccccd);
    frame.vec_b.x = EVLOG_F(log_index, 0x10);
    frame.vec_b.y = EVLOG_F(log_index, 0x14);
    frame.vec_b.z = EVLOG_F(log_index, 0x18);
    frame.burst_pos_z = (float)(rand_val + EVLOG_F(log_index, 0x0c));
    if (k_f_70 < g_event_log_data[log_index].w00) {
      RwV3dLength(&frame.vec_a);
      car_get_velocity(&frame.vec_a,car_index);
      if (k_f_1_0en5 < frame.vec_a.z * frame.vec_a.z + frame.vec_a.x * frame.vec_a.x + frame.vec_a.y * frame.vec_a.y)
      {
        RwV3dNormalize(&frame.burst_dir,&frame.vec_a);
      }
      if (k_f_1_0en5 < frame.vec_b.z * frame.vec_b.z + frame.vec_b.x * frame.vec_b.x + frame.vec_b.y * frame.vec_b.y)
      {
        RwV3dNormalize(&frame.vec_b,&frame.vec_b);
      }
      frame.burst_dir.x = frame.burst_dir.x + frame.vec_b.x;
      frame.burst_dir.y = frame.burst_dir.y + frame.vec_b.y;
      frame.burst_dir.z = frame.burst_dir.z + frame.vec_b.z;
      if (k_f_1_0en5 < frame.burst_dir.z * frame.burst_dir.z + frame.burst_dir.x * frame.burst_dir.x + frame.burst_dir.y * frame.burst_dir.y)
      {
        RwV3dNormalize(&frame.burst_dir,&frame.burst_dir);
      }
      frame.burst_dir.y = 1.0;
      fx_spawn_burst_spread(&frame.burst_pos_x,0xc,0x3c656042,&frame.burst_dir,0x41a00000,0,&frame.gravity_x,0x40200000);
    }
    if (k_f_3000 < g_event_log_data[log_index].w00) {
      frame.fx_pos.x = frame.burst_pos_x;
      frame.fx_pos.y = frame.burst_pos_y;
      frame.fx_mat_flags = frame.fx_mat_flags | 0x20003;
      frame.mat_at_z = 1.0;
      frame.mat_up_y = 0x3f800000;
      frame.fx_mat = 1.0;
      frame.mat_up_x = 0;
      frame.fx_mat_z = 0.0;
      frame.fx_mat_y = 0.0;
      frame.mat_at_y = 0.0;
      frame.mat_at_x = 0;
      frame.mat_up_z = 0;
      frame.mat_pos_z = 0;
      frame.mat_pos_y = 0;
      frame.mat_pos_x = 0;
      frame.fx_pos.z = frame.burst_pos_z;
      frame.fx_pos_w = 0x3f000000;
      car_index = particle_spawn_at_weapon_pos(&frame.fx_pos,&frame.fx_mat);
      if (car_index != 0) {
        fx_spawn_from_matrix_2(&frame.fx_mat,0x3f99999a);
        return;
      }
    }
    break;
  /* (inferred) car collision: burst scaled by speed, explosion decal for big impacts */
  case 4:
    rand_val = (float10)random_range(0,0x3f800000);
    if (rand_val <= (float10)k_f_0_5) {
      car_or_count = g_event_log_args[log_index].arg1;
    }
    else {
      car_or_count = g_event_log_args[log_index].arg0;
    }
    car_get_velocity(&frame.vec_a,car_or_count);
    RwV3dNormalize(&frame.burst_dir,&frame.vec_a);
    speed = RwV3dLength(&frame.vec_a);
    frame.scalar_tmp2 = speed * k_f_2_5en5;
    frame.burst_pos_x = g_event_log_data[log_index].w00;
    frame.burst_pos_y = g_event_log_data[log_index].w04;
    frame.burst_pos_z = EVLOG_F(log_index, 0x08);
    frame.burst_dir.y = 0.5;
    frame.gravity_x = 0;
    frame.gravity_y = 0xc0e00000;
    frame.gravity_z = 0;
    fx_spawn_burst_spread(&frame.burst_pos_x,0xc,*(int *)&frame.scalar_tmp2,&frame.burst_dir,0x40a00000,0,&frame.gravity_x,0x400ccccd);
    frame.vec_b.x = EVLOG_F(log_index, 0x24);
    frame.vec_b.y = EVLOG_F(log_index, 0x28);
    frame.vec_b.z = EVLOG_F(log_index, 0x2c);
    frame.fx_pos.x = EVLOG_F(log_index, 0x30);
    frame.fx_pos.y = EVLOG_F(log_index, 0x34);
    frame.fx_pos.z = EVLOG_F(log_index, 0x38);
    frame.scalar_tmp = RwV3dLength(&frame.vec_b);
    g_event_log_7ef77c = RwV3dLength(&frame.fx_pos);
    g_event_log_7ef778 = frame.scalar_tmp;
    if ((k_f_1300000 < frame.scalar_tmp) || (k_f_1300000 < g_event_log_7ef77c)) {
      explosion_decal(&frame.burst_pos_x,0x3f99999a);
      return;
    }
    break;
  /* (inferred) car state change on a player car: camera-relative emitter, flag cleared */
  case 5:
    frame.scalar_tmp2 = 0.0;
    *(int *)&frame.scalar_tmp = 4;
    car_get_int_from_float_field(&frame.scalar_tmp,car_index);
    if (2 < *(int *)&frame.scalar_tmp) {
      car_current_matrix(&frame.scalar_tmp2,car_index);
      frame.vec_a.x = *(float *)(*(int *)&frame.scalar_tmp2 + 0x30);
      frame.vec_a.y = *(float *)(*(int *)&frame.scalar_tmp2 + 0x34);
      frame.vec_a.z = *(float *)(*(int *)&frame.scalar_tmp2 + 0x38);
      cam_matrix = get_viewport_camera_matrix(0);
      frame.burst_pos_x = *(float *)(cam_matrix + 0x20);
      frame.burst_pos_y = *(float *)(cam_matrix + 0x24);
      frame.burst_pos_z = *(float *)(cam_matrix + 0x28);
      frame.vec_a.x = frame.vec_a.x - frame.burst_pos_x;
      frame.vec_a.y = frame.vec_a.y - frame.burst_pos_y;
      frame.vec_a.z = frame.vec_a.z - frame.burst_pos_z;
      rand_val = (float10)rp_random_float_range(0x3f800000,0x3fc00000);
      fx_emitter_spawn_b_default(&frame.vec_a,&g_car_62ac74,0x43b40000,(fbits1 = (float)rand_val, *(int *)&fbits1));
      vehicle_set_flag_10(car_index,1,0);
      return;
    }
    break;
  /* (inferred) by argument colour: 0xff0000ff = car wrecked (damage 50), 0xffff0080 = eliminated (ring, deactivate) */
  case 6:
    frame.scalar_tmp2 = 0.0;
    if (g_event_log_args[log_index].arg0 == 0xff0000ff) {
      car_current_matrix(&frame.scalar_tmp2,car_index);
      frame.vec_a.x = *(float *)(*(int *)&frame.scalar_tmp2 + 0x30);
      frame.vec_a.y = *(float *)(*(int *)&frame.scalar_tmp2 + 0x34);
      frame.vec_a.z = *(float *)(*(int *)&frame.scalar_tmp2 + 0x38);
      cam_matrix = get_viewport_camera_matrix(0);
      frame.burst_pos_x = *(float *)(cam_matrix + 0x20);
      frame.burst_pos_y = *(float *)(cam_matrix + 0x24);
      frame.burst_pos_z = *(float *)(cam_matrix + 0x28);
      frame.vec_a.x = frame.vec_a.x - frame.burst_pos_x;
      frame.vec_a.y = frame.vec_a.y - frame.burst_pos_y;
      frame.vec_a.z = frame.vec_a.z - frame.burst_pos_z;
      rand_val = (float10)rp_random_float_range(0x3f800000,0x3fc00000);
      fx_emitter_spawn_b_default(&frame.vec_a,&g_car_62ac74,0x43b40000,(fbits2 = (float)rand_val, *(int *)&fbits2));
      car_set_damage(car_index,0x42480000,0);
      car_set_damage(car_index,0x42480000,1);
    }
    if (g_event_log_args[log_index].arg0 == 0xffff0080) {
      frame.fx_pos.x = g_event_log_data[log_index].w00;
      frame.fx_pos.y = g_event_log_data[log_index].w04;
      frame.fx_pos.z = EVLOG_F(log_index, 0x08);
      fx_explosion_spawn_ring(&frame.fx_pos,0x400ccccd,2);
      vehicle_set_active(car_index,0);
      player_status_set(car_index,2);
    }
    break;
  case 7:
  case 9:
  case 10:
  case 0xb:
    break;
  /* (inferred) rwp object hit: rumble or burst above 750 */
  case 8:
    car_or_count = g_event_log_args[log_index].arg2;
    frame.scalar_tmp = g_event_log_data[log_index].w04;
    rwp_object_get_matrix(g_event_log_args[log_index].arg1);
    frame.vec_a.x = EVLOG_F(log_index, 0x24);
    frame.vec_a.y = EVLOG_F(log_index, 0x28);
    frame.vec_a.z = EVLOG_F(log_index, 0x2c);
    frame.burst_pos_x = EVLOG_F(log_index, 0x30);
    frame.burst_pos_y = EVLOG_F(log_index, 0x34);
    frame.burst_pos_z = EVLOG_F(log_index, 0x38);
    if (car_or_count == 0) {
      if (k_f_750 < frame.scalar_tmp) {
        player_rumble(car_index,1,2,0xff);
        return;
      }
    }
    else if ((car_or_count == 3) && (k_f_750 < frame.scalar_tmp)) {
      fx_spawn_burst_spread(&frame.vec_a,10,0x3c656042,&frame.burst_pos_x,0x42480000,0,&frame.gravity_x,0x40200000);
      return;
    }
    break;
  /* multi-point impact (arg0 points): burst at every point and between neighbouring points */
  case 0xc:
    car_or_count = 0;
    if (!(k_f_85 > EVLOG_F(log_index, 0x38) * EVLOG_F(log_index, 0x38) +
                  EVLOG_F(log_index, 0x30) * EVLOG_F(log_index, 0x30) +
                  EVLOG_F(log_index, 0x34) * EVLOG_F(log_index, 0x34))) {
      frame.burst_pos_x = (float)(int)g_event_log_args[log_index].arg2 * k_f_n0_001;
      frame.burst_pos_y = (float)(int)g_event_log_args[log_index].w0c * k_f_n0_001;
      frame.burst_pos_z = (float)(int)g_event_log_args[log_index].w10 * k_f_n0_001;
      if (g_event_log_args[log_index].arg0 != 0) {
        pt_out = &frame.fx_mat;
        pt_in = &g_event_log_data[log_index].w04;
        do {
          tmp_a = *pt_in;
          tmp_b = pt_in[1];
          *pt_out = ((EventLogData *)(pt_in + -1))->w00;
          pt_out[1] = tmp_a;
          pt_out[2] = tmp_b;
          fx_spawn_burst_spread(pt_out,2,0x3ccccccd,&frame.burst_pos_x,0x41000000,0,&frame.gravity_x,0x40200000);
          if (0 < (int)car_or_count) {
            frame.vec_a.x = pt_out[-3] - *pt_out;
            frame.vec_a.y = pt_out[-2] - pt_out[1];
            frame.vec_a.z = pt_out[-1] - pt_out[2];
            rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
            frame.vec_b.x = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.x +
                                 (float10)*pt_out);
            rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
            frame.vec_b.y = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.y +
                                 (float10)pt_out[1]);
            rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
            frame.vec_b.z = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.z +
                                 (float10)pt_out[2]);
            rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
            frame.vec_b.x = (RwReal)(rand_val + (float10)frame.vec_b.x);
            rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
            frame.vec_b.y = (RwReal)(rand_val + (float10)frame.vec_b.y);
            rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
            frame.vec_b.z = (RwReal)(rand_val + (float10)frame.vec_b.z);
            fx_spawn_burst_spread
                      (&frame.vec_b,2,0x3ccccccd,&frame.burst_pos_x,0x41c80000,0,&frame.gravity_x,0x40200000);
          }
          car_or_count = car_or_count + 1;
          pt_in = pt_in + 3;
          pt_out = pt_out + 3;
        } while (car_or_count < g_event_log_args[log_index].arg0);
      }
      if (g_event_log_args[log_index].arg0 == 4) {
        frame.vec_a.x = frame.mat_at_y - frame.fx_mat;
        frame.vec_a.y = frame.mat_at_z - frame.fx_mat_y;
        frame.vec_a.z = frame.mat_at_pad - frame.fx_mat_z;
        rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
        frame.vec_b.x = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.x + (float10)frame.fx_mat);
        rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
        frame.vec_b.y = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.y + (float10)frame.fx_mat_y);
        rand_val = (float10)rp_random_float_range(0xbe99999a,0x3e99999a);
        frame.vec_b.z = (RwReal)((rand_val + (float10)k_f_0_5) * (float10)frame.vec_a.z + (float10)frame.fx_mat_z);
        rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
        frame.vec_b.x = (RwReal)(rand_val + (float10)frame.vec_b.x);
        rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
        frame.vec_b.y = (RwReal)(rand_val + (float10)frame.vec_b.y);
        rand_val = (float10)rp_random_float_range(0xbdcccccd,0x3dcccccd);
        frame.vec_b.z = (RwReal)(rand_val + (float10)frame.vec_b.z);
        fx_spawn_burst_spread(&frame.vec_b,2,0x3ca3d70a,&frame.burst_pos_x,0x41c80000,0,&frame.gravity_x,0x40200000);
        return;
      }
    }
    break;
  default:
    goto switchD_0040d3e4_default;
  }
switchD_0040d3e4_default:
  return;
}

/* entry thunk at the original address of event_log_dispatch: register inputs (eax) -> natural C event_log_dispatch_n; ret 0 */
__declspec(naked) void event_log_dispatch(void)
{
    __asm {
        push eax
        call event_log_dispatch_n
        add esp, 4
        ret
    }
}
