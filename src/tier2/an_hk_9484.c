#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004c4c70*/ extern int FUN_004c4c70(int p0, float *p1, float *p2);
/*004c51a0*/ extern void rwmatrix_from_three_points(undefined4 p0, void *p1, float *p2);
/*005de490*/ extern float k_f_0_02;
/* particle_spawn_at_weapon_pos */

/* "spawns at weapon pos offset -hit_distance along the direction" */

undefined4 __cdecl particle_spawn_at_weapon_pos(float *pos,undefined4 out_matrix)

{
  undefined4 unused_ret;
  int hit;
  struct {
    float spawn_x;
    float spawn_y;
    float spawn_z;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_34[4];
    undefined1 hit_info [40];
    float hit_distance;
    undefined1 pad_4[4];
  } frame;
  
  hit = FUN_004c4c70(weapon_get_rw_801b14(),pos,&frame.hit_normal_x);
  if (hit != 0) {
    double s = -frame.hit_distance;
    double x = frame.hit_normal_x * s;
    double y;
    *(volatile float *)&frame.spawn_y = frame.hit_normal_y * s;
    *(volatile float *)&frame.spawn_z = frame.hit_normal_z * s;
    x = x + *pos;
    y = pos[1] + *(volatile float *)&frame.spawn_y;
    *(volatile float *)&frame.spawn_z = pos[2] + *(volatile float *)&frame.spawn_z;
    frame.spawn_x = k_f_0_02 * frame.hit_normal_x + x;
    frame.spawn_y = k_f_0_02 * frame.hit_normal_y + y;
    frame.spawn_z = k_f_0_02 * frame.hit_normal_z + *(volatile float *)&frame.spawn_z;
    rwmatrix_from_three_points(out_matrix,frame.hit_info,&frame.spawn_x);
    return 1;
  }
  return 0;
}
