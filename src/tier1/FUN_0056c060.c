/* compile: /G5 */
#include "mfl_tier2.h"
/*00575b80*/ extern int __cdecl FUN_00575b80(void *a);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls virtual method 1 of the object, then FUN_00575b80; returns TRUE. (Rwp library, proposed name rwp_sphere_0056c060) */
int FUN_0056c060(void *o)
{
  (*(void (__cdecl **)(void *))(FLD(o, int, 0x5c) + 4))(o);
  FUN_00575b80(o);
  return 1;
}
