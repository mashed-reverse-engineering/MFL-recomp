// cluster-entries: hud_icon_uv_rect_get
/* Installed cluster hud_icon_uv_rect_get (tools/cluster_install.py from output/cluster_pilot5/hud_icon_uv_rect_get.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster hud_icon_uv_rect_get: uv rectangle (u0 v0 u1 v1) of icon 0..4 in a 4 wide atlas with 0.25 steps; icon 3 is trimmed, anything else the whole texture. */
extern float k_f_0_25_5de1bc;

/* was: ESI = index, ECX = result */
void hud_icon_uv_rect_get_n(int index, float *uv)
{
    if (index < 0 || index >= 5) {
        uv[0] = 0.0f;
        uv[1] = 0.0f;
        uv[2] = 1.0f;
        uv[3] = 1.0f;
        return;
    }
    uv[0] = (float)(index & 3) * k_f_0_25_5de1bc;
    uv[1] = (float)(index / 4) * k_f_0_25_5de1bc;
    uv[2] = (float)((index & 3) + 1) * k_f_0_25_5de1bc;
    uv[3] = (float)((index + 4) / 4) * k_f_0_25_5de1bc;
    if (index == 3) {
        uv[0] = 0.255f;
        uv[1] = 0.255f;
        uv[2] = 0.93f;
        uv[3] = 0.97f;
    }
}

/* entry thunk at the original address of hud_icon_uv_rect_get: register inputs (esi,ecx) -> natural C hud_icon_uv_rect_get_n; ret 0 */
__declspec(naked) void hud_icon_uv_rect_get(void)
{
    __asm {
        push ecx
        push esi
        call hud_icon_uv_rect_get_n
        add esp, 8
        ret
    }
}
