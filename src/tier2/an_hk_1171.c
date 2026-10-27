#include "mfl_tier2.h"
/* gap_5b9ae0 */

/* Listener3D orientation setter from a matrix (up row = front, at row = top, right row = side). */

/* TIER2: unvalidated 2026-10-27 authority=diffemu */
void gap_5b9ae0(Listener3D *listener,char *unused_2,const RwMatrix *orientation)
{
  listener->front.x = orientation->up.x;
  listener->front.y = orientation->up.y;
  listener->front.z = orientation->up.z;
  listener->top.x = orientation->at.x;
  listener->top.y = orientation->at.y;
  listener->top.z = orientation->at.z;
  listener->side.x = orientation->right.x;
  listener->side.y = orientation->right.y;
  listener->side.z = orientation->right.z;
  listener->dirty |= LISTENER3D_DIRTY_TOP | LISTENER3D_DIRTY_FRONT;
  return;
}


