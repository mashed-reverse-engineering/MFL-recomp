#include "mfl_tier2.h"
/* gap_5b9c90 */

/* Listener3D rolloff factor setter. */

void gap_5b9c90(Listener3D *listener,char *unused_2,const float *factor)
{
  listener->rolloff_factor = *factor;
  listener->dirty |= LISTENER3D_DIRTY_ROLLOFF;
  return;
}


