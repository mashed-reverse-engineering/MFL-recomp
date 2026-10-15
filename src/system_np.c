/* MFL.exe system_np: functions moved verbatim from game_globals.c, game_misc.c, game_misc2.c, misc3.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0040ae30
void gamesave_request_load_slot1(void)
{
    g_gamesave_operation = 1;
    g_gamesave_format_step = 1;
    g_gamesave_status_poll_count = 0;
    g_gamesave_flag_c0c = 0;
}

// FUNCTION: MFL 0x0040ae50
void gamesave_request_format(void)
{
    g_gamesave_operation = 1;
    g_gamesave_format_step = 0xc;
    g_gamesave_status_poll_count = 0;
    g_gamesave_flag_c0c = 0;
}

// FUNCTION: MFL 0x0040aeb0
void gamesave_request_save_a(void)
{
    g_gamesave_operation = 3;
    g_gamesave_save_step = 1;
    g_gamesave_status_poll_count = 0;
}

// FUNCTION: MFL 0x0040aed0
void gamesave_request_save_b(void)
{
    g_gamesave_operation = 3;
    g_gamesave_save_step = 3;
    g_gamesave_status_poll_count = 0;
}

// FUNCTION: MFL 0x0040aef0
void gamesave_request_delete(void)
{
    g_gamesave_previous_operation = g_gamesave_operation;
    g_gamesave_operation = 5;
    g_gamesave_delete_step = 1;
    g_gamesave_status_poll_count = 0;
}

// FUNCTION: MFL 0x00405e20
int gamesave_exists(void)
{
    return FUN_00561060(g_gamesave_5dd9b4) != 0;
}

// FUNCTION: MFL 0x004316f0
void message_close(void)
{
    if (g_message_state == 2)
        g_message_state = 3;
}

// FUNCTION: MFL 0x0040bfb0
void gamesave_cb_load_state(void)
{
    switch (g_gamesave_load_step) {
    case 0:
    case 5:
        g_gamesave_operation = 0;
        break;
    case 7:
        g_gamesave_load_step = 2;
        return;
    }
}
