#include "mfl_tier2.h"
#define PLAYER_PTR_SIZE 4
#define FX_SLOT_COUNT 0x200
#define FX_SLOT_STRIDE_DWORDS 8

/*0075cdec*/ extern undefined g_particles_75cdec;


/* fx_slot_table_clear */

/* Clears all FX slot entries in the per-player particle table. */

void __cdecl fx_slot_table_clear(int player_idx)
{
  int *slot_ptr;
  int remaining;
  
  slot_ptr = *(int **)(&g_particles_75cdec + player_idx * PLAYER_PTR_SIZE);
  remaining = FX_SLOT_COUNT;
  do {
    /* Clear first two fields (unknown purpose) of the slot */
    *slot_ptr = 0;
    slot_ptr[1] = 0;
    slot_ptr = slot_ptr + FX_SLOT_STRIDE_DWORDS;
    remaining = remaining + -1;
  } while (remaining != 0);
  return;
}
