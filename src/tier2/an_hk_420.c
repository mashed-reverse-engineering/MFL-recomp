#include "mfl_tier2.h"
/*006582c0*/ extern undefined g_load_6582c0;
static int hudpanel_slot_clear_r(int reg_eax);
extern void hudpanel_slot_clear(void);
extern char g_hudpanel_658450[];
#define HUDPANEL_SLOT_SIZE (100) /* 0x64 bytes per hudpanel slot */
__declspec(naked) int hudpanel_slot_clear_r(int reg_eax)
{
    __asm {
        mov eax, dword ptr [esp + 4]
        call hudpanel_slot_clear
        ret
    }
}

/* hudpanel_slots_clear */

/* "hudpanel_slot_clear for every 0x64 byte slot from 0x6582C0" */

/* Clears every HUD panel slot: calls hudpanel_slot_clear on each HUDPANEL_SLOT_SIZE-byte slot
   from g_load_6582c0 up to (not including) g_hudpanel_658450. No parameters, no return value.
   The slot pointer is passed in eax via hudpanel_slot_clear_r. */

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


