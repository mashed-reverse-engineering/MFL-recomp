#include "mfl_tier2.h"
/*00497fa0*/ extern FxTrailParticle *fx_trail_particle_spawn();
/* fx_smoke_particle_spawn_grey */

/* "spawns a grey (0x20) type 6 particle with scale 1.4" */

void __cdecl
fx_smoke_particle_spawn_grey
          (undefined4 param_1,undefined4 param_2,char *unused_3,char *unused_4,
          char *unused_5,char *unused_6,char *unused_7)

{
  FxTrailParticle *particle;
  
  particle = fx_trail_particle_spawn(param_1,0,0,0);
  /* the float fields are written as raw 32 bit words, as in the original (float stores compile differently) */
  particle->colour[0] = 0x20;
  particle->colour[1] = 0x20;
  particle->colour[2] = 0x20;
  *(undefined4 *)&particle->size_end = param_2;
  *(undefined4 *)&particle->accel.x = 0;
  *(undefined4 *)&particle->accel.y = 0;
  *(undefined4 *)&particle->accel.z = 0;
  *(undefined4 *)&particle->velocity.x = 0;
  *(undefined4 *)&particle->velocity.z = 0;
  particle->kind = 6;
  *(undefined4 *)&particle->velocity.y = 0x40000000;
  particle->colour[3] = 0xff;
  *(undefined4 *)&particle->scale = 0x3fb33333;
  *(undefined4 *)&particle->size_start = param_2;
  particle->flags |= 0x1000;
  return;
}
