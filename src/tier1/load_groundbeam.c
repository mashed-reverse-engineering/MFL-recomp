#include "mfl_tier2.h"
/*0040d0d0*/ extern int __cdecl find_texture_dict_b(RwChar *p0);
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*0065c8f8*/ extern undefined g_game_65c8f8;
/*0065d808*/ extern undefined g_load_groundbeam_65d808;
/*00660568*/ extern undefined4 g_game_660568;
/* Size in bytes of one per-slot GroundBeam record (inferred from the stride used for both globals). */
#define GROUNDBEAM_SLOT_SIZE (0xf1c)
/* load_groundbeam
 * Resets the GroundBeam state for one slot: zeroes its record in g_game_65c8f8, stores the slot
 * index in the first int of its record in g_load_groundbeam_65d808, and loads the "GroundBeam"
 * texture dictionary into g_game_660568 if it has not been loaded yet (the dictionary is shared
 * by all slots). Parameter: slot_index, the index of the slot to reset. Returns nothing.
 */
/* GroundBeam */
void __cdecl load_groundbeam(int slot_index)
{
  bool texture_dict_missing;
  char *rec = (char *)&g_game_65c8f8 + slot_index * GROUNDBEAM_SLOT_SIZE;

  /* clear the whole slot record */
  mem_zero(rec,GROUNDBEAM_SLOT_SIZE);
  texture_dict_missing = g_game_660568 == 0;
  /* (inferred) record field 0xf10 (= g_load_groundbeam_65d808 - g_game_65c8f8) holds the slot index */
  *(int *)(rec + 0xf10) = slot_index;
  if (texture_dict_missing) {
    g_game_660568 = find_texture_dict_b("GroundBeam");
  }
  return;
}
