// cluster-entries: score_panel_entry_destroy score_panel_destroy
/* Installed cluster score_panel_entry_destroy (tools/cluster_install.py from output/cluster_pilot3/score_panel_entry_destroy.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/*004f6f40*/ extern RwBool RpClumpDestroy(struct RpClump *p0);
/*00485240*/ extern RwBool RwTexDictionaryDestroy_004d5ac0(struct RwTexDictionary *p0);
/* DECLS-END */
/* Cluster score_panel_entry_destroy: releases the clump of one score panel entry, then the texture dictionary of the panel. */
typedef struct ScorePanelEntry {
    char unknown_00[0x5c];
    RpClump *clump;
    char unknown_60[0x14];
} ScorePanelEntry;                  /* 0x74 bytes */

extern ScorePanelEntry g_score_panel_658df0[4];
extern RwTexDictionary *g_score_panel_658da8;

/* Destroys the clump of one entry (was: EAX = entry). */
void score_panel_entry_destroy_n(ScorePanelEntry *entry)
{
    RpClumpDestroy(entry->clump);
}

/* Destroys all four entries and the panel texture dictionary. */
void score_panel_destroy(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        score_panel_entry_destroy_n(&g_score_panel_658df0[i]);
    }
    RwTexDictionaryDestroy_004d5ac0(g_score_panel_658da8);
}

/* entry thunk at the original address of score_panel_entry_destroy: register inputs (eax) -> natural C score_panel_entry_destroy_n; ret 0 */
__declspec(naked) void score_panel_entry_destroy(void)
{
    __asm {
        push eax
        call score_panel_entry_destroy_n
        add esp, 4
        ret
    }
}
