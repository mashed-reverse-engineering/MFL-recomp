/* MFL.exe career_np: functions moved verbatim from game_globals.c, misc3.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#include "mfl_types.h"
#include "mfl_externs.h"

// FUNCTION: MFL 0x0042fb20
void career_event_won_set_course_state(int value)
{
    g_menu_8c8090 = 0x40;
    g_course_8c8724 = value;
}

// FUNCTION: MFL 0x004169f0
char *career_event_slot_table(void)
{
    if (get_event_type() == 0xe)
        return g_career_event_608d38;
    if (get_event_type() == 0xf)
        return g_career_event_608e28;
    if (get_event_type() == 0x10)
        return g_career_event_608f18;
    return g_career_event_608a68;
}
