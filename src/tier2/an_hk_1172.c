#include "mfl_tier2.h"
/* gap_5b9b40 */

/* Listener3D orientation getter into a matrix (up row = front, at row = top, right row = side). */

char *gap_5b9b40(Listener3D *listener,char *unused_2,RwMatrix *out)
{
  out->up.x = listener->front.x;
  out->up.y = listener->front.y;
  out->up.z = listener->front.z;
  out->at.x = listener->top.x;
  out->at.y = listener->top.y;
  out->at.z = listener->top.z;
  out->right.x = listener->side.x;
  out->right.y = listener->side.y;
  out->right.z = listener->side.z;
  return (char *)listener;
}


