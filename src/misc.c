/* MFL.exe misc: functions moved verbatim from batch16.c, batch9.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x004967d0
int *triggers_get_array(int *count)
{
    *count = g_particles_71ab00;
    return &g_triggers;
}

// FUNCTION: MFL 0x00437f60
void screen_shake_add(int v)
{
    g_hit_shake = g_hit_shake + v;
}
