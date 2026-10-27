// cluster-entries: machinegun_states_clear
/* Installed cluster machinegun_states_clear (tools/cluster_install.py from output/cluster_pilot_zx/machinegun_states_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/*006a60d0*/ extern struct MachinegunState MachinegunState_ARRAY_006a60d0[4];
/*006a6230*/ extern undefined4 g_weapon_6a6230;
extern char g_weapon_6a61f0[];

/* machinegun_states_clear */

/* "g_weapon_6a6230 = 0 and clears every MachinegunState" */

/* Resets the machine-gun weapon state: clears the global g_weapon_6a6230 (inferred: the machine-gun
   weapon flag), then clears each MachinegunState in MachinegunState_ARRAY_006a60d0 (4 entries).
   Takes no parameters and returns nothing. */

static int machinegun_state_clear_r(undefined4 * p0);
extern void machinegun_state_clear(void);
void machinegun_states_clear(void)

{
  MachinegunState *state;

  g_weapon_6a6230 = 0;
  /* Walk the array from its first entry up to g_weapon_6a61f0 (the address just past the last entry),
     clearing each one through machinegun_state_clear. */
  state = MachinegunState_ARRAY_006a60d0;
  do {
    machinegun_state_clear_r(state);
    state = state + 1;
  } while ((LONG_PTR)state < ((LONG_PTR)&g_weapon_6a61f0));
  return;
}

/* machinegun_state_clear_r: C -> original machinegun_state_clear (registers esi) */
__declspec(naked) int machinegun_state_clear_r(undefined4 * p0)
{
    __asm {
        push esi
        mov esi, dword ptr [esp + 8]
        call machinegun_state_clear
        pop esi
        ret
    }
}
