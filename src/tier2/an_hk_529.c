#include "mfl_tier2.h"
/*00497fa0*/ extern FxTrailParticle *fx_trail_particle_spawn();
/*005dd330*/ extern float float_1_0;
/* fx_smoke_particle_spawn_tinted */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "spawns a type 6 particle with a given colour and size+1" */

void fx_smoke_particle_spawn_tinted
               (undefined4 param_1,undefined4 param_2,undefined4 *param_3,char *unused_4,
               float param_5)

{
  undefined4 colour;
  FxTrailParticle *particle;
  
  particle = fx_trail_particle_spawn(param_1,0,0,0);
  /* the float fields are written as raw 32 bit words, as in the original (float stores compile differently) */
  param_5 = param_5 + float_1_0;
  *(undefined4 *)&particle->accel.x = 0;
  *(undefined4 *)&particle->accel.y = 0;
  *(undefined4 *)&particle->accel.z = 0;
  *(undefined4 *)&particle->velocity.x = 0;
  *(undefined4 *)&particle->velocity.z = 0;
  particle->kind = 6;
  *(undefined4 *)&particle->velocity.y = 0x3f800000;
  colour = *param_3;
  particle->scale = param_5;
  *(undefined4 *)particle->colour = colour;
  *(undefined4 *)&particle->size_end = param_2;
  *(undefined4 *)&particle->size_start = param_2;
  particle->flags |= 0x1000;
  return;
}
