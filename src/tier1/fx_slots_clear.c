#include "mfl_tier2.h"
/*00759760*/ extern undefined4 g_particles_759760;
/*0075c000*/ extern undefined g_particles_75c000;
/* fx_ptank_b_lock_and_fill */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "locks ptank atomic b and writes position/size/colour/uv for each active fx entry (two vertices
   per entry)" */
/* fx_ptank_a_lock_and_fill */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* "locks ptank atomic a and writes the per-entry particles (position, size by style, colour, uv)"
    */
/* fx_ptank_render_b */
/* "renders g_particles_75969c" */
#define FX_SLOT_WORDS (0x1a)
/* fx_slots_clear */
/* "clears every 0x68 byte slot from 0x75C000" */
/* Resets the fx slot table that starts at 0x75C000 and runs down to 0x759760 (160 slots of
   FX_SLOT_WORDS words). Takes no parameters, returns nothing. For each slot it sets the first
   word to 1 (inferred: an in use flag) and zeroes the word just below the slot. */
void fx_slots_clear(void)
{
  undefined4 *slot;

  slot = (undefined4 *)&g_particles_75c000;
  do {
    slot = slot + -FX_SLOT_WORDS;
    slot[-1] = 0;
    *slot = 1;
  } while (slot != &g_particles_759760);
  return;
}
