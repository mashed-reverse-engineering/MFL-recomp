#include "mfl_tier2.h"
/* gap_5b9ac0 */

/* Listener3D velocity getter: copies the velocity to *out, returns the listener. */

char *gap_5b9ac0(Listener3D *listener,char *unused_2,RwV3d *out)
{
  out->x = listener->velocity.x;
  out->y = listener->velocity.y;
  out->z = listener->velocity.z;
  return (char *)listener;
}


