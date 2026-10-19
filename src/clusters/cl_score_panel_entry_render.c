// cluster-entries: score_panel_entry_render score_panel_render
/* Installed cluster score_panel_entry_render (tools/cluster_install.py from output/cluster_pilot2/score_panel_entry_render.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster score_panel_entry_render: one entry of the score panel (4 entries, 116 bytes each). Each entry holds sub-objects
   that are drawn / released through their virtual method at +0x48; the flag word selects which of them are shown. */

struct ScorePanelEntry {
    void *sub[16];          /* 0x00-0x3c: sub-objects, sub[0] .. sub[15] */
    void *head;             /* 0x40: header sub-object */
    int unknown_44[9];      /* 0x44-0x63 */
    unsigned int flags;     /* 0x68: display flags */
    int active;             /* 0x6c: non-zero when the entry is shown */
    int unknown_70;         /* 0x70 */
};

extern struct ScorePanelEntry g_score_panel_658df0[4];

/* Calls the function pointer stored at +0x48 of the sub-object itself (one dereference, no vtable: `push obj; call [obj+0x48]`). */
static void sub_object_method48(void *obj)
{
    void (*fn)(void *) = *(void (**)(void *))((char *)obj + 0x48);
    fn(obj);
}

/* Updates the sub-objects of one score panel entry in the order of the original code: header, base, then the
   variant selected by flag 0x800 (0x1000 / 0x40 gate the optional pair), then the optional items by flag bit. */
void score_panel_entry_render_n(struct ScorePanelEntry *entry)
{
    sub_object_method48(entry->head);
    sub_object_method48(entry->sub[0]);
    if ((entry->flags & 0x800U) == 0) {
        if ((entry->flags & 0x1000U) != 0) {
            sub_object_method48(entry->sub[2]);
            sub_object_method48(entry->sub[1]);
            if ((entry->flags & 0x40U) != 0) sub_object_method48(entry->sub[3]);
        }
    } else {
        sub_object_method48(entry->sub[5]);
        sub_object_method48(entry->sub[4]);
        if ((entry->flags & 0x40U) != 0) sub_object_method48(entry->sub[6]);
    }
    if ((entry->flags & 0x1U) != 0) sub_object_method48(entry->sub[7]);
    if ((entry->flags & 0x2U) != 0) sub_object_method48(entry->sub[8]);
    if ((entry->flags & 0x4U) != 0) sub_object_method48(entry->sub[9]);
    if ((entry->flags & 0x8U) != 0) sub_object_method48(entry->sub[10]);
    if ((entry->flags & 0x10U) != 0) sub_object_method48(entry->sub[11]);
    if ((entry->flags & 0x20U) != 0) sub_object_method48(entry->sub[12]);
    if ((entry->flags & 0x2000U) != 0) sub_object_method48(entry->sub[13]);
    if ((entry->flags & 0x4000U) != 0) sub_object_method48(entry->sub[14]);
    if ((entry->flags & 0x8000U) != 0) sub_object_method48(entry->sub[15]);
}

/* Renders every shown entry of the score panel (the 4 entries of g_score_panel_658df0). */
void score_panel_render(void)
{
    int i;

    for (i = 0; i < 4; i++) {
        if (g_score_panel_658df0[i].active != 0) {
            score_panel_entry_render_n(&g_score_panel_658df0[i]);
        }
    }
}

/* entry thunk at the original address of score_panel_entry_render: register inputs (eax) -> natural C score_panel_entry_render_n; ret 0 */
__declspec(naked) void score_panel_entry_render(void)
{
    __asm {
        push eax
        call score_panel_entry_render_n
        add esp, 4
        ret
    }
}
