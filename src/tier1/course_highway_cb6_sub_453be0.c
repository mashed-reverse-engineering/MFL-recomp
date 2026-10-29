#include "mfl_tier2.h"
/*00453980*/ extern int course_particle_timers_reset();

/* Highway course callback: resets the particle timers. */
void course_highway_cb6_sub_453be0(void)

{
  course_particle_timers_reset();
}
