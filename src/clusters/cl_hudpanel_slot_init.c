// cluster-entries: hudpanel_slot_init
/* Installed cluster hudpanel_slot_init (tools/cluster_install.py from output/cluster_pilot5/hudpanel_slot_init.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster hudpanel_slot_init: clears a HUD panel slot (0x6c bytes) and sets the default scale 0.05 and the white vertex colour. */
typedef struct HudPanelSlot {
    char unknown_00[8];
    float scale_x;         /* 0x08 */
    float scale_y;         /* 0x0c */
    unsigned char colour[4];   /* 0x10: r g b a */
} HudPanelSlot;

extern void FUN_004c66a0(void *p, int n);

/* was: ESI = slot */
void hudpanel_slot_init_n(HudPanelSlot *slot)
{
    FUN_004c66a0(slot, 0x6c);
    slot->scale_x = 0.05f;
    slot->scale_y = 0.05f;
    slot->colour[0] = 0xff;
    slot->colour[1] = 0xff;
    slot->colour[2] = 0xff;
    slot->colour[3] = 0xff;
}

/* entry thunk at the original address of hudpanel_slot_init: register inputs (esi) -> natural C hudpanel_slot_init_n; ret 0 */
__declspec(naked) void hudpanel_slot_init(void)
{
    __asm {
        push esi
        call hudpanel_slot_init_n
        add esp, 4
        ret
    }
}
