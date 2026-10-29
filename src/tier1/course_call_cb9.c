#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
#define COURSE_ENTRY_CB9_OFFSET (0x38)
/* Forwards callback_arg to the current course's callback at COURSE_ENTRY_CB9_OFFSET inside g_current_course_entry (inferred). */
/*
 * course_call_cb9: forwards callback_arg to the current course's callback
 * at COURSE_ENTRY_CB9_OFFSET inside g_current_course_entry (inferred).
 * Returns nothing; the callee's result is not read here.
 */
/* "jmp [entry+0x38]" */
void __cdecl course_call_cb9(char *callback_arg)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB9_OFFSET))(callback_arg);
  return;
}
