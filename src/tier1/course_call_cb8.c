#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset of the callback slot in the current course entry table (inferred) */
#define COURSE_ENTRY_CB8_OFFSET (0x34)
/* course_call_cb8 */
/* "jmp [entry+0x34]" */
/* Forwards callback_arg to callback slot 8 of the current course entry table
   (the function pointer at COURSE_ENTRY_CB8_OFFSET). Returns nothing. (inferred) */
void __cdecl course_call_cb8(char *callback_arg)
{
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB8_OFFSET))(callback_arg);
  return;
}
