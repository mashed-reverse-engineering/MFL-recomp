#include "mfl_tier2.h"
/* gap_5b9a40 */

/* Listener3D position setter: copies *v, marks LISTENER3D_DIRTY_POSITION. */

void gap_5b9a40(Listener3D *listener,char *unused_2,const RwV3d *v)
{
  listener->position.x = v->x;
  listener->position.y = v->y;
  listener->position.z = v->z;
  listener->dirty |= LISTENER3D_DIRTY_POSITION;
  return;
}


