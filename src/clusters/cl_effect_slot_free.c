// cluster-entries: effect_slot_free
/* Installed cluster effect_slot_free (tools/cluster_install.py from output/cluster_pilot10/effect_slot_free.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/*004c66a0*/ extern void * mem_zero(void * p0, int p1);
/*006c5e20*/ extern undefined g_gfx_6c5e20;
/*006f1e20*/ extern undefined1 g_gfx_6f1e20;
/* effect_slot_free */

/* WARNING: Unknown calling convention */
/* "clears the 0x2C0 byte effect slot and its bit in the allocation mask" */

void effect_slot_free_n(int in_esi)

{
  mem_zero((char *)&g_gfx_6c5e20 + in_esi * 0x2c0,0x2c0);
  (&g_gfx_6f1e20)[in_esi >> 3] = (&g_gfx_6f1e20)[in_esi >> 3] & ~('\x01' << ((byte)in_esi & 7));
  return;
}

/* entry thunk at the original address of effect_slot_free: register inputs (esi) -> natural C effect_slot_free_n; ret 0 */
__declspec(naked) void effect_slot_free(void)
{
    __asm {
        push esi
        call effect_slot_free_n
        add esp, 4
        ret
    }
}
