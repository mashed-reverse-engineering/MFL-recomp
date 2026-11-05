/* rwp_box_0056d850 (0x0056d850): RwpBox vtable slot (5f6504): full length of the box along its longest axis */
#include "mfl_tier2.h"

/* Box half sizes at +0x40/+0x44/+0x48.  The three pairwise compares form a 3 bit code; the nibble table 0x21312300 maps it to
   the index of the largest half size (ties keep the later axis).  Returns twice that half size. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
float rwp_box_0056d850(float *box)
{
  int xy, xz, yz, code, k;

  xy = box[16] > box[17];
  xz = box[16] > box[18];
  yz = box[17] > box[18];
  code = ((xy * 2 | xz) * 2 | yz) * 4;
  k = (0x21312300 >> code) & 3;
  return box[16 + k] + box[16 + k];
}
