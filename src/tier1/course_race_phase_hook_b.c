#include "mfl_tier2.h"
/*004584c0*/ extern int course_race_phase_hook(void);
int course_race_phase_hook_b(void)
{
  return course_race_phase_hook();
}
