// cluster-entries: stopclock_render_timed_parts stopclock_render_timed_parts_forward
/* Installed cluster stopclock_render_timed_parts (tools/cluster_install.py from output/cluster_pilot/stopclock_render_timed_parts.c); entries are patched at their original addresses by build_overlay.py --clusters. */
#include "mfl_tier2.h"
/* DECLS-BEGIN */
/* DECLS-END */
/* Cluster stopclock_render_timed_parts: the stopclock intro; which parts are drawn depends on the timer. */
typedef struct RenderObject RenderObject;
struct RenderObject {
    char unknown_00[0x48];
    void (*render)(RenderObject *self);      /* callback at +0x48, called with the object */
};

typedef struct StopclockPanel {
    RenderObject *parts[6];
    char unknown_18[0x14];
    float timer;           /* seconds since the panel started */
} StopclockPanel;

extern StopclockPanel g_load_659a7c;

static void render_part(RenderObject *part)
{
    part->render(part);
}

/* Draws the parts for the current timer value (thresholds 0.34, 0.82, 1.3, 1.86, 4.0).  (was: EAX = panel) */
void stopclock_render_timed_parts_n(StopclockPanel *panel)
{
    float t = panel->timer;

    if (t < 4.0f) {
        render_part(panel->parts[0]);
        render_part(panel->parts[5]);
    }
    if (0.34f <= panel->timer) {
        if (0.82f <= panel->timer) {
            if (1.3f <= panel->timer) {
                if (panel->timer < 1.86f) {
                    render_part(panel->parts[1]);
                    render_part(panel->parts[2]);
                    render_part(panel->parts[3]);
                }
            } else {
                render_part(panel->parts[1]);
                render_part(panel->parts[2]);
            }
        } else {
            render_part(panel->parts[1]);
        }
        if (1.86f <= panel->timer && panel->timer < 4.0f) {
            render_part(panel->parts[4]);
        }
    }
}

/* Draws the global stopclock panel. */
void stopclock_render_timed_parts_forward(void)
{
    stopclock_render_timed_parts_n(&g_load_659a7c);
}

/* entry thunk at the original address of stopclock_render_timed_parts: register inputs (eax) -> natural C stopclock_render_timed_parts_n; ret 0 */
__declspec(naked) void stopclock_render_timed_parts(void)
{
    __asm {
        push eax
        call stopclock_render_timed_parts_n
        add esp, 4
        ret
    }
}
