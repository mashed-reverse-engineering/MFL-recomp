#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset of the course callback pointer inside the current course entry (inferred) */
#define COURSE_ENTRY_CALLBACK_OFFSET (0x40)
/* course_call_cb11
 * Forwards `data` to the current course's callback (the pointer at COURSE_ENTRY_CALLBACK_OFFSET
 * in g_current_course_entry). Does not return a value. `data` is passed through unchanged;
 * its meaning is set by the course callback (inferred: opaque course-call data).
 */
/* "jmp [entry+0x40]" */
void __cdecl course_call_cb11(char *data)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CALLBACK_OFFSET))(data);
  return;
}
