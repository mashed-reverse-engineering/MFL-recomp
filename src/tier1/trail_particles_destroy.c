#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*00653258*/ extern undefined g_trail_653258;


/* trail_particles_destroy */

/* "particles_destroy(0x653258)" */

void trail_particles_destroy(void)
{
  particles_destroy(&g_trail_653258);
  return;
}
