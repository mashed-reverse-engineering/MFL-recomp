#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset in the course entry table of the post load callback pointer */
#define ENTRY_POST_LOAD_CB_OFFSET (0x14)
/* course_call_cb0
 * Runs the current course's post load callback (entry table slot at ENTRY_POST_LOAD_CB_OFFSET),
 * passing it callback_arg. Called by course_open_and_run_scripts.
 * callback_arg: opaque pointer forwarded unchanged to the callback (meaning inferred).
 * Returns nothing.
 */
/* "jmp [entry+0x14]: post load, called by course_open_and_run_scripts" */
void __cdecl course_call_cb0(char *callback_arg)
{
                    /* WARNING: Could not recover jumptable at 0x004219a6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(g_current_course_entry + ENTRY_POST_LOAD_CB_OFFSET))(callback_arg);
  return;
}
