/* MFL.exe lua: functions moved verbatim from batch15.c, batch23.c, batch24.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0048bc60
int lua_Lap_Line_End(void)
{
    int n = g_lap_line_count;
    g_course_lap->lap_lines[n] = LAP_LINE_END;
    g_lap_line_count = n + 1;
    return 0;
}

// FUNCTION: MFL 0x0048bc80
int lua_Lap_Line_Change(void)
{
    int n = g_lap_line_count;
    g_course_lap->lap_lines[n] = LAP_LINE_CHANGE;
    g_lap_line_count = n + 1;
    return 0;
}

// FUNCTION: MFL 0x0048ce30
void lua_init(void)
{
    g_lua_state = lua_open(0);
    lua_mathlib_open(g_lua_state);
}

// FUNCTION: MFL 0x004a0540
void lua_RainSetPosition_impl(int a, int b, int c, int d, int e, int f)
{
    g_app_62ac3c = a;
    g_app_62ac40 = b;
    g_app_76e3c4 = c;
    g_app_76e3c8 = d;
    g_app_62ac44 = e;
    g_app_62ac48 = f;
}

// FUNCTION: MFL 0x004a0580
void lua_RainSetScale_impl(int a, int b)
{
    g_app_62ac30 = b;
    g_app_62ac2c = a;
}

// FUNCTION: MFL 0x004a05a0
void lua_RainSetCameraScale_impl(int a, int b)
{
    g_app_62ac34 = a;
    g_app_62ac38 = b;
}

// FUNCTION: MFL 0x004a05c0
void lua_RainSetDirection_impl(int a, int b, int c, int d, int e, int f)
{
    g_app_62ac4c = a;
    g_app_62ac50 = b;
    g_app_62ac54 = c;
    g_app_62ac58 = d;
    g_app_62ac5c = e;
    g_app_62ac60 = f;
}

// FUNCTION: MFL 0x004a0ac0
int lua_RainEnable(void)
{
    ui_colour_a_default();
    g_app_76e3c0 = 1;
    return 0;
}
