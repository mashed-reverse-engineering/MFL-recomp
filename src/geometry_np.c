/* MFL.exe geometry_np: functions moved verbatim from game_misc.c, game_misc2.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0040ac10
void line_exceptions_clear(void)
{
    int i;
    for (i = 0; i < 0x480; i++)
        g_line_exceptions[i] = -1.0f;
}

// FUNCTION: MFL 0x00474910
void spatial_grid_clear(void)
{
    int i;
    for (i = 0; i < 0x7f; i++)
        g_car_6c2688[i] = 0;
    g_car_6c28c0 = 0;
}

// FUNCTION: MFL 0x00474ac0
int spatial_grid_get(int x, int y)
{
    return g_car_6c2884[g_car_6c28b4 * y + x];
}

// FUNCTION: MFL 0x00474ba0
void spatial_grid_set(int x, int y, int value)
{
    g_car_6c2884[g_car_6c28b4 * y + x] = value;
}
