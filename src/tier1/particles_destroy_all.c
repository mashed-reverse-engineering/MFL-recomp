#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006c40a4*/ extern undefined4 g_particles_6c40a4;
/*006c40a8*/ extern char *g_particles_6c40a8;
/*006c40ac*/ extern undefined4 g_particles_6c40ac;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;
/*007d2128*/ extern struct RwGlobalsRelease *RwEngineInstance;

#define PARTICLE_BLOCK_STRIDE 0x40 /* bytes per particle entry in g_particles_6c40ac */


/* particles_destroy_all */

/* "destroys and frees each particle block" */

/* Frees every particle block, then the shared particle, handle and gfx buffers, and clears their globals. */
void particles_destroy_all(void)
{
  int handle;
  int block_index;
  int block_offset;
  
  block_index = 0;
  /* g_particles_6c40a4 = particle block count */
  if (0 < ((int)g_particles_6c40a4)) {
    block_offset = 0;
    do {
      /* g_particles_6c40ac = particle entry array, one per block */
      if (g_particles_6c40ac + block_offset != 0) {
        particles_destroy(g_particles_6c40ac + block_offset);
      }
      /* g_particles_6c40a8 = per-block handle array; 0x10c = RwEngine vtable slot (used as free, inferred from the zeroing below) */
      handle = *(int *)(g_particles_6c40a8 + block_index * 4);
      if (handle != 0) {
        RwFree(handle);
      }
      block_index = block_index + 1;
      block_offset = block_offset + PARTICLE_BLOCK_STRIDE;
    } while (block_index < ((int)g_particles_6c40a4));
  }
  /* free the shared buffers; g_particles_6c40b4 and g_gfx_6c40b8 purpose unknown */
  RwFree(g_particles_6c40ac);
  RwFree(g_particles_6c40a8);
  RwFree(g_particles_6c40b4);
  RwFree(g_gfx_6c40b8);
  g_particles_6c40ac = 0;
  g_particles_6c40a8 = 0;
  g_particles_6c40b4 = 0;
  g_gfx_6c40b8 = 0;
  return;
}
