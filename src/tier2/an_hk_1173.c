#include "mfl_tier2.h"
/* gap_5b9b90 */

/* Listener3D::set_front(const RwV3d *v): stores the vector, recomputes side = front x top (offsets 0x40 front, 0x4c top, 0x58 side), sets dirty bit 8 = LISTENER3D_DIRTY_FRONT.
   Kept as raw offsets: typed Listener3D fields change the load order of the cross product. */

void gap_5b9b90(char *param_1,char *unused_2,float *param_3)

{
  float *pfVar1;
  
  pfVar1 = (float *)(param_1 + 0x40);
  *pfVar1 = *param_3;
  *(float *)(param_1 + 0x44) = param_3[1];
  *(float *)(param_1 + 0x48) = param_3[2];
  *(float *)(param_1 + 0x58) =
       *(float *)(param_1 + 0x44) * *(float *)(param_1 + 0x54) -
       *(float *)(param_1 + 0x48) * *(float *)(param_1 + 0x50);
  *(float *)(param_1 + 0x5c) =
       *(float *)(param_1 + 0x4c) * *(float *)(param_1 + 0x48) -
       *pfVar1 * *(float *)(param_1 + 0x54);
  *(uint *)(param_1 + 0x70) = *(uint *)(param_1 + 0x70) | 8;
  *(float *)(param_1 + 0x60) =
       *pfVar1 * *(float *)(param_1 + 0x50) -
       *(float *)(param_1 + 0x4c) * *(float *)(param_1 + 0x44);
  return;
}


