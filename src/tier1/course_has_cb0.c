#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;


/* course_has_cb0 */

/* "[entry+0x14] != 0" */

undefined4 course_has_cb0(void)
{
  return *(undefined4 *)(g_current_course_entry + 0x14);
}
