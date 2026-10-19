#include "mfl_tier2.h"
/*004077d0*/ extern int fx_counters_clear();
/*0048e720*/ extern undefined __cdecl rwp_object_set_matrix(int p0, RwMatrix *p1);
/*004c48b0*/ extern void FUN_004c48b0(undefined4 p0, undefined4 p1, undefined4 p2);
/*004d15a0*/ extern struct RwFrame * RwFrameTransform(struct RwFrame *p0, RwMatrix *p1, enum RwOpCombineType p2);
/*00651320*/ extern undefined4 g_fx_651320;
/*00651aa0*/ extern undefined4 g_fx_object_count;
/*00651aa4*/ extern undefined4 g_explosion_651aa4;
/*00651aa8*/ extern undefined4 g_fx_651aa8;

/* one effect object entry, in dwords (stride of the table walk) */
#define FX_ENTRY_SIZE_DWORDS (0x3b)

/* fx_objects_reset */

/* "clears the effect object table at 0x651250" */

/*
 * Resets the effect object table: clears the effect counters, then walks
 * every entry (g_fx_object_count of them) and puts it back to its default
 * state. An entry whose handle at [-1] is non-zero also gets its matrix
 * rebuilt from its parent frame and pushed to the render object.
 * Clears g_explosion_651aa4 and g_fx_651aa8 at the end.
 * Parameters: none. Returns nothing.
 */
void fx_objects_reset(void)

{
  undefined4 *entry;
  int entry_index;
  RwMatrix transform;

  entry_index = 0;
  fx_counters_clear();
  if (0 < (int)g_fx_object_count) {
    /* walk the table, one entry per step (FX_ENTRY_SIZE_DWORDS apart) */
    entry = &g_fx_651320;
    do {
      /* reset the entry's fields to defaults (inferred: field meanings unknown, float constants are raw bits) */
      entry[-0x22] = 0;
      *entry = 0;
      entry[-0x1d] = 0x42c80000;
      entry[5] = 0;
      entry[4] = 0;
      entry[3] = 0;
      entry[1] = 0;
      entry[-0x1b] = 0;
      entry[-0x21] = 0;
      entry[-10] = 0;
      entry[-0xb] = 0;
      entry[-0x19] = 0;
      entry[-0x1a] = 0;
      entry[-0x18] = 0x3f19999a;
      entry[-0x17] = 0x3fb33333;
      entry[-0x16] = 0x3d4ccccd;
      entry[-0xf] = 0x3db851ec;
      entry[-0xe] = 0x41200000;
      entry[-0xd] = 0x3dcccccd;
      entry[-0xc] = 0x3e4ccccd;
      /* entries with a handle at [-1] get their matrix refreshed (inferred) */
      /* entries with a handle at [-1] get their matrix refreshed (inferred) */
      if (entry[-1] != 0) {
        FUN_004c48b0(&transform,entry[-1],0);
        RwFrameTransform(*(RwFrame **)(entry[-0x30] + 4),&transform,rwCOMBINEREPLACE);
        rwp_object_set_matrix(entry[-0x23],&transform);
      }
      entry_index = entry_index + 1;
      entry = entry + FX_ENTRY_SIZE_DWORDS;
    } while (entry_index < (int)g_fx_object_count);
  }
  g_explosion_651aa4 = 0;
  g_fx_651aa8 = 0;
  return;
}


