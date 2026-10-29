#include "mfl_tier2.h"
/*00453a90*/ extern int course_smoke_particles_destroy();

/* Highway course callback: destroys the smoke particles. */
void course_highway_cb5_sub_453c10(void)

{
  course_smoke_particles_destroy();
}
