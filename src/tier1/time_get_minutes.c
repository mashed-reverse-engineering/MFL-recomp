#include "mfl_tier2.h"
/*0069bb5c*/ extern undefined4 g_course_69bb5c;


/* time_get_minutes */

/* Minutes of time slot `index`. */
/* "(index): minutes of the time slot" */

undefined4 time_get_minutes(int index)
{
  return (&g_course_69bb5c)[index];
}
