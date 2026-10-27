#include "mfl_tier2.h"
/*00486c40*/ extern int particles_flush();
/*00759658*/ extern undefined g_fx_dust_759658;


/* fx_dust_flush */

/* "particles_flush(&g_particles_759658)" */

void fx_dust_flush(void)
{
  particles_flush(&g_fx_dust_759658);
  return;
}
