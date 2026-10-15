/* functions of batch9.c that use a name declared with a different type in another source (tools/reorg.py); the preamble of batch9.c is kept as it was */
#pragma auto_inline(off)
#include "mfl_consts.h"
#include "mfl_types.h"
extern float g_course_69bb6c;
extern int g_course_69bb64, g_course_69bb5c, g_course_8c879c;
extern int g_camera_fov_factor, g_ui_69cbe8[12];
extern int controller_present(int);
extern void message_show_now(int, int, int, int, int, int);
extern void RwTexDictionaryDestroy(void *);
extern int RpWorldDestroy(void *);
extern void *g_frontend_world_69cbe4, *g_frontend_world_69cbd8;
extern int g_message_state, g_message_code, g_hit_shake;
extern int g_course_dem_6a07ac, g_course_dem_6a07b0, g_course_dem_6a07b4, g_course_dem_6a07b8, g_course_dem_6a07bc, g_course_dem_6a07c0;
extern int g_course_dem_60d808, g_course_dem_60d80c;
extern int g_menu_current;
extern struct MenuRecord g_menu_records[];
extern int g_game_state_2812;
extern struct MenuItemAnim g_menu_item_anims[30];
extern int FID_conflict__wprintf(const char *, ...);
extern int get_event_type(void);
extern int g_car_8c7460;
extern void menu_event_0x11_reset_page(int);
/* prototypes of functions that live in other modules */


// FUNCTION: MFL 0x0044c560
void menu_set_car_8c7460_and_reset(int v)
{
    g_car_8c7460 = v;
    if (get_event_type() == 0x11 && v == 0)
        menu_event_0x11_reset_page(0);
}
