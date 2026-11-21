#include "mfl_tier2.h"
/*006582c0*/ extern undefined g_load_6582c0;
extern void hudpanel_marker_update_n(int *m);
extern char g_hudpanel_658450[];
#define HUDPANEL_SLOT_SIZE (100)

/* hudpanel_markers_update_all_slots */

/* "hudpanel_marker_update for each 0x64 byte slot from 0x6582C0" */

/* Calls hudpanel_marker_update on every HUDPANEL_SLOT_SIZE byte slot, from g_load_6582c0 up to g_hudpanel_658450 (end of the slot array).
   No parameters, no return value. (inferred) */
void hudpanel_markers_update_all_slots(void)

{
  undefined *slot;

  slot = &g_load_6582c0;
  /* walk the slots in order, one update call per slot */
  do {
    hudpanel_marker_update_n((int *)slot);
    slot = slot + HUDPANEL_SLOT_SIZE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_hudpanel_658450));
  return;
}


