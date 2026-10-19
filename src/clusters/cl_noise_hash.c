// cluster-entries: noise_hash noise_smooth_value
/* Installed cluster noise_hash (tools/cluster_install.py from output/cluster_pilot/noise_hash.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster noise_hash: value noise of the skid / dirt systems (cosine interpolated 1D noise on an integer hash). */
extern float interp_cosine(float a, float b, float t);
extern double floor(double x);

/* Integer hash of a lattice point mapped to [0, 1).  `mask` (0 = none) is ANDed onto the point first (was: EAX = mask, ECX = point). */
float noise_hash_n(uint mask, uint point)
{
    uint h;

    if (mask != 0) {
        point &= mask;
    }
    h = (point << 13) ^ point;
    return (float)(((h * h * 0x3d73 + 0xc0ae5) * h + 0xd208dd0d) & 0x7fffffff) * 4.656613e-10f;      /* 2^-31 */
}

/* Cosine interpolated noise at x between the lattice points floor(x) and floor(x) + 1. */
float noise_smooth_value(float x, uint mask)
{
    int cell = (int)floor(x);
    float frac = x - (float)(uint)cell;

    return interp_cosine(noise_hash_n(mask, cell), noise_hash_n(mask, cell + 1), frac);
}

/* entry thunk at the original address of noise_hash: register inputs (eax,ecx) -> natural C noise_hash_n; ret 0 */
__declspec(naked) void noise_hash(void)
{
    __asm {
        push edx
        push ecx
        push eax
        call noise_hash_n
        add esp, 8
        pop edx
        ret
    }
}
