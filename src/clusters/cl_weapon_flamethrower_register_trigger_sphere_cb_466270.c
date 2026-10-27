// cluster-entries: weapon_flamethrower_register_trigger_sphere_cb_466270
/* Installed cluster weapon_flamethrower_register_trigger_sphere_cb_466270 (tools/cluster_install.py from output/cluster_pilot_kb/weapon_flamethrower_register_trigger_sphere_cb_466270.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
extern char s_333_Lap_frames_005df368[];
/*00424760*/ extern undefined __cdecl car_damage_at_world_point(undefined4 p0, undefined4 p1, struct RwV3d *p2);
/*00437f60*/ extern int screen_shake_add();
/*0047a4b0*/ extern int car_set_field_set();
/*0047a5b0*/ extern int car_add_velocity();
/*0047b6e0*/ extern int car_get_velocity();
/*00481fd0*/ extern double random_range();
/*004989a0*/ extern int fx_explosion_spawn();
/*005dd330*/ extern float float_1_0;
/*005de1b0*/ extern float k_f_n0_05;
/*007ef748*/ extern float g_data_7ef748;
/*007ef74c*/ extern float g_vehicle_render_7ef74c;
/* weapon_flamethrower_register_trigger_sphere_cb_466270 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* function pointer pushed/stored by weapon_flamethrower_register_trigger_sphere (MOV dword ptr [ESP
   + 0x20],0x466270) */

void weapon_flamethrower_register_trigger_sphere_cb_466270(RwV3d *trigger, int hit)

{
  int car;
  int event_id;
  float10 rand_val;
  struct {
    float vel_x;
    float vel_y;
    float vel_z;
  } frame;
  
  event_id = *(int *)(hit + 0x10);
  if ((((0xfeff < event_id) && (event_id < 0xff08)) &&
      (car = event_id + -0xff00, *(int *)&trigger[1].y == 16)) && (*(int *)&trigger[2].x != car))
  {
    rand_val = (float10)random_range(0,0x3f800000);
    if ((float10)*(float *)((char *)&s_333_Lap_frames_005df368 + 0) < rand_val) {
      car_get_velocity(&frame.vel_x,car);
      frame.vel_x = frame.vel_x * k_f_n0_05;
      frame.vel_y = frame.vel_y * k_f_n0_05;
      frame.vel_z = frame.vel_z * k_f_n0_05;
      car_add_velocity(&frame.vel_x,car);
    }
    car_damage_at_world_point(car,0x3eb33333,trigger);
    screen_shake_add(1);
    g_data_7ef748 = g_data_7ef748 + float_1_0;
    g_vehicle_render_7ef74c = g_vehicle_render_7ef74c + float_1_0;
    car_set_field_set(car,6,1,0x463b8000);
    fx_explosion_spawn(car,3,0x40800000,0x3e99999a);
    fx_explosion_spawn(car,2,0x40800000,0x3e99999a);
    fx_explosion_spawn(car,1,0x40800000,0x3e99999a);
    fx_explosion_spawn(car,0,0x40800000,0x3e99999a);
  }
  return;
}

