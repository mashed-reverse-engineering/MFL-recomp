// cluster-entries: hudpanel_slots_clear
/* Installed cluster hudpanel_slots_clear (tools/cluster_install.py from output/cluster_pilot_zx/hudpanel_slots_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006582c0*/ extern undefined g_load_6582c0;
extern char g_hudpanel_658450[];
#define HUDPANEL_SLOT_SIZE (100) /* 0x64 bytes per hudpanel slot */

/* hudpanel_slots_clear */

/* "hudpanel_slot_clear for every 0x64 byte slot from 0x6582C0" */

/* Clears every HUD panel slot: calls hudpanel_slot_clear on each HUDPANEL_SLOT_SIZE-byte slot
   from g_load_6582c0 up to (not including) g_hudpanel_658450. No parameters, no return value.
   The slot pointer is passed in eax via hudpanel_slot_clear. */

static int hudpanel_slot_clear_r(int p0);
extern void hudpanel_slot_clear(void);
void hudpanel_slots_clear(void)

{
  undefined *slot;

  /* walk the slot array from its first slot to the end marker */
  slot = &g_load_6582c0;
  do {
    hudpanel_slot_clear_r(slot);
    slot = slot + HUDPANEL_SLOT_SIZE;
  } while ((LONG_PTR)slot < ((LONG_PTR)&g_hudpanel_658450));
  return;
}

/* hudpanel_slot_clear_r: C -> original hudpanel_slot_clear (registers eax) */
__declspec(naked) int hudpanel_slot_clear_r(int p0)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_clear
        ret
    }
}
