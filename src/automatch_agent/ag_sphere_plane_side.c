#include "mfl_tier2.h"


/* sphere_plane_side */

/* "(plane, sphere): 2 when the sphere is in front, 0 / 1 otherwise using the radius" */

undefined4 __cdecl sphere_plane_side(undefined4 plane,char *sphere)

{
  int side;
  float distance;
  
  side = plane_classify_point(plane,sphere,&distance);
  if (side == 0) {
    if (distance > *(float *)(sphere + 0xc)) {
      return 0;
    }
  }
  else if ((side == 2) && (distance > *(float *)(sphere + 0xc))) {
    return 2;
  }
  return 1;
}
