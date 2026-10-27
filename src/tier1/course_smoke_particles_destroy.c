#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006a07d8*/ extern undefined g_course_smoke_6a07d8;


/* course_smoke_particles_destroy */

/* "particles_destroy(&g_particles_6a07d8)" */

void course_smoke_particles_destroy(void)
{
  particles_destroy(&g_course_smoke_6a07d8);
  return;
}
