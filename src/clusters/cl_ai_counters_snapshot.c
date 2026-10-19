// cluster-entries: ai_counters_snapshot
/* Installed cluster ai_counters_snapshot (tools/cluster_install.py from output/cluster_pilot5/ai_counters_snapshot.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster ai_counters_snapshot: copies the four AI counters into g_ai_counters_659ab0 and derives the digit count of the score panel from the first one
   (4 digits below counter 1, one fewer for every further counter it reaches). */
typedef struct ScorePanel {
    char unknown_00[0x4c];
    float value;           /* 0x4c */
    int digits;            /* 0x50 */
} ScorePanel;

extern float g_ai_counters_659ab0[4];
extern float ai_get_counter(int index);

/* was: EDI = panel */
void ai_counters_snapshot_n(ScorePanel *panel)
{
    int i;

    for (i = 0; i < 4; i++) {
        g_ai_counters_659ab0[i] = ai_get_counter(i);
    }
    panel->value = g_ai_counters_659ab0[0];
    panel->digits = 4;
    if (panel->value >= g_ai_counters_659ab0[1]) {
        panel->digits = 3;
    }
    if (panel->value >= g_ai_counters_659ab0[2]) {
        panel->digits--;
    }
    if (panel->value >= g_ai_counters_659ab0[3]) {
        panel->digits--;
    }
}

/* entry thunk at the original address of ai_counters_snapshot: register inputs (edi) -> natural C ai_counters_snapshot_n; ret 0 */
__declspec(naked) void ai_counters_snapshot(void)
{
    __asm {
        push edi
        call ai_counters_snapshot_n
        add esp, 4
        ret
    }
}
