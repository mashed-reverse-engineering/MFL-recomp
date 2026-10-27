#include "mfl_tier2.h"
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00653040*/ extern undefined4 g_trail_653040;
/*00653148*/ extern undefined4 g_trail_653148;

#define TRAIL_CLUMP_OFFSET 0x3c /* unknown struct offset */
#define TRAIL_RECORD_DWORDS 0xb
#define TRAIL_TABLE_END ((int)&g_trail_653148)


/* trail_clumps_destroy */
/* Destroys all trail record clumps in the global table and the master clump. */

void trail_clumps_destroy(void)
{
  int *trail_record;
  
  /* Iterate through trail records and destroy each clump (offset in unknown struct) */
  trail_record = &g_trail_653040;
  do {
    RpClumpDestroy(*(RpClump **)(*trail_record + TRAIL_CLUMP_OFFSET));
    trail_record = trail_record + TRAIL_RECORD_DWORDS;
  } while ((int)trail_record < TRAIL_TABLE_END);

  /* Destroy and clear the master clump if present */
  if (g_trail_653148 != (RpClump *)0x0) {
    RpClumpDestroy(g_trail_653148);
    g_trail_653148 = (RpClump *)0x0;
  }
  return;
}
