#include "mfl_tier2.h"
/*00486ef0*/ extern int particles_render();
/*006a07d8*/ extern undefined g_course_smoke_6a07d8;


/* course_smoke_particles_render */

/* "particles_render(&g_particles_6a07d8)" */

void course_smoke_particles_render(void)
{
  particles_render(&g_course_smoke_6a07d8);
  return;
}
