#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
#define COURSE_ENTRY_CB5_OFFSET (0x28)
/* course_call_cb5 */
/* "jmp [entry+0x28]: called by course_unload before the world is destroyed" */
/* Forwards to the current course entry's cb5 hook (entry + COURSE_ENTRY_CB5_OFFSET),
   called by course_unload before the world is destroyed.
   unload_context: the char * passed through to the hook (inferred). Returns nothing. */
void __cdecl course_call_cb5(char *unload_context)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB5_OFFSET))(unload_context);
  return;
}
