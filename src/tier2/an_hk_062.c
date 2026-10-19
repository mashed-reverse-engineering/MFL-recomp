#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*00659d04*/ extern char *g_current_course_entry;
/* course_call_cb3 */

#define COURSE_ENTRY_CB3_SLOT (0x20)

/* Forwards its argument to the course entry's callback 3 (inferred: the
 * function at COURSE_ENTRY_CB3_SLOT in the entry table). Called by course_render.
 * arg: passed through unchanged (inferred: a render-related pointer).
 * Returns nothing.
 */
void __cdecl course_call_cb3(char *arg)

{
                    /* WARNING: Could not recover jumptable at 0x004219d6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB3_SLOT))(arg);
  return;
}


