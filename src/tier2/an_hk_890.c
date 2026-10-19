#include "mfl_tier2.h"
/*00467ea0*/ extern int weapon_get_rw_801b14();
/*004c4720*/ extern void FUN_004c4720(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3);
/*004c4de0*/ extern int world_intersect_line(RpWorld * p0, undefined4 p1, undefined4 p2);
/*004c51a0*/ extern void FUN_004c51a0(undefined4 p0, undefined4 p1, undefined4 p2);
/*005ddafc*/ extern float k_f_0_05;

/* byte offset of the second RwV3d (direction, inferred) inside the weapon block at weapon_pos */
#define WEAPON_DIR_OFFSET (0xc)

/* rocket_hit_spawn_particle */

/* Spawns one particle where a weapon shot hits the world.
 * Traces a line from weapon_pos (pointer to the weapon position vector, inferred) against the
 * world returned by weapon_get_rw_801b14(). On a hit, the impact point (from FUN_004c4720) is
 * moved k_f_0_05 (0.05 per the old comment, float bits unchecked) off the surface along the hit normal, and FUN_004c51a0 spawns the
 * particle on particle_system (inferred). unused_arg_3 and unused_arg_4 are not used.
 * Returns 1 if a particle was spawned, 0 if there was no hit or an argument was NULL.
 * (was: "particle at the weapon position along the rw_801b14 direction *0.05")
 */

undefined4 __cdecl rocket_hit_spawn_particle(int particle_system,int weapon_pos,char *unused_arg_3,char *unused_arg_4)

{
  undefined4 world;
  int hit_result;
  float *hit_normal;
  struct {
    float spawn_pos_x;
    float spawn_pos_y;
    float spawn_pos_z;
    float hit_normal_x;
    float hit_normal_y;
    float hit_normal_z;
    undefined1 pad_34[4];
    undefined1 particle_params [40];
    undefined4 unused_word;
    int pad_top;
  } frame;

  if ((particle_system != 0) && (weapon_pos != 0)) {
    /* Trace the shot line; on a hit, hit_normal receives the surface normal (inferred) */
    hit_normal = &frame.hit_normal_x;
    hit_result = weapon_pos;
    hit_result = world_intersect_line((RpWorld *)weapon_get_rw_801b14(),hit_result,(undefined4)hit_normal);
    /* Hit: start from the impact point and step 0.05 off the surface along the normal */
    if (hit_result != 0) {
      FUN_004c4720((undefined4)&frame.spawn_pos_x,weapon_pos,weapon_pos + WEAPON_DIR_OFFSET,(undefined4)frame.unused_word);
      frame.spawn_pos_x = frame.hit_normal_x * 0.05f + frame.spawn_pos_x;
      frame.spawn_pos_y = frame.hit_normal_y * 0.05f + frame.spawn_pos_y;
      frame.spawn_pos_z = frame.hit_normal_z * 0.05f + frame.spawn_pos_z;
      FUN_004c51a0(particle_system,(undefined4)frame.particle_params,(undefined4)&frame.spawn_pos_x);
      return 1;
    }
  }
  return 0;
}


