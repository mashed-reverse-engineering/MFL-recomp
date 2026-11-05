/* rwp_box_0056d790 (0x0056d790): RwpBox vtable slot (5f64fc): the box's three face axes as (axis, -half, +half) intervals */
#include "mfl_tier2.h"

typedef struct MflV3 { float x, y, z; } MflV3;
typedef struct MflAxisRow { MflV3 v; float pad; } MflAxisRow;
typedef struct MflAxisInterval { MflV3 axis; float lo, hi; } MflAxisInterval;

/* Box half sizes at +0x40/+0x44/+0x48; `axes` is the box orientation (3 rows of 16 bytes).  For every axis k: iv[k] = (axis k,
   -half k, +half k) and dirs[k] = axis k.  Returns the number of axis groups written as 1. */
/* TIER2: unvalidated 2026-11-05 authority=diffemu */
int rwp_box_0056d790(float *box, MflAxisRow *axes, MflAxisInterval *iv, MflV3 *dirs)
{
  int k;
  float h;

  for (k = 0; k < 3; k++) {
    iv[k].axis = axes[k].v;
    h = box[16 + k];
    iv[k].hi = h;
    iv[k].lo = -h;
    dirs[k] = axes[k].v;
  }
  return 1;
}
