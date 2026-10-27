#include "mfl_tier2.h"
/* layout of *obj in FUN_005cfec0, reconstructed from the byte offsets it is accessed at (explicit padding: the offsets are the original ones, every field sits at its natural alignment; fields are named by offset) */
typedef struct FUN_005cfec0_obj_layout {
    char pad_0x0[16];
    undefined4 f_0x10;
    undefined ** f_0x14;
    undefined2 f_0x18;
    char pad_0x1a[254];
    undefined4 f_0x118;
    undefined4 f_0x11c;
    undefined4 f_0x120;
    undefined4 f_0x124;
    undefined4 f_0x128;
    char pad_0x12c[28];
    undefined4 f_0x148;
    undefined4 f_0x14c;
    undefined4 f_0x150;
} FUN_005cfec0_obj_layout;

extern char PTR_g_data_720043_0064bed0[];
/* FUN_005cfec0 */
/* Resets the state fields of the object at obj to zero (inferred: object
 * initializer). Also stores the global vtable-like pointer
 * PTR_g_data_720043_0064bed0 at obj + 0x14. Returns nothing. */
void __cdecl FUN_005cfec0(FUN_005cfec0_obj_layout *obj)
{
  /* zero the scalar state fields (offsets not yet mapped to members) */
  obj->f_0x10 = 0;
  obj->f_0x18 = 0;
  obj->f_0x118 = 0;
  obj->f_0x120 = 0;
  obj->f_0x11c = 0;
  obj->f_0x124 = 0;
  obj->f_0x128 = 0;
  obj->f_0x14 = &PTR_g_data_720043_0064bed0;
  obj->f_0x148 = 0;
  obj->f_0x14c = 0;
  obj->f_0x150 = 0;
  return;
}
