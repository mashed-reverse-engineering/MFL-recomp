#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00658cd8*/ extern undefined4 g_btb_658cd8;
/*00658ce0*/ extern char g_hudpanel_658ce0[];

/* Bytes between g_btb_658cd8 and g_hudpanel_658ce0: the clump pointer table (2 slots). */
#define BTB_CLUMP_TABLE_BYTES (8)

/* btb_panel_destroy_extra */

/* "RpClumpDestroy(0x658CD8)" */

/* Destroys every RpClump in the clump pointer table at g_btb_658cd8 (inferred: the
   table holds the extra panel clumps), then zeroes the table. Takes no parameters,
   returns nothing. */
void btb_panel_destroy_extra(void)

{
  undefined4 *clump_slot;

  /* Walk the table slot by slot up to g_hudpanel_658ce0 (inferred: end of the table). */
  clump_slot = &g_btb_658cd8;
  do {
    RpClumpDestroy((RpClump *)*clump_slot);
    clump_slot = clump_slot + 1;
  } while ((LONG_PTR)clump_slot < ((LONG_PTR)&g_hudpanel_658ce0));
  mem_zero(&g_btb_658cd8,BTB_CLUMP_TABLE_BYTES);
  return;
}


