#include "mfl_tier2.h"
/* gap_5b9c70 */

/* Listener3D doppler factor setter. */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
void gap_5b9c70(Listener3D *listener,char *unused_2,const float *factor)
{
  listener->doppler_factor = *factor;
  listener->dirty |= LISTENER3D_DIRTY_DOPPLER;
  return;
}
