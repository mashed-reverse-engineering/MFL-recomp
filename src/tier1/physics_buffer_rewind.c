#include "mfl_tier2.h"


/* Physics buffer field offsets (exact field names unknown) */
enum {
  PHYSICS_BUFFER_OFFSET_START = 0x18,
  PHYSICS_BUFFER_OFFSET_CURRENT = 0x1c
};

/* Rewind the physics buffer by resetting the current pointer to the start pointer. */
void __cdecl physics_buffer_rewind(char *physics_buffer)
{
  /* Reset current position to buffer start */
  *(undefined4 *)(physics_buffer + PHYSICS_BUFFER_OFFSET_CURRENT) = *(undefined4 *)(physics_buffer + PHYSICS_BUFFER_OFFSET_START);
  return;
}
