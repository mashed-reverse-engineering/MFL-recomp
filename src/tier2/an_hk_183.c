#include "mfl_tier2.h"
extern char PTR_g_data_720043_0064bed0[];
/* FUN_005cfec0 */

/* Resets the state fields of the object at obj to zero (inferred: object
 * initializer). Also stores the global vtable-like pointer
 * PTR_g_data_720043_0064bed0 at obj + 0x14. Returns nothing. */
void __cdecl FUN_005cfec0(char *obj)

{
  /* zero the scalar state fields (offsets not yet mapped to members) */
  *(undefined4 *)(obj + 0x10) = 0;
  *(undefined2 *)(obj + 0x18) = 0;
  *(undefined4 *)(obj + 0x118) = 0;
  *(undefined4 *)(obj + 0x120) = 0;
  *(undefined4 *)(obj + 0x11c) = 0;
  *(undefined4 *)(obj + 0x124) = 0;
  *(undefined4 *)(obj + 0x128) = 0;
  *(undefined ***)(obj + 0x14) = &PTR_g_data_720043_0064bed0;
  *(undefined4 *)(obj + 0x148) = 0;
  *(undefined4 *)(obj + 0x14c) = 0;
  *(undefined4 *)(obj + 0x150) = 0;
  return;
}


