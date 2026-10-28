#include "mfl_tier2.h"
/* gap_5c8bf0 */

/* no context */

/* Fills slot[0..3] from the current entry (obj+0x1a4, index obj+0x1ac, stride 0x14) according to slot[1] kind 1/2/other (inferred). */
void gap_5c8bf0(char *obj,uint *slot)

{
  int *entry;
  uint value;
  
  entry = (int *)(*(char **)(obj + 0x1a4) + *(int *)(obj + 0x1ac) * 0x14);
  if (slot[1] == 1) {
    value = *(uint *)(*entry + 4);
  }
  else {
    if (slot[1] != 2) {
      slot[1] = 3;
      *slot = (uint)(*entry - *(int *)(*(char **)(obj + 0x174) + 0x10)) >> 5;
      goto LAB_005c8c43;
    }
    value = *(uint *)*entry;
  }
  *slot = value;
LAB_005c8c43:
  slot[3] = 2;
  value = gap_5c3020(obj + 0x5c,entry,0);
  slot[2] = value;
  return;
}


