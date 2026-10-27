#include "mfl_tier2.h"
/*0040eb10*/ extern int race_load_course(void);
/* Duplicate entry point that tail-calls race_load_course. */
int race_load_course_0042fad0(void)
{
  return race_load_course();
}
