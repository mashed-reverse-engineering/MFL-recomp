#include "mfl_tier2.h"
/*004978d0*/ extern char *__cdecl fx_pool_alloc_particle(undefined4 p0);
/*005ddc28*/ extern float k_f_0;
/*005e07f0*/ extern float k_f_0_80999994;
/*00745ce8*/ extern char *g_particles_745ce8;
/*00745cec*/ extern char *g_particles_745cec;
/*00745cf0*/ extern char *g_particles_745cf0;
/*007519ac*/ extern float g_particles_7519ac;

#define EMITTER_SLOT_STRIDE (12)  /* bytes per emitter slot: last emission point (x, y, z) */
#define PARTICLE_TYPE_OFS (0x45)  /* byte offset of the particle type in a particle */
#define PARTICLE_TYPE_TRAIL (3)   /* type 3: white trail */
#define COLOUR_CHANNEL_FULL (0xff)
/* fx_trail_particle_spawn */

/* "spawns a trail particle (white, type 3), rate limited by distance to the last emission point of
   the emitter slot" */

/* Spawns one white trail particle at pos. emitter_slot selects a 12-byte slot holding the last
   emission point (0 = no rate limit). emit_gate <= k_f_0 (inferred: first/forced emission) stores
   pos as the new emission point and always spawns; otherwise the spawn is refused when pos is
   farther than sqrt(k_f_0_80999994) from that point (inferred). scale is stored in the particle
   (inferred: size). Returns the new particle, or NULL if the spawn was refused or the pool is empty. */

float * fx_trail_particle_spawn(float *pos,float emit_gate,float scale,int emitter_slot)

{
  float pos_y;
  float pos_z;
  float dx;
  float dz;
  float dy;
  float *particle;

  pos_y = pos[1];
  pos_z = pos[2];
  /* rate limit: compare pos with the slot's last emission point */
  if (emitter_slot != 0) {
    emitter_slot = emitter_slot * EMITTER_SLOT_STRIDE;
    dx = *(float *)((char *)&g_particles_745ce8 + emitter_slot) - *pos;
    dy = *(float *)((char *)&g_particles_745cec + emitter_slot) - pos_y;
    dz = *(float *)((char *)&g_particles_745cf0 + emitter_slot) - pos_z;
    if (!(emit_gate > k_f_0)) {      /* NaN resets the point (original: fcomp, test ah,0x41, jne) */
      *(float *)((char *)&g_particles_745ce8 + emitter_slot) = *pos;
      *(float *)((char *)&g_particles_745cec + emitter_slot) = pos_y;
      *(float *)((char *)&g_particles_745cf0 + emitter_slot) = pos_z;
    }
    else if (k_f_0_80999994 < dz * dz + dx * dx + dy * dy) {
      return (float *)0x0;
    }
  }
  /* allocate a particle from the pool and initialise it */
  particle = (float *)fx_pool_alloc_particle(0);
  if (particle == (float *)0x0) {
    return (float *)0x0;
  }
  particle[0xc] = 1.0;
  particle[0xe] = 1.0;
  particle[0xd] = 0.0;
  *(undefined1 *)((char *)particle + PARTICLE_TYPE_OFS) = PARTICLE_TYPE_TRAIL;
  *(int *)&particle[0x10] = 288;
  *particle = *pos;
  particle[1] = pos[1];
  pos_z = pos[2];
  particle[5] = 1.0;
  particle[4] = 1.0;
  particle[2] = pos_z;
  *(undefined1 *)((char *)particle + 0xf) = COLOUR_CHANNEL_FULL;
  *(undefined1 *)((char *)particle + 0xe) = COLOUR_CHANNEL_FULL;
  *(undefined1 *)((char *)particle + 0xd) = COLOUR_CHANNEL_FULL;
  *(undefined1 *)(particle + 3) = COLOUR_CHANNEL_FULL;
  /* pos_y is reused as a scratch float here: the value stored is g_particles_7519ac */
  pos_y = g_particles_7519ac;
  particle[6] = 0.0;
  particle[7] = 0.0;
  particle[8] = 0.0;
  particle[9] = 0.0;
  particle[10] = 0.0;
  particle[0xb] = 0.0;
  *(undefined1 *)(particle + 0x11) = 0;
  particle[0xf] = pos_y;
  particle[0x12] = scale;
  return particle;
}


