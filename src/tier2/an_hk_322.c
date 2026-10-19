#include "mfl_tier2.h"
/*00497750*/ extern int pool_stack_pop_index();
/*0072dfd8*/ extern undefined4 g_particles_72dfd8;
/*00745c48*/ extern undefined1 g_particles_745c48;
/*007519b8*/ extern undefined4 g_particles_7519b8;
#define PARTICLE_MAX_INDEX (0x4ff)
#define PARTICLE_RECORD_DWORDS (0x13)
/* fx_pool_alloc_particle */

/* Allocates one particle slot. Pops a free index (0..0x4FF) from the free-index stack
   g_particles_7519b8, marks it as used in the bitmap g_particles_745c48 (inferred: one bit per
   index), and returns a pointer to its 0x4C byte record in g_particles_72dfd8.
   Parameters: out_index (optional, may be NULL) receives the allocated index.
   Returns: the record pointer, or NULL if the pool is empty (pop returned < 0) or the index is
   out of range. */

undefined4 * __cdecl fx_pool_alloc_particle(int *out_index)

{
  int pool_index;
  undefined4 *record;

  pool_index = pool_stack_pop_index(g_particles_7519b8);
  if ((pool_index < 0) || (PARTICLE_MAX_INDEX < pool_index)) {
    record = (undefined4 *)0x0;
  }
  else {
    /* Record address is index * record size in dwords from the start of the record array. */
    record = &g_particles_72dfd8 + pool_index * PARTICLE_RECORD_DWORDS;
    /* Set this index's bit in the in-use bitmap (8 indices per byte). */
    (&g_particles_745c48)[pool_index >> 3] =
         (&g_particles_745c48)[pool_index >> 3] | '\x01' << ((byte)pool_index & 7);
    if (out_index != (int *)0x0) {
      *out_index = pool_index;
      return record;
    }
  }
  return record;
}


