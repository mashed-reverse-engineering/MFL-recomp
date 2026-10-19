// cluster-entries: vehicle_marker_icon_select
/* Installed cluster vehicle_marker_icon_select (tools/cluster_install.py from output/cluster_pilot8/vehicle_marker_icon_select.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_marker_icon_select: picks the marker icon id of a vehicle from its flag bits (0x400 = 0x40, both 0x10 and 0x20 =
   0x3e, 0x20 = 0x3d, 0x10 = 0x3c, else 0x3b) and, outside the movie state, tells the vehicle renderer the matching icon variant. */
extern int g_game_64d88c;
extern void __cdecl vehicle_render_state_set_6614e4(int vehicle_index, int variant);

/* was: EDI = vehicle record */
void vehicle_marker_icon_select_n(VehicleRecord *vehicle)
{
    unsigned int flags = vehicle->flags;
    int variant = 0;
    int icon = 0x3b;

    if ((flags & 0x400) != 0) {
        icon = 0x40;
    } else if ((flags & 0x10) != 0 && (flags & 0x20) != 0) {
        variant = 3;
        icon = 0x3e;
    } else if ((flags & 0x10) != 0) {
        variant = 1;
        icon = 0x3c;
    } else if ((flags & 0x20) != 0) {
        variant = 2;
        icon = 0x3d;
    }
    vehicle->field_28c = icon;
    if (g_game_64d88c == 0)
        vehicle_render_state_set_6614e4(vehicle->index, variant);
}

/* entry thunk at the original address of vehicle_marker_icon_select: register inputs (edi) -> natural C vehicle_marker_icon_select_n; ret 0 */
__declspec(naked) void vehicle_marker_icon_select(void)
{
    __asm {
        push edi
        call vehicle_marker_icon_select_n
        add esp, 4
        ret
    }
}
