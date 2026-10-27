#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset of the world-setup hook in the current course entry table (inferred) */
#define COURSE_ENTRY_WORLD_SETUP_HOOK (0x44)
/* course_call_world_setup
 * Runs the world-setup hook of the current course entry, passing it world_setup_data
 * (inferred: opaque setup block for the course's world). Returns nothing.
 */
/* "jmp [entry+0x44] (cb12), before the world is created" */
void __cdecl course_call_world_setup(char *world_setup_data)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_WORLD_SETUP_HOOK))(world_setup_data);
  return;
}
