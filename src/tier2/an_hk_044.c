#include "mfl_tier2.h"
/*006c50c8*/ extern float g_gfx_6c50c8;
/*006c50cc*/ extern undefined4 g_particles_6c50cc;
/*006c50d0*/ extern undefined4 g_particles_6c50d0;
/*006c50d8*/ extern undefined4 g_gfx_6c50d8;
/*006c5118*/ extern undefined4 g_particles_6c5118;
/*006c511c*/ extern undefined4 g_particles_6c511c;
/*006c5120*/ extern undefined4 g_particles_6c5120;
/*006c5124*/ extern undefined4 g_particles_6c5124;
/*006c5144*/ extern undefined4 g_particles_6c5144;
/*006c5188*/ extern undefined4 g_gfx_6c5188;
/*006c518c*/ extern undefined4 g_gfx_6c518c;
/*006c5190*/ extern undefined4 g_gfx_6c5190;
/*006c5194*/ extern undefined4 g_gfx_6c5194;

/* Byte offsets of the fields of the particle buffer header (struct passed as `particles`) */
#define PARTICLES_CAPACITY_OFS (0x8)
#define PARTICLES_COUNT_OFS (0xc)
#define PARTICLES_POS_ARRAY_OFS (0x10)
#define PARTICLES_VEC2_ARRAY_OFS (0x14) /* (inferred) two dwords per particle */
#define PARTICLES_SCALAR_ARRAY_OFS (0x18)
#define PARTICLES_MATRIX_ARRAY_OFS (0x1c) /* (inferred) 4x4 matrix per particle */
#define PARTICLES_SIZE_ARRAY_OFS (0x20)
#define PARTICLES_COLOUR_ARRAY_OFS (0x24)
#define PARTICLES_BLOCK_ARRAY_OFS (0x28) /* (inferred) eight dwords per particle */

/* Per-particle element sizes in bytes */
#define PARTICLE_POS_STRIDE (0xc)
#define PARTICLE_VEC2_STRIDE (8)
#define PARTICLE_MATRIX_STRIDE (0x40)
#define PARTICLE_COLOUR_STRIDE (0x10)
#define PARTICLE_BLOCK_STRIDE (0x20)

/* Dwords copied per particle into the matrix and block arrays */
#define PARTICLE_MATRIX_DWORDS (0x10)
#define PARTICLE_BLOCK_DWORDS (8)


/* particles_add */

/* "appends a particle with the current emitter state (position, size, colour, matrix, ...)"
 * Appends one particle at index `count` of the buffer `particles` (count < capacity, else nothing).
 * Each parallel array that is present (non-NULL) receives the current emitter value for that slot,
 * then count is incremented. Returns nothing. */

void particles_add(char *particles)

{
  int index;
  int scratch;
  undefined4 *cursor;
  undefined4 *dst;

  /* only append while the buffer has room */
  if (*(int *)(particles + PARTICLES_COUNT_OFS) < *(int *)(particles + PARTICLES_CAPACITY_OFS)) {
    /* position (vec3) */
    if (*(int *)(particles + PARTICLES_POS_ARRAY_OFS) != 0) {
      cursor = (undefined4 *)(*(char **)(particles + PARTICLES_POS_ARRAY_OFS) + *(int *)(particles + PARTICLES_COUNT_OFS) * PARTICLE_POS_STRIDE);
      *cursor = g_particles_6c5118;
      cursor[1] = g_particles_6c511c;
      cursor[2] = g_particles_6c5120;
    }
    /* two-dword entry; scratch holds the array base, then is reused as the copy countdown below */
    scratch = *(int *)(particles + PARTICLES_VEC2_ARRAY_OFS);
    if (scratch != 0) {
      index = *(int *)(particles + PARTICLES_COUNT_OFS);
      *(undefined4 *)(scratch + index * PARTICLE_VEC2_STRIDE) = g_particles_6c50cc;
      *(undefined4 *)(scratch + 4 + index * PARTICLE_VEC2_STRIDE) = g_particles_6c50d0;
    }
    /* one dword per particle */
    if (*(int *)(particles + PARTICLES_SCALAR_ARRAY_OFS) != 0) {
      *(undefined4 *)(*(char **)(particles + PARTICLES_SCALAR_ARRAY_OFS) + *(int *)(particles + PARTICLES_COUNT_OFS) * 4) = g_particles_6c5144;
    }
    /* 4x4 matrix (16 dwords) copied from the current emitter matrix */
    if (*(int *)(particles + PARTICLES_MATRIX_ARRAY_OFS) != 0) {
      cursor = &g_gfx_6c50d8;
      dst = (undefined4 *)(*(int *)(particles + PARTICLES_COUNT_OFS) * PARTICLE_MATRIX_STRIDE + *(char **)(particles + PARTICLES_MATRIX_ARRAY_OFS));
      for (scratch = PARTICLE_MATRIX_DWORDS; scratch != 0; scratch = scratch + -1) {
        *dst = *cursor;
        cursor = cursor + 1;
        dst = dst + 1;
      }
    }
    /* size (one float) */
    if (*(int *)(particles + PARTICLES_SIZE_ARRAY_OFS) != 0) {
      *(undefined4 *)(*(char **)(particles + PARTICLES_SIZE_ARRAY_OFS) + *(int *)(particles + PARTICLES_COUNT_OFS) * 4) = *(undefined4 *)&g_gfx_6c50c8;
    }
    /* colour (four dwords) */
    if (*(int *)(particles + PARTICLES_COLOUR_ARRAY_OFS) != 0) {
      cursor = (undefined4 *)(*(int *)(particles + PARTICLES_COUNT_OFS) * PARTICLE_COLOUR_STRIDE + *(char **)(particles + PARTICLES_COLOUR_ARRAY_OFS));
      *cursor = g_gfx_6c5188;
      cursor[1] = g_gfx_6c518c;
      cursor[2] = g_gfx_6c5190;
      cursor[3] = g_gfx_6c5194;
    }
    /* eight-dword block copied from the current emitter block */
    if (*(int *)(particles + PARTICLES_BLOCK_ARRAY_OFS) != 0) {
      cursor = &g_particles_6c5124;
      dst = (undefined4 *)(*(int *)(particles + PARTICLES_COUNT_OFS) * PARTICLE_BLOCK_STRIDE + *(char **)(particles + PARTICLES_BLOCK_ARRAY_OFS));
      for (scratch = PARTICLE_BLOCK_DWORDS; scratch != 0; scratch = scratch + -1) {
        *dst = *cursor;
        cursor = cursor + 1;
        dst = dst + 1;
      }
    }
    /* commit: one more particle in the buffer */
    *(int *)(particles + PARTICLES_COUNT_OFS) = *(int *)(particles + PARTICLES_COUNT_OFS) + 1;
  }
  return;
}
