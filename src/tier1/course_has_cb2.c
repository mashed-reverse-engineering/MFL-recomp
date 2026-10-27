#include "mfl_tier2.h"
/*00659d04*/ extern char *g_current_course_entry;


/* course_has_cb2 */

/*  */

undefined4 course_has_cb2(void)
{
  return *(undefined4 *)(g_current_course_entry + 0x1c);
}
