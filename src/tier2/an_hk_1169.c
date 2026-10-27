#include "mfl_tier2.h"
/* gap_5b9a90 */

/* Listener3D velocity setter: copies *v, marks LISTENER3D_DIRTY_VELOCITY. */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
void gap_5b9a90(Listener3D *listener,char *unused_2,const RwV3d *v)
{
  listener->velocity.x = v->x;
  listener->velocity.y = v->y;
  listener->velocity.z = v->z;
  listener->dirty |= LISTENER3D_DIRTY_VELOCITY;
  return;
}


