#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
#define ENTRY_STRIDE (0x30) /* bytes per particle entry (inferred) */
#define UV_OFFSET_0 (-0x20) /* offset of uv[0] from the entry base (inferred) */
#define UV_OFFSET_1 (-0x1c) /* offset of uv[1] from the entry base (inferred) */
/* particle_batch_set_uv_a */

/* "stores 2 floats into the current entry" */

/* Copies the two UV floats in uv_in[0..1] into the current entry of batch.
   batch[0] = entry array base (inferred), batch[2] = current entry index (inferred).
   Writes at entry_index * 0x30 - 0x20 and - 0x1c from the base (inferred: UV slots of the entry). */
void particle_batch_set_uv_a(int *batch,undefined4 *uv_in)

{
  int entry_index;
  char *entry_base;

  entry_index = batch[2];
  entry_base = *batch;
  *(undefined4 *)(entry_index * ENTRY_STRIDE + UV_OFFSET_0 + entry_base) = *uv_in;
  *(undefined4 *)(entry_index * ENTRY_STRIDE + UV_OFFSET_1 + entry_base) = uv_in[1];
  return;
}


