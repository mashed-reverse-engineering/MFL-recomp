#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006c40c0*/ extern undefined g_particles_6c40c0;
/*006c5000*/ extern undefined g_particles_6c5000;
/*006c5080*/ extern undefined g_particles_6c5080;
/*006c4140*/ extern char g_gfx_6c4140[];
#define PARTICLE_SYSTEM_STRIDE 0x40
#define PARTICLE_SYSTEMS_6C40C0_END ((int)&g_gfx_6c4140)
#define PARTICLE_SYSTEMS_6C5000_END ((int)&g_particles_6c5080)


/* particle_systems_destroy_all */

/* Destroys all active particle systems across global particle pools and the standalone system. */

void particle_systems_destroy_all(void)
{
  /* unknown particle system struct */
  undefined *particle_system;
  
  /* Destroy systems in the first particle pool (0x6c40c0) */
  particle_system = &g_particles_6c40c0;
  do {
    particles_destroy(particle_system);
    particle_system = particle_system + PARTICLE_SYSTEM_STRIDE;
  } while ((int)particle_system < PARTICLE_SYSTEMS_6C40C0_END);

  /* Destroy systems in the second particle pool (0x6c5000) */
  particle_system = &g_particles_6c5000;
  do {
    particles_destroy(particle_system);
    particle_system = particle_system + PARTICLE_SYSTEM_STRIDE;
  } while ((int)particle_system < PARTICLE_SYSTEMS_6C5000_END);

  /* Destroy standalone particle system at 0x6c5080 */
  particles_destroy(&g_particles_6c5080);
  return;
}
