/* compile: /G5 */
#include "mfl_tier2.h"
#define FLD(p, T, off) (*(T *)((char *)(p) + (off)))
/* Calls virtual method 4 (+0x10) of the object's vtable at +0x5c. (Rwp library, proposed name rwp_sphere_0056c340)
   Returns the method's result; rwp_trilist_00574160 0x57417a tests it.
   VOID_w16: cl 13 schedules the tail call differently when the result is returned, so the byte-matching cl 13 build keeps
   the void form (the callee's EAX passes through, as in the original); every other compiler gets the real return. */
#if defined(_MSC_VER) && _MSC_VER < 1310
typedef void (__cdecl *V1)(void *);
void FUN_0056c340(void *o)
{
  (*(V1 *)(FLD(o, char *, 0x5c) + 0x10))(o);
}
#else
typedef int (__cdecl *V1)(void *);
int FUN_0056c340(void *o)
{
  return (*(V1 *)(FLD(o, char *, 0x5c) + 0x10))(o);
}
#endif
