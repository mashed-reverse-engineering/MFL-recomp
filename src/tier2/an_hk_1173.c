#include "mfl_tier2.h"
/* gap_5b9b90 */

/* Listener3D::set_front(const RwV3d *v): stores the vector, recomputes side = front x top (offsets 0x40 front, 0x4c top, 0x58 side), sets dirty bit 8 = LISTENER3D_DIRTY_FRONT.
   Kept as raw offsets: typed Listener3D fields change the load order of the cross product. */

void gap_5b9b90(char *listener,char *unused_2,float *new_front)

{
  float *front_xyz;
  
  front_xyz = (float *)(listener + 0x40);
  *front_xyz = *new_front;
  *(float *)(listener + 0x44) = new_front[1];
  *(float *)(listener + 0x48) = new_front[2];
  *(float *)(listener + 0x58) =
       *(float *)(listener + 0x44) * *(float *)(listener + 0x54) -
       *(float *)(listener + 0x48) * *(float *)(listener + 0x50);
  *(float *)(listener + 0x5c) =
       *(float *)(listener + 0x4c) * *(float *)(listener + 0x48) -
       *front_xyz * *(float *)(listener + 0x54);
  *(uint *)(listener + 0x70) = *(uint *)(listener + 0x70) | 8;
  *(float *)(listener + 0x60) =
       *front_xyz * *(float *)(listener + 0x50) -
       *(float *)(listener + 0x4c) * *(float *)(listener + 0x44);
  return;
}


