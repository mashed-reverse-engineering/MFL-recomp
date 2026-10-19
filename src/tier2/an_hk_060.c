#include "mfl_tier2.h"
/*004b4ba7*/ extern int entry();
/*00659d04*/ extern undefined4 g_current_course_entry;
#define COURSE_ENTRY_UPDATE_SLOT (0x18)
/* course_call_cb1 */

/* "jmp [entry+0x18]: per frame update, called by course_update" */

/* Per-frame update for the current course: forwards both arguments unchanged to the
 * update slot (COURSE_ENTRY_UPDATE_SLOT) of the course entry in g_current_course_entry.
 * update_arg_1 / update_arg_2: the two pointers course_update passes on (meaning inferred). */

void __cdecl course_call_cb1(char *update_arg_1,char *update_arg_2)

{
                    /* WARNING: Could not recover jumptable at 0x004219b6. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(g_current_course_entry + COURSE_ENTRY_UPDATE_SLOT))(update_arg_1,update_arg_2);
  return;
}


