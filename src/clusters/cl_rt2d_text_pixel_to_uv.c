// cluster-entries: rt2d_text_pixel_to_uv
/* Installed cluster rt2d_text_pixel_to_uv (tools/cluster_install.py from output/cluster_pilot5/rt2d_text_pixel_to_uv.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster rt2d_text_pixel_to_uv: 640x480 pixel position to the unit square of the Rt2d text (v flipped). */
extern float k_f_0_0020828124;
extern float k_f_0_0020833334;
extern float float_1_0;

/* was: EAX = result (u, v), x and y on the stack */
void rt2d_text_pixel_to_uv_n(float *uv, float x, float y)
{
    uv[0] = x * k_f_0_0020828124;
    uv[1] = float_1_0 - y * k_f_0_0020833334;
}

/* entry thunk at the original address of rt2d_text_pixel_to_uv: register inputs (eax,s0,s1) -> natural C rt2d_text_pixel_to_uv_n; ret 0 */
__declspec(naked) void rt2d_text_pixel_to_uv(void)
{
    __asm {
        push eax
        push ecx
        push edx
        push dword ptr [esp + 20]
        push dword ptr [esp + 20]
        push eax
        call rt2d_text_pixel_to_uv_n
        add esp, 12
        pop edx
        pop ecx
        pop eax
        ret
    }
}
