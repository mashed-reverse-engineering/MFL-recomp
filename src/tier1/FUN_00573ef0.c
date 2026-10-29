/* compile: /G5 */
#include "mfl_tier2.h"
/*00573a00*/ extern void __cdecl FUN_00573a00(void *p);
/* Calls FUN_00573a00 on the object (Rwp data destructor); returns the object. */
void *FUN_00573ef0(void *t)
{
  FUN_00573a00(t);
  return t;
}
