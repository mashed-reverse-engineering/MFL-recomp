#include "mfl_tier2.h"
/* gap_5b9cc0 */

/* Listener3D distance factor setter. */

void gap_5b9cc0(Listener3D *listener,char *unused_2,const float *factor)
{
  listener->distance_factor = *factor;
  listener->dirty |= LISTENER3D_DIRTY_DISTANCE;
  return;
}


