/* MFL.exe ai: functions moved verbatim from batch26.c, batch7.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x004271a0
void ai_clear(void)
{
    int *p;
    int n;
    g_ai_data.tiles_used = 0;
    p = (int *)&g_ai_data;
    for (n = 0x2000; n != 0; n--)
        *p++ = -1;
    p = (int *)g_ai_data.tile_pool;
    for (n = 0x2000; n != 0; n--)
        *p++ = -1;
    g_ai_editor_mode = 0;
    g_ai_editor_menu_cursor = 0;
}

// FUNCTION: MFL 0x004183b0
void ai_bot_set_timer_30000(int bot)
{
    *(int *)((char *)&g_ai_bots[bot] + 0x4c) = AI_BOT_TIMER_RESET;
}

// FUNCTION: MFL 0x00418db0
void ai_bot_set_field_24(int bot, int v)
{
    g_ai_bots[bot].active = v;
}

// FUNCTION: MFL 0x0041a000
void ai_bot_clear_flag_20(int bot)
{
    *(int *)((char *)&g_ai_bots[bot] + 0x40) = 0;
}
