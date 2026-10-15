/* MFL.exe career: functions moved verbatim from batch9.c by tools/reorg.py (shared declarations: mfl_externs.h). */
#pragma auto_inline(off)
#include "mfl_types.h"
#include "mfl_externs.h"
extern char g_game_state_records_0_[];

// FUNCTION: MFL 0x004357e0
int career_record_ptr(int course, int on)
{
    if (on == 0)
        return 0;
    FID_conflict__wprintf("courseid=%d\n", course);
    return course * 0x14 + ((int)&g_game_state_records_0_);
}
