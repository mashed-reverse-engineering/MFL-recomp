#include "mfl_tier2.h"
/* FUN_005cc300 */

/* Stores `value` in obj+0x74 and sets flag bit 0x100 in obj+0x78 (inferred: a "dirty/updated" flag).
   If bit 0x08 of obj+0x78 is also set, `value` is copied to the sub-object at obj+0x11c, field +0x38 (inferred).
   obj: object pointer (type unknown, inferred from offsets only).
   value: 32-bit value to store (meaning unknown).
   Returns nothing. */
void FUN_005cc300(char *obj,undefined4 value)

{
  /* store the value in the object */
  *(undefined4 *)(obj + 0x74) = value;
  /* set the update flag (inferred) */
  *(uint *)(obj + 0x78) = *(uint *)(obj + 0x78) | 0x100;
  /* if bit 0x08 is set, also forward the value to the sub-object (inferred) */
  if ((*(byte *)(obj + 0x78) & 8) != 0) {
    *(undefined4 *)(*(char **)(obj + 0x11c) + 0x38) = value;
  }
  return;
}


