#include "mfl_tier2.h"


/* particle_batch_add */

/* "appends a particle (position, colour) to the batch if it is not full" */

void particle_batch_add(int *batch,undefined4 *position,undefined1 *colour)
{
  undefined4 *slot;
  int byte_offset;
  
  if (batch[2] >= batch[1]) {
    batch[2] = batch[1];
    return;
  }
  byte_offset = batch[2] * 0x30;
  slot = (undefined4 *)(*batch + 4 + byte_offset);
  *slot = *position;
  slot[1] = position[1];
  slot[2] = position[2];
  *(undefined4 *)(*batch + 0x1c + byte_offset) = 0;
  *(undefined1 *)(byte_offset + *batch) = *colour;
  *(undefined1 *)(*batch + 1 + byte_offset) = colour[1];
  *(undefined1 *)(*batch + 2 + byte_offset) = colour[2];
  *(undefined1 *)(*batch + 3 + byte_offset) = colour[3];
  batch[2] = batch[2] + 1;
  batch[0x15] = 1;
  return;
}
