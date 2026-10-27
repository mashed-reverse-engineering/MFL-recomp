#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* Byte offset in the current course entry of the function pointer that course_call_cb7 calls (inferred: course callback slot). */
#define COURSE_ENTRY_CALLBACK_OFFSET (0x30)
/*
 * course_call_cb7: forwards arg unchanged to the current course's callback
 * (inferred: course entry slot at COURSE_ENTRY_CALLBACK_OFFSET). Returns nothing.
 *   arg: opaque pointer passed through to the callback (inferred: char *).
 */
/* "jmp [entry+0x30]" */
void __cdecl course_call_cb7(char *arg)
{
                    /* WARNING: Could not recover jumptable at 0x00421a16. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CALLBACK_OFFSET))(arg);
  return;
}
