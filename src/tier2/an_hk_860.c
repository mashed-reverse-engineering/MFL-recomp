#include "mfl_tier2.h"
/*00472340*/ extern int sfx_play_player_set_at();
/*004726a0*/ extern int sfx_play_at();
/*00481fd0*/ extern double random_range();
/*004866b0*/ extern int fx_emitter_spawn_a_default();
/*0049cd60*/ extern undefined fx_spawn_burst(float *p1, int p2, float *p3, undefined4 *p4, undefined4 *p5, float p6);
/*004d5290*/ extern RwReal RwV3dNormalize(struct RwV3d *p0, struct RwV3d *p1);
/*005ddafc*/ extern float k_f_0_05;
/*0062ac74*/ extern float g_car_62ac74;

#define SPAWN_DIR_COUNT (3)
#define SPAWN_PARTICLES_PER_DIR (5)
#define DIR_COMPONENT_MIN (0xbf800000) /* float -1.0 */
#define DIR_COMPONENT_MAX (0x3f800000) /* float 1.0 */
#define SPAWN_POS_JITTER_MIN (0xbecccccd) /* float -0.4 */
#define SPAWN_POS_JITTER_MAX (0x3ecccccd) /* float 0.4 */
#define BURST_UP_SPEED_MIN (0x3dcccccd) /* float 0.1 (inferred) */
#define BURST_UP_SPEED_MAX (0x3e4ccccd) /* float 0.2 (inferred) */

/* course_effect_spawn_at */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "(object): shaking + sfx 0x12 / 0x15 at the object position" */
/* Effect for object `obj` (struct base pointer; its position is at obj+0x3c). Shakes it (func_0x0045c8f0, inferred),
   plays sfx 0x12 and 0x15 and the player-set sfx at the position, and spawns the default emitter at obj+0x48.
   Then it writes SPAWN_DIR_COUNT random unit directions into the RwV3d array at obj+0x18 and, for each, spawns
   SPAWN_PARTICLES_PER_DIR burst particles around the position (dir_vec[3]). Parameters 2-5 are unused.
   Returns nothing. */

void __cdecl course_effect_spawn_at(int obj,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5)

{
  /* iVar1: position pointer (obj+0x3c) first, then the particle loop counter */
  int iVar1;
  /* current direction vector; dir_vec[3] is the object position */
  RwV3d *dir_vec;
  double rnd;
  struct {
    undefined1 col[4];
    int cnt;
    RwV3d pos;
    RwV3d v24;
    undefined4 accel[3];
    float dir[3];
  } frame;
  iVar1 = obj + 0x3c;
  func_0x0045c8f0(iVar1,0x41f00000);
  sfx_play_at(0x12,iVar1);
  sfx_play_at(0x15,iVar1);
  sfx_play_player_set_at(iVar1);
  fx_emitter_spawn_a_default(obj + 0x48,&g_car_62ac74,0x43b40000);
  dir_vec = (RwV3d *)(obj + 0x18);
  frame.cnt = SPAWN_DIR_COUNT;
  /* One pass per direction: pick a random unit direction, then emit the particles for it */
  do {
    *(undefined4 *)(obj + 4) = 0x40000000;
    rnd = (double)random_range(DIR_COMPONENT_MIN,DIR_COMPONENT_MAX);
    dir_vec->x = (float)rnd;
    dir_vec->y = 1.0;
    rnd = (double)random_range(DIR_COMPONENT_MIN,DIR_COMPONENT_MAX);
    dir_vec->z = (float)rnd;
    RwV3dNormalize(dir_vec,dir_vec);
    iVar1 = SPAWN_PARTICLES_PER_DIR;
    /* Inner loop: one particle per pass, jittered around the position, moving along its own direction */
    do {
      frame.pos.x = dir_vec[3].x;
      frame.pos.y = dir_vec[3].y;
      frame.pos.z = dir_vec[3].z;
      frame.v24.x = 0.0;
      frame.v24.y = 1.0;
      frame.v24.z = 0.0;
      frame.accel[0] = 0;
      frame.accel[1] = 0xbb83126f;
      frame.accel[2] = 0;
      frame.col[0] = 0xff;
      frame.col[1] = 0x58;
      frame.col[2] = 0xf;
      frame.col[3] = 0xff;
      rnd = (double)random_range(SPAWN_POS_JITTER_MIN,SPAWN_POS_JITTER_MAX);
      frame.pos.x = (float)(rnd + (double)frame.pos.x);
      rnd = (double)random_range(SPAWN_POS_JITTER_MIN,SPAWN_POS_JITTER_MAX);
      frame.pos.y = (RwReal)(rnd + (double)frame.pos.y);
      rnd = (double)random_range(SPAWN_POS_JITTER_MIN,SPAWN_POS_JITTER_MAX);
      frame.pos.z = (RwReal)(rnd + (double)frame.pos.z);
      frame.v24.x = frame.pos.x - dir_vec[3].x;
      frame.v24.y = frame.pos.y - dir_vec[3].y;
      frame.v24.z = frame.pos.z - dir_vec[3].z;
      RwV3dNormalize(&frame.v24,&frame.v24);
      frame.dir[0] = frame.v24.x * k_f_0_05;
      frame.dir[1] = frame.v24.y * k_f_0_05;
      frame.dir[2] = frame.v24.z * k_f_0_05;
      rnd = (double)random_range(BURST_UP_SPEED_MIN,BURST_UP_SPEED_MAX);
      frame.dir[1] = (float)rnd;
      random_range(0x3f000000,0x3f800000);
      fx_spawn_burst(&frame.pos.x,1,&frame.dir[0],(undefined4 *)&frame.col[0],&frame.accel[0],0.2);
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    dir_vec = dir_vec + 1;
    frame.cnt = frame.cnt + -1;
  } while (frame.cnt != 0);
  return;
}


