/* compile: /G5 */
#include "mfl_tier2.h"
/*005f6450*/ extern char g_data_5f6450[];
/*0056c950*/ extern void __cdecl FUN_0056c950(void *a, void *b);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Rwp sphere constructor: base constructor with the sphere vtable, stores the radius at +0x4c. (Rwp library, proposed name rwp_sphere_0056cb10) */
void *FUN_0056cb10(void *o, int v)
{
  FUN_0056c950(o, g_data_5f6450);
  FLD(o, int, 0x4c) = v;
  return o;
}
