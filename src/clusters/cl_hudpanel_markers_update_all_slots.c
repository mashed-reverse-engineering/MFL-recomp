// cluster-entries: hudpanel_markers_update_all_slots
/* Installed cluster hudpanel_markers_update_all_slots (tools/cluster_install.py from output/cluster_pilot_zx/hudpanel_markers_update_all_slots.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006582c0*/ extern undefined g_load_6582c0;
extern char g_hudpanel_658450[];
#define HUDPANEL_SLOT_SIZE (100)

/* hudpanel_markers_update_all_slots */

/* "hudpanel_marker_update for each 0x64 byte slot from 0x6582C0" */

/* Calls hudpanel_marker_update on every HUDPANEL_SLOT_SIZE byte slot, from g_load_6582c0 up to g_hudpanel_658450 (end of the slot array).
   No parameters, no return value. (inferred) */
static int hudpanel_marker_update_r(int p0);
extern void hudpanel_marker_update(void);
void hudpanel_markers_update_all_slots(void)

{
  undefined *slot;

  slot = &g_load_6582c0;
  /* walk the slots in order, one update call per slot */
  do {
    hudpanel_marker_update_r(slot);
    slot = slot + HUDPANEL_SLOT_SIZE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_hudpanel_658450));
  return;
}

/* hudpanel_marker_update_r: C -> original hudpanel_marker_update (registers esi) */
__declspec(naked) int hudpanel_marker_update_r(int p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call hudpanel_marker_update
        pop esi
        ret
    }
}
