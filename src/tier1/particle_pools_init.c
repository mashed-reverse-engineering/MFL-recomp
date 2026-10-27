#include "mfl_tier2.h"

enum {
  PARTICLE_STRUCT_SIZE = 0x40,
  POOL_ENTRY_SIZE = 4
};

/*004846b0*/ extern int alloc_zeroed();
/*006c40a4*/ extern undefined4 g_particles_6c40a4;
/*006c40a8*/ extern undefined4 g_particles_6c40a8;
/*006c40ac*/ extern undefined4 g_particles_6c40ac;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;


/* particle_pools_init */

/* Initializes particle pool capacity and allocates zeroed buffers for particle and graphics tables. */
/* "(count): allocates the pool tables g_particles_6c40ac / a8 / b4" */

void particle_pools_init(int count)
{
  g_particles_6c40a4 = count;
  /* Allocate particle structs and auxiliary tables (unknown entry types) */
  g_particles_6c40ac = alloc_zeroed(count,PARTICLE_STRUCT_SIZE);
  g_particles_6c40a8 = alloc_zeroed(count,POOL_ENTRY_SIZE);
  g_particles_6c40b4 = alloc_zeroed(count,POOL_ENTRY_SIZE);
  g_gfx_6c40b8 = alloc_zeroed(count,POOL_ENTRY_SIZE);
  return;
}
