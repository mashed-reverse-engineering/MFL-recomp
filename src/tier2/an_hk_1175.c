#include "mfl_tier2.h"
/* gap_5b9c10 */

/* Listener3D::set_top(const RwV3d *v): stores the vector, recomputes side = front x top (offsets 0x40 front, 0x4c top, 0x58 side), sets dirty bit 4 = LISTENER3D_DIRTY_TOP.
   Kept as raw offsets: typed Listener3D fields change the load order of the cross product. */

void gap_5b9c10(char *param_1,char *unused_2,float *param_3)

{
  float *pfVar1;
  
  pfVar1 = (float *)(param_1 + 0x4c);
  *pfVar1 = *param_3;
  *(float *)(param_1 + 0x50) = param_3[1];
  *(float *)(param_1 + 0x54) = param_3[2];
  *(float *)(param_1 + 0x58) =
       *(float *)(param_1 + 0x44) * *(float *)(param_1 + 0x54) -
       *(float *)(param_1 + 0x48) * *(float *)(param_1 + 0x50);
  *(float *)(param_1 + 0x5c) =
       *pfVar1 * *(float *)(param_1 + 0x48) -
       *(float *)(param_1 + 0x40) * *(float *)(param_1 + 0x54);
  *(uint *)(param_1 + 0x70) = *(uint *)(param_1 + 0x70) | 4;
  *(float *)(param_1 + 0x60) =
       *(float *)(param_1 + 0x40) * *(float *)(param_1 + 0x50) -
       *pfVar1 * *(float *)(param_1 + 0x44);
  return;
}


