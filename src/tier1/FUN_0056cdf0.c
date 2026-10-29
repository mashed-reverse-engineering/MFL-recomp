/* compile: /G5 */
#include "mfl_tier2.h"
/*005f64e0*/ extern char g_data_5f64e0[];
/*0056c950*/ extern void __cdecl FUN_0056c950(void *o, void *vtbl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Rwp box constructor: base constructor with the box vtable, copies the three half-extents to +0x40..+0x48. (Rwp library, proposed name rwp_box_0056cdf0) */
void *FUN_0056cdf0(void *o, float *size)
{
  FUN_0056c950(o, g_data_5f64e0);
  FLD(o, float, 0x40) = size[0];
  FLD(o, float, 0x44) = size[1];
  FLD(o, float, 0x48) = size[2];
  return o;
}
