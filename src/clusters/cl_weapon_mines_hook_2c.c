// cluster-entries: weapon_mines_hook_2c
/* Installed cluster weapon_mines_hook_2c (tools/cluster_install.py from output/cluster_pilot_zx/weapon_mines_hook_2c.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006a7d38*/ extern undefined g_weapon_6a7d38;
extern char g_weapon_6a8ab8[];
#define WEAPON_SLOT_STRIDE (0x6c)
#define WEAPON_SLOT_STATE_OFFSET (0x60)

/* weapon_mines_hook_2c
 *
 * Walks the weapon fx slot table (g_weapon_6a7d38, WEAPON_SLOT_STRIDE bytes per
 * slot, up to g_weapon_6a8ab8) and clears each slot whose state, read at
 * WEAPON_SLOT_STATE_OFFSET, is 2 or 4..6 (inferred: these are the mine states).
 * Takes no parameters and returns nothing.
 */

static int weapon_fx_slot_clear_b_r(undefined4 * p0);
extern void weapon_fx_slot_clear_b(void);
void weapon_mines_hook_2c(void)

{
  int slot_state;
  undefined *weapon_slot;

  weapon_slot = &g_weapon_6a7d38;
  /* for each slot in the table: clear it if its state matches (inferred) */
  do {
    slot_state = *(int *)(weapon_slot + WEAPON_SLOT_STATE_OFFSET);
    if ((slot_state == 2) || ((3 < slot_state && (slot_state < 7)))) {
      weapon_fx_slot_clear_b_r(weapon_slot);
    }
    weapon_slot = weapon_slot + WEAPON_SLOT_STRIDE;
  } while ((LONG_PTR)weapon_slot < ((LONG_PTR)&g_weapon_6a8ab8));
  return;
}

/* weapon_fx_slot_clear_b_r: C -> original weapon_fx_slot_clear_b (registers esi) */
__declspec(naked) int weapon_fx_slot_clear_b_r(undefined4 * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call weapon_fx_slot_clear_b
        pop esi
        ret
    }
}
