#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
#define COURSE_ENTRY_CB2_OFFSET (0x1c)
/* course_call_cb2 */
/* "jmp [entry+0x1C]: called by course_render" */
/* Forwards callback_arg to the course entry's callback at COURSE_ENTRY_CB2_OFFSET (inferred: a per-course hook called by course_render).
   Returns nothing. */
void __cdecl course_call_cb2(char *callback_arg)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB2_OFFSET))(callback_arg);
  return;
}
