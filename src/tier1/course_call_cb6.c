#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset in the current course entry of the callback function pointer (inferred) */
#define COURSE_ENTRY_CALLBACK_OFFSET (0x2c)
/* course_call_cb6 */
/* "jmp [entry+0x2C]" */
/*
 * Forwards its argument to the current course's callback: calls the function
 * pointer at COURSE_ENTRY_CALLBACK_OFFSET in g_current_course_entry, passing
 * callback_arg unchanged. Returns nothing. (inferred: what the callback does
 * with its argument is not known from this function.)
 */
void __cdecl course_call_cb6(char *callback_arg)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CALLBACK_OFFSET))(callback_arg);
  return;
}
