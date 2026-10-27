#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
#define COURSE_ENTRY_CB4_OFFSET (0x24)
/*
 * course_call_cb4: calls the course entry's callback slot (+COURSE_ENTRY_CB4_OFFSET)
 * with cb_arg. Called by course_race_start. (inferred: cb_arg is passed through unchanged)
 */
/* "jmp [entry+0x24]: called by course_race_start" */
void __cdecl course_call_cb4(char *cb_arg)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB4_OFFSET))(cb_arg);
  return;
}
