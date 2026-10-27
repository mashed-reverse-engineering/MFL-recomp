// cluster-entries: fx_ring_update_cb_499120
/* Installed cluster fx_ring_update_cb_499120 (tools/cluster_install.py from output/cluster_pilot_kb/fx_ring_update_cb_499120.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*0047b750*/ extern int car_get_speed();
/*0047b7c0*/ extern int car_set_field_144();
/*00481fd0*/ extern double random_range();
/*00482010*/ extern int random_int_range();
/*0049d060*/ extern int fx_spawn_burst_default_scale();
/*0049fd50*/ extern int fx_smoke_particle_spawn_tinted();
/*005dd374*/ extern float k_f_0_2;
/*005ddb18*/ extern float float_0_8;
/*005e64a4*/ extern float k_f_0;
/*005de19c*/ extern float k_f_1_0en4;
/*005df7b8*/ extern float k_f_n0_2;
/*007ef804*/ extern undefined4 g_tick_number;
/* fx_ring_update_cb_499120 */

/* function pointer pushed/stored by fx_ring_update (MOV dword ptr [ESP + 0xe0],0x499120) */

void fx_ring_update_cb_499120(RwV3d *hit_pos, int other_record)

{
  int object; /* record the trigger belongs to (pointer stored in a float slot of the trigger) */
  int roll;
  uint tick_mod4;
  int event_or_car;
  bool every_fourth_tick;
  float10 random_val;
  struct {
    undefined1 col_r;
    undefined1 col_g;
    undefined1 col_b;
    undefined1 col_a;
    float car_speed;
    undefined1 pad_18[12];
    float kick_x;
    float kick_y;
    float kick_z_or_scale;
  } frame;
  RwV3d spawn_pos; /* contiguous x, y, z: handed to the effect spawners */
  
  frame.col_r = 0x20;
  frame.col_g = 0x20;
  frame.col_b = 0x20;
  event_or_car = *(int *)(other_record + 0x10);
  frame.col_a = 0x80;
  if ((0xfeff < event_or_car) && (event_or_car < 0xff08)) {
    object = *(int *)&hit_pos[1].z;
    frame.car_speed = 0.0;
    event_or_car = event_or_car + -0xff00;
    if (!(*(float *)(object + 0x20) > float_0_8)) {
      car_get_speed(&frame.car_speed,event_or_car);
      frame.kick_x = 0.0;
      random_val = (float10)random_range(0xbf800000,0x3f800000);
      if (!((float10)k_f_0_2 > random_val) || !(random_val > k_f_0)) {
        if (((float10)k_f_n0_2 < random_val) && (random_val < k_f_0)) {
          random_val = (float10)k_f_n0_2;
        }
      }
      else {
        random_val = (float10)k_f_0_2;
      }
      frame.kick_z_or_scale = frame.car_speed * k_f_1_0en4;
      frame.kick_x = frame.kick_z_or_scale * frame.kick_x;
      frame.kick_y = (float)((float10)frame.kick_z_or_scale * random_val);
      frame.kick_z_or_scale = frame.kick_z_or_scale * k_f_0;
      car_set_field_144(&frame.kick_x,event_or_car);
      if (*(int *)(object + 0x6c) != 0) {
        roll = random_int_range(0,10);
        if (5 < roll) {
          car_damage_at_world_point(event_or_car,0x3c23d70a,hit_pos);
        }
        tick_mod4 = g_tick_number & 0x80000003;
        every_fourth_tick = tick_mod4 == 0;
        if ((int)tick_mod4 < 0) {
          every_fourth_tick = (tick_mod4 - 1 | 0xfffffffc) == 0xffffffff;
        }
        if (every_fourth_tick) {
          event_or_car = 2;
          do {
            spawn_pos.x = hit_pos->x;
            spawn_pos.y = hit_pos->y;
            spawn_pos.z = hit_pos->z;
            random_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            spawn_pos.x = (float)(random_val + (float10)spawn_pos.x);
            random_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            spawn_pos.y = (RwReal)(random_val + (float10)spawn_pos.y);
            random_val = (float10)random_range(0xbdcccccd,0x3dcccccd);
            spawn_pos.z = (RwReal)(random_val + (float10)spawn_pos.z);
            fx_spawn_burst_default_scale(&spawn_pos,5,0,0,0,0x40200000);
            fx_smoke_particle_spawn_tinted(&spawn_pos,0x3e4ccccd,&frame.col_r,0x40a00000,0x3f000000);
            event_or_car = event_or_car + -1;
          } while (event_or_car != 0);
        }
      }
    }
  }
  return;
}

