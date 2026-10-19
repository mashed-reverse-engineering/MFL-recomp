#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();

/* byte size of one particle batch entry (inferred) */
#define PARTICLE_ENTRY_SIZE (0x30)
/* byte offsets from the current entry of the uv_b and uv_c pairs (inferred) */
#define UV_B_OFFSET (-0x10)
#define UV_C_OFFSET (-0x8)
/* particle_batch_set_uv_b_c */

/* "stores 2+2 floats into the current entry" */

/*
 * Writes two uv pairs (uv_b, uv_c) into the current particle batch entry.
 * batch[0] is the base pointer of the entry array, batch[2] is the index of the
 * current entry; each entry is PARTICLE_ENTRY_SIZE bytes and its uv fields sit at
 * negative offsets from it (inferred). Returns nothing.
 */
void particle_batch_set_uv_b_c(int *batch,undefined4 *uv_b,undefined4 *uv_c)

{
  int entry_index;
  char *entry_base;

  /* uv_b: two floats, one stored per 4-byte slot */
  entry_index = batch[2];
  entry_base = *batch;
  *(undefined4 *)(entry_index * PARTICLE_ENTRY_SIZE + UV_B_OFFSET + entry_base) = *uv_b;
  *(undefined4 *)(entry_index * PARTICLE_ENTRY_SIZE + -0xc + entry_base) = uv_b[1];
  /* uv_c: same layout, written after uv_b */
  entry_index = batch[2];
  entry_base = *batch;
  *(undefined4 *)(entry_index * PARTICLE_ENTRY_SIZE + UV_C_OFFSET + entry_base) = *uv_c;
  *(undefined4 *)(entry_index * PARTICLE_ENTRY_SIZE + -4 + entry_base) = uv_c[1];
  return;
}


