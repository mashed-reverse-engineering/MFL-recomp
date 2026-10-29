/* compile: /G5 */
#include "mfl_tier2.h"
/*00576f80*/ extern int __cdecl FUN_00576f80(void *a, int b);
/* Forwards a value to FUN_00576f80 on the object's first member; returns the object. (Rwp library, proposed name rwp_volume_0056b510) */
void **FUN_0056b510(void **a, int v)
{
  FUN_00576f80(*a, v);
  return a;
}
