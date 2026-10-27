#include "mfl_tier2.h"
extern char g_split_sectors[];


/* course_get_68b8b4_ptr */

/* "returns 0x68B8B4" */

undefined4 course_get_68b8b4_ptr(void)
{
  return ((int)&g_split_sectors);
}
