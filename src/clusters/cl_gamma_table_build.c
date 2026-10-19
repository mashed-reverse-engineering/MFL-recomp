// cluster-entries: gamma_table_build
/* Installed cluster gamma_table_build (tools/cluster_install.py from output/cluster_pilot_tr/gamma_table_build.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster gamma_table_build: builds the three 256 entry gamma ramps (red, green, blue: the same values) with pow(i / 255, 1 / gamma) * scale * 65535. */
extern float float_1_0;
extern float k_f_0_003921569;
extern float k_f_65535;
extern double __cdecl pow(double x, double y);
#pragma function(pow)

/* was: EAX = ramps (3 x 256 words), then the stack arguments gamma and scale */
void gamma_table_build_n(unsigned short *ramps, float gamma, float scale)
{
    float inverse = float_1_0 / gamma;
    int i;

    for (i = 0; i < 0x100; i++) {
        double scaled = (pow(i * k_f_0_003921569, inverse) * scale);
        unsigned short value = (unsigned short)(int)(scaled * k_f_65535);
        ramps[i] = value;
        ramps[i + 0x100] = value;
        ramps[i + 0x200] = value;
    }
}

/* entry thunk at the original address of gamma_table_build: register inputs (eax,s0,s1) -> natural C gamma_table_build_n; ret 0 */
__declspec(naked) void gamma_table_build(void)
{
    __asm {
        push dword ptr [esp + 8]
        push dword ptr [esp + 8]
        push eax
        call gamma_table_build_n
        add esp, 12
        ret
    }
}
