/* functions of batch24.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch24.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_consts.h"
#include "mfl_types.h"
extern unsigned char g_game_62ad2c, g_game_set_62ad2d, g_game_set_62ad2e;
extern int g_app_76e878, g_app_76e87c, g_app_76e880, g_app_76e884;
extern struct PlayerInput g_player_inputs[];
extern int g_app_76e3c0, g_app_state, g_app_76e800;
extern unsigned char *g_app_76e3bc;
extern unsigned char g_app_62c5d0, g_app_62c5d1, g_app_62c5d2, g_app_62c5d3;
extern int *g_dinput_76ed10;
extern void fx_buffers_alloc(void);
extern void ui_colour_a_default(void);
extern void menu_state_restore(void);
extern void frontend_enter_menu(int);
extern void debug_printf(const char *, ...);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x004a0600
void rain_buffers_init_colours(void)
{
    unsigned char *p = g_app_76e3bc + 0x1d;
    int n = 4;
    int i;
    do {
        i = 0xe0;
        do {
            p[-1] = g_app_62c5d0;
            p[0] = g_app_62c5d2;
            p[1] = g_app_62c5d1;
            p[2] = g_app_62c5d3;
            p[-0xf] = 0;
            p += 0x20;
            i--;
        } while (i != 0);
        n--;
    } while (n != 0);
}
