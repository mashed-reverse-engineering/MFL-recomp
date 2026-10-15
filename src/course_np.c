/* MFL.exe course_np: functions moved verbatim from game_globals.c, game_misc.c, game_misc2.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_course_lap_data_field49624_0xc1d8[];

// FUNCTION: MFL 0x0042e940
void course_banner_state_clear(void)
{
    g_intro_banner_text_id = 0.0f;
    g_intro_banner_slide_step = 0.0f;
}

// FUNCTION: MFL 0x0042fb40
int course_event_icon_lookup_a(int slot)
{
    int course = race_setup_get_course(slot);
    int i = 0;
    int key = g_menu_draw_608790[0];
    while (1) {
        if (key == -1)
            return 0;
        if (key == course)
            break;
        key = g_menu_draw_608790[i + 3];
        i += 3;
    }
    return g_menu_draw_608790[i + 1];
}

// FUNCTION: MFL 0x0042fbb0
int course_event_icon_lookup_b(int slot)
{
    int course = race_setup_get_course(slot);
    int i = 0;
    int key = g_menu_draw_608790[0];
    while (1) {
        if (key == -1)
            return 0;
        if (key == course)
            break;
        key = g_menu_draw_608790[i + 3];
        i += 3;
    }
    return g_menu_draw_608790[i + 2];
}

// FUNCTION: MFL 0x00421aa0
int course_select(int id)
{
    return *(int *)(g_current_course_entry + 0x10) == id;
}

int course_get_record_point(int record, int point)
{
    return (int)((char *)&g_course_lap_data_field49624_0xc1d8 + record * 0x4c + point * 0xc);
}
