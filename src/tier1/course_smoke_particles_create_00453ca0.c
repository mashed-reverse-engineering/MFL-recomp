#include "mfl_tier2.h"
/*00453c30*/ extern int course_smoke_particles_create(void);
/* Duplicate entry point that tail-calls course_smoke_particles_create. */
int course_smoke_particles_create_00453ca0(void)
{
  return course_smoke_particles_create();
}
