// cluster-entries: particle_random_scale
/* Installed cluster particle_random_scale (tools/cluster_install.py from output/cluster_pilot5/particle_random_scale.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster particle_random_scale: x and y scale of a particle = c1 * c2 * a * b + offset (global constants of the smoke emitter). */
extern float g_particle_random_629450;
extern float g_particle_random_629430;
extern float g_particle_random_62944c;

/* was: EAX = result (two floats), a and b on the stack */
void particle_random_scale_n(float *scale, float a, float b)
{
    float v = g_particle_random_629450 * g_particle_random_629430 * a * b + g_particle_random_62944c;
    scale[1] = v;
    scale[0] = v;
}

/* entry thunk at the original address of particle_random_scale: register inputs (eax,s0,s1) -> natural C particle_random_scale_n; ret 0 */
__declspec(naked) void particle_random_scale(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push eax
        call particle_random_scale_n
        add esp, 12
        pop edx
        pop ecx
        pop eax
        ret
    }
}
