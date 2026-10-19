// cluster-entries: effect_slot_init_with_ftol
/* Installed cluster effect_slot_init_with_ftol (tools/cluster_install.py from output/cluster_pilot6/effect_slot_init_with_ftol.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster effect_slot_init_with_ftol: black colour with an alpha byte made from a triangle wave of the argument (a below 0.5, else 1 - a) * 255. */
extern float k_f_0_5;
extern float float_1_0;
extern float k_f_255;

/* was: ESI = colour (4 bytes), the argument is the one stack argument */
void effect_slot_init_with_ftol_n(unsigned char *colour, float a)
{
    float wave = (a < k_f_0_5) ? a : float_1_0 - a;

    colour[3] = (unsigned char)(int)(wave * k_f_255);
    colour[0] = 0;
    colour[1] = 0;
    colour[2] = 0;
}

/* entry thunk at the original address of effect_slot_init_with_ftol: register inputs (esi,s0) -> natural C effect_slot_init_with_ftol_n; ret 0 */
__declspec(naked) void effect_slot_init_with_ftol(void)
{
    __asm {
        push dword ptr [esp + 4]
        push esi
        call effect_slot_init_with_ftol_n
        add esp, 8
        ret
    }
}
