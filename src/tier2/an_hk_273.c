#include "mfl_tier2.h"
/*004846b0*/ extern int alloc_zeroed();
/*00486f10*/ extern undefined __cdecl particle_system_setup_ptank(uint *p0, uint p1, uint p2, struct RwTexture *p3);
/*006c40a8*/ extern undefined4 g_particles_6c40a8;
/*006c40ac*/ extern char *g_particles_6c40ac;
/*006c40b0*/ extern undefined4 g_particles_6c40b0;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;

#define PARTICLE_POOL_ENTRY_SIZE (0x40) /* bytes per entry in the g_particles_6c40ac pool (inferred) */
#define PARTICLE_SETUP_ARG (0x80c) /* 2nd argument to particle_system_setup_ptank (inferred) */
#define PARTICLE_ALLOC_SIZE (0x50) /* element size passed to alloc_zeroed (inferred) */

/* particle_pool_add */

/* "(texture, count): particles_create(0x80c) + record allocation, g_particles_6c40b0++" */

/* Adds one particle pool slot for `texture` holding `count` particles: sets up the pool
   entry at the current slot (g_particles_6c40ac + index * PARTICLE_POOL_ENTRY_SIZE), allocates
   a zeroed block of `count` elements and stores its pointer and count in the parallel arrays
   g_particles_6c40a8 / g_particles_6c40b4 at the same slot, then advances g_particles_6c40b0.
   Returns nothing. */
void particle_pool_add(RwTexture *texture,uint count)

{
  undefined4 alloc_block;
  char *slot_offset;

  /* Set up the pool entry for this slot (inferred) */
  particle_system_setup_ptank
            ((uint *)(g_particles_6c40b0 * PARTICLE_POOL_ENTRY_SIZE + g_particles_6c40ac),PARTICLE_SETUP_ARG,count,texture);
  /* Byte offset of this slot in the 4-byte-per-slot arrays */
  slot_offset = g_particles_6c40b0 * 4;
  /* Allocate and zero the particle block, record its pointer and count (inferred) */
  alloc_block = alloc_zeroed(count,PARTICLE_ALLOC_SIZE);
  *(undefined4 *)(slot_offset + g_particles_6c40a8) = alloc_block;
  *(uint *)(slot_offset + g_particles_6c40b4) = count;
  g_particles_6c40b0 = g_particles_6c40b0 + 1;
  return;
}


