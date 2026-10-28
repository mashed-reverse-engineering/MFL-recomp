#include "mfl_tier2.h"
/* gap_5b9c10 */

/* Listener3D::set_top(const RwV3d *v): stores the vector, recomputes side = front x top (offsets 0x40 front, 0x4c top, 0x58 side), sets dirty bit 4 = LISTENER3D_DIRTY_TOP.
   Kept as raw offsets: typed Listener3D fields change the load order of the cross product. */

void gap_5b9c10(char *listener,char *unused_2,float *top_in)

{
  float *top_x;
  
  top_x = (float *)(listener + 0x4c);
  *top_x = *top_in;
  *(float *)(listener + 0x50) = top_in[1];
  *(float *)(listener + 0x54) = top_in[2];
  *(float *)(listener + 0x58) =
       *(float *)(listener + 0x44) * *(float *)(listener + 0x54) -
       *(float *)(listener + 0x48) * *(float *)(listener + 0x50);
  *(float *)(listener + 0x5c) =
       *top_x * *(float *)(listener + 0x48) -
       *(float *)(listener + 0x40) * *(float *)(listener + 0x54);
  *(uint *)(listener + 0x70) = *(uint *)(listener + 0x70) | 4;
  *(float *)(listener + 0x60) =
       *(float *)(listener + 0x40) * *(float *)(listener + 0x50) -
       *top_x * *(float *)(listener + 0x44);
  return;
}


