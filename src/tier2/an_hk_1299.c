#include "mfl_tier2.h"
/*005ec3d4*/ extern float k_f_0_9999;
/*005f8f4c*/ extern float k_f_n0_9999;
/* gap_5d7b30 */

/* no context */

/* Fills out_bounds (min xyz at [0..2], max xyz at [4..6]) with +-1.7014117e+38 (float "infinity"), then clamps one side from the dominant axis of contact_normal (|n|>0.9999) using margin = box_owner+0x4c and the position at contact_normal+0x30 (inferred). */
float * gap_5d7b30(char *box_owner,char *contact_normal,char *unused_arg,float *out_bounds)

{
  float margin;
  
  margin = *(float *)(box_owner + 0x4c);
  *out_bounds = 1.7014117e+38;
  out_bounds[1] = 1.7014117e+38;
  out_bounds[2] = 1.7014117e+38;
  out_bounds[4] = -1.7014117e+38;
  out_bounds[5] = -1.7014117e+38;
  out_bounds[6] = -1.7014117e+38;
  if (contact_normal != 0) {
    if (k_f_0_9999 < *(float *)(contact_normal + 0x20)) {
      *out_bounds = margin + *(float *)(contact_normal + 0x30);
      return out_bounds;
    }
    if (*(float *)(contact_normal + 0x20) < k_f_n0_9999) {
      out_bounds[4] = *(float *)(contact_normal + 0x30) - margin;
      return out_bounds;
    }
    if (k_f_0_9999 < *(float *)(contact_normal + 0x24)) {
      out_bounds[1] = margin + *(float *)(contact_normal + 0x34);
      return out_bounds;
    }
    if (*(float *)(contact_normal + 0x24) < k_f_n0_9999) {
      out_bounds[5] = *(float *)(contact_normal + 0x34) - margin;
      return out_bounds;
    }
    if (!(*(float *)(contact_normal + 0x28) > k_f_0_9999)) {      /* NaN takes this branch in the original (and eax,0x4100; jne) */
      if (*(float *)(contact_normal + 0x28) < k_f_n0_9999) {
        out_bounds[6] = *(float *)(contact_normal + 0x38) - margin;
      }
      return out_bounds;
    }
    margin = margin + *(float *)(contact_normal + 0x38);
  }
  out_bounds[2] = margin;
  return out_bounds;
}


