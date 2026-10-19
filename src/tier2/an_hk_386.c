#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006c40a8*/ extern undefined4 g_particles_6c40a8;
/*006c40b0*/ extern undefined4 g_particles_6c40b0;
/*006c40b4*/ extern undefined4 g_particles_6c40b4;
/*006c40b8*/ extern undefined4 g_gfx_6c40b8;
#define PARTICLE_RECORD_SIZE (0x50) /* bytes per particle record (inferred from the mem_zero size multiplier) */
/* particle_pool_counts_reset */

/* "clears the pool record buffers and counters of every particle pool" */

/*
 * Clears every particle pool: zeroes each pool's record buffer and resets its
 * per-pool counter to 0. Takes no parameters and returns nothing.
 * Pool count is in g_particles_6c40b0; per-pool buffer pointers are in
 * g_particles_6c40a8[], per-pool record counts in g_particles_6c40b4[], and
 * per-pool counters in g_gfx_6c40b8[] (names inferred from usage).
 */
void particle_pool_counts_reset(void)

{
  int pool_count;
  int pool_index;

  pool_index = 0;
  /* loop over all pools; skipped when there are none */
  if (0 < (int)g_particles_6c40b0) {
    do {
      /* zero this pool's record buffer: record count * record size bytes */
      mem_zero(*(undefined4 *)(g_particles_6c40a8 + pool_index * 4),
                   *(int *)(g_particles_6c40b4 + pool_index * 4) * PARTICLE_RECORD_SIZE);
      pool_count = g_particles_6c40b0;
      /* reset this pool's counter */
      *(undefined4 *)(g_gfx_6c40b8 + pool_index * 4) = 0;
      pool_index = pool_index + 1;
    } while (pool_index < pool_count);
  }
  return;
}

