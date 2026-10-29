/* compile: /G5 */
#include "mfl_tier2.h"
typedef void (__cdecl *V1)(void *);
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls virtual method 4 (+0x10) of the object's vtable at +0x5c. (Rwp library, proposed name rwp_sphere_0056c340) */
void FUN_0056c340(void *o)
{
  (*(V1 *)(FLD(o, char *, 0x5c) + 0x10))(o);
}
