/* compile: /G5 */
#include "mfl_tier2.h"
/*005f6890*/ extern char g_data_5f6890[];
/*0056c950*/ extern void __cdecl FUN_0056c950(void *o, void *vtbl);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Rwp capsule constructor: base constructor with the capsule vtable, clears two fields and stores two values. (Rwp library, proposed name rwp_capsule_005749c0) */
void *FUN_005749c0(void *o, int a, int b)
{
  FUN_0056c950(o, g_data_5f6890);
  FLD(o, int, 0x48) = 0;
  FLD(o, int, 0x44) = 0;
  FLD(o, int, 0x40) = b;
  FLD(o, int, 0x4c) = a;
  return o;
}
