#include "mfl_tier2.h"
/*00486710*/ extern int particles_destroy();
/*00759658*/ extern undefined g_fx_dust_759658;


/* fx_dust_destroy */

/* "particles_destroy(&g_particles_759658)" */

void fx_dust_destroy(void)
{
  particles_destroy(&g_fx_dust_759658);
  return;
}
