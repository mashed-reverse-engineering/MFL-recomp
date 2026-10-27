#include "mfl_tier2.h"
/*0069bb64*/ extern undefined4 g_course_69bb64;


/* time_get_seconds */

/* Seconds of time slot `index`. */
/* "(index): seconds of the time slot" */

undefined4 time_get_seconds(int index)
{
  return (&g_course_69bb64)[index];
}
