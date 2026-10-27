#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;


/* course_has_world_setup */

/* "returns the callback at +0x44 of the current record (cb12)" */

undefined4 course_has_world_setup(void)
{
  return *(undefined4 *)(g_current_course_entry + 0x44);
}
