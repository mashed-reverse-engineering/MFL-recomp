#include "mfl_tier2.h"
/* gap_5b9a70 */

/* Listener3D position getter: copies the position to *out, returns the listener. */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
char *gap_5b9a70(Listener3D *listener,char *unused_2,RwV3d *out)
{
  out->x = listener->position.x;
  out->y = listener->position.y;
  out->z = listener->position.z;
  return (char *)listener;
}


