#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*006586e8*/ extern undefined g_load_6586e8;
/*00486710*/ extern int particles_destroy();
/*006586e8*/ extern undefined g_load_6586e8;
/* marker_particles_destroy */

/* "particles_destroy(0x6586E8)" */

void marker_particles_destroy(void)
{
  particles_destroy(&g_load_6586e8);
  return;
}
