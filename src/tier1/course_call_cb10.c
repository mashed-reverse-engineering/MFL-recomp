#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;
/* byte offset in the course entry table of callback slot 10 */
#define COURSE_ENTRY_CB10_OFFSET (0x3c)
/* course_call_cb10 */
/* "jmp [entry+0x3C]": forwards callback_arg to callback slot 10 of the current course entry table
 * (opaque pointer, the caller passes the address of the course world). Returns nothing. (inferred) */
void __cdecl course_call_cb10(char *callback_arg)
{
                    /* WARNING: Could not recover jumptable at 0x00421a46. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(g_current_course_entry + COURSE_ENTRY_CB10_OFFSET))(callback_arg);
  return;
}
