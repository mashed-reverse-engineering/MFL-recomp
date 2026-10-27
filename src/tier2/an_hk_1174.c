#include "mfl_tier2.h"
/* gap_5b9bf0 */

/* Listener3D front vector getter. */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
char *gap_5b9bf0(Listener3D *listener,char *unused_2,RwV3d *out)
{
  out->x = listener->front.x;
  out->y = listener->front.y;
  out->z = listener->front.z;
  return (char *)listener;
}


