// cluster-entries: vehicle_markers_render_parts vehicle_markers_render_if_loaded
/* Installed cluster vehicle_markers_render_parts (tools/cluster_install.py from output/cluster_pilot5/vehicle_markers_render_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster vehicle_markers_render_parts: draws the three marker objects (+0x16c..) and six per-player slots (0x2c bytes each: 11 digit part pointers, a fallback part at +0x28). */
extern int g_game_659d00;
extern char g_load_659b30[];

static void part_draw(void *part)
{
    void (*fn)(void *) = *(void (**)(void *))((char *)part + 0x48);
    fn(part);
}

/* was: EDI = marker block */
void vehicle_markers_render_parts_n(char *block)
{
    int i;

    part_draw(*(void **)(block + 0x16c));
    part_draw(*(void **)(block + 0x170));
    part_draw(*(void **)(block + 0x174));
    for (i = 0; i < 6; i++) {
        char *slot = block + i * 0x2c;
        int value = *(int *)(block + 0x188 + (i / 2) * 4);
        if (value == -1) {
            part_draw(*(void **)(slot + 0x28));
        } else {
            int digit = (i % 2 == 0) ? value / 10 : value % 10;
            part_draw(*(void **)(slot + digit * 4));
        }
    }
}

void vehicle_markers_render_if_loaded(void)
{
    if (g_game_659d00 != 0) {
        vehicle_markers_render_parts_n(g_load_659b30);
    }
}

/* entry thunk at the original address of vehicle_markers_render_parts: register inputs (edi) -> natural C vehicle_markers_render_parts_n; ret 0 */
__declspec(naked) void vehicle_markers_render_parts(void)
{
    __asm {
        push edi
        call vehicle_markers_render_parts_n
        add esp, 4
        ret
    }
}
