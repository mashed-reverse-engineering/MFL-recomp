// cluster-entries: debris_slot_clear debris_slot_clear_forward
/* Installed cluster debris_slot_clear (tools/cluster_install.py from output/cluster_pilot5/debris_slot_clear.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster debris_slot_clear: clears one 0x28 byte slot of the car_damage table (the debris part, +8..+0x1c). */
extern char car_damage[];
extern void FUN_004c66a0(void *p, int n);

/* was: ESI = slot */
void debris_slot_clear_n(int *slot)
{
    FUN_004c66a0(slot, 8);
    slot[2] = 0;
    slot[3] = 0;
    slot[4] = 0;
    slot[5] = 0;
    slot[7] = 0;
}

void debris_slot_clear_forward(int index)
{
    debris_slot_clear_n((int *)(car_damage + index * 40));
}

/* entry thunk at the original address of debris_slot_clear: register inputs (esi) -> natural C debris_slot_clear_n; ret 0 */
__declspec(naked) void debris_slot_clear(void)
{
    __asm {
        push esi
        call debris_slot_clear_n
        add esp, 4
        ret
    }
}
