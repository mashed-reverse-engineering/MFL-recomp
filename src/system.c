/* MFL.exe system: functions moved verbatim from batch24.c, batch25.c, batch5.c, batch9.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x004a1860
void game_set_option_flags(unsigned char a, unsigned char b, unsigned char c)
{
    g_game_62ad2c = a;
    g_game_set_62ad2d = b;
    g_game_set_62ad2e = c;
}

// FUNCTION: MFL 0x004a34b0
void app_get_two_size_pairs(int *a, int *b)
{
    if (a != 0) {
        a[0] = g_app_76e878;
        a[1] = g_app_76e87c;
    }
    if (b != 0) {
        b[0] = g_app_76e880;
        b[1] = g_app_76e884;
    }
}

// FUNCTION: MFL 0x004a16e0
void input_player_rumble_set(int p)
{
    int *q = (int *)((char *)&g_player_inputs[p] + 0x20);
    if (q[1] != 0) {
        q[0] = 1;
        q[4] = 3;
        q[3] = 10;
        q[5] = 0xff;
    }
}

// FUNCTION: MFL 0x004a29a0
void app_enter_frontend(int mode)
{
    menu_state_restore();
    frontend_enter_menu(mode);
    g_app_state = APP_STATE_FRONTEND;
    g_app_76e800 = 0;
}

// FUNCTION: MFL 0x004a46d0
void dinput_release_object(void)
{
    int *obj = g_dinput_76ed10;
    if (g_dinput_76ed10 != 0) {
        debug_printf("Releasing object\n");
        (*(void (**)(int *))(*obj + 8))(obj);
    }
    g_dinput_76ed10 = 0;
}

// FUNCTION: MFL 0x0040b050
void gamesave_message_show_pending(void)
{
    switch (g_gamesave_message_8d8bfc) {
    case 1:
        message_show(0x293, 0xff460000, 2, 0x2f, 0x30, 0);
        g_gamesave_message_8d8bfc = 0;
        break;
    }
}

// FUNCTION: MFL 0x0040ae80
void loading_start_log(void)
{
    FID_conflict__wprintf("Load start\n");
    g_gamesave_operation = 2;
    g_gamesave_load_step = 1;
    g_gamesave_status_poll_count = 0;
}

// FUNCTION: MFL 0x0040af20
void autosave_request(void)
{
    if (g_gamesave_flag_c0c != 0) {
        FID_conflict__wprintf("starting autosave\n");
        g_gamesave_operation = 4;
        g_autosave_step = 1;
        g_gamesave_status_poll_count = 0;
    }
}

// FUNCTION: MFL 0x0040af70
void gamesave_ui_load_done(int code)
{
    g_gamesave_load_step = 5;
    g_gamesave_format_step = 3;
    g_gamesave_result_code = code;
    message_close();
}

// FUNCTION: MFL 0x0040afa0
void gamesave_ui_save_done(int code)
{
    g_gamesave_save_step = 5;
    g_gamesave_result_code = code;
    message_close();
}

// FUNCTION: MFL 0x0040afc0
void gamesave_ui_autosave_done(int code)
{
    g_autosave_step = 5;
    g_gamesave_result_code = code;
    message_close();
}

// FUNCTION: MFL 0x0040afe0
void gamesave_ui_format_done(int code)
{
    g_gamesave_delete_step = 5;
    g_gamesave_result_code = code;
    message_close();
}

// FUNCTION: MFL 0x00431a50
int controllers_count_present(void)
{
    int n = 0;
    int i = 0;
    do {
        if (controller_present(i) != 0)
            n++;
        i++;
    } while (i < 6);
    return n == 0;
}

// FUNCTION: MFL 0x00431ae0
void message_show_controller_missing(void)
{
    message_show_now(MSG_CONTROLLER_MISSING, 0xff210000, 0, 0, 0, 0);
}

// FUNCTION: MFL 0x00431b00
void frontend_world_destroy(void)
{
    if (g_frontend_world_69cbe4 != 0)
        RwTexDictionaryDestroy(g_frontend_world_69cbe4);
    if (g_frontend_world_69cbd8 != 0)
        RpWorldDestroy(g_frontend_world_69cbd8);
}

// FUNCTION: MFL 0x00437ee0
int message_is_controller_missing_shown(void)
{
    if (g_message_state != 0 && (g_message_code == MSGCODE_CONTROLLER_MISSING_A || g_message_code == MSGCODE_CONTROLLER_MISSING_B))
        return 1;
    return 0;
}
