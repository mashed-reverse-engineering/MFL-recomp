#include "mfl_tier2.h"
/*0069bb6c*/ extern float g_course_69bb6c;


/* time_get_fraction */

/* "(index): fraction of the time slot" */

double time_get_fraction(int param_1)

{
  return (double)((float *)&g_course_69bb6c)[param_1];
}
