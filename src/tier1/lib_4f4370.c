/* compile: /G5 */
#include "mfl_tier2.h"
/*00501aa0*/ extern int __cdecl rpWorldSectorNativeWrite(void *a, void *b);
/* Native write of a world sector (forwards stream and sector). */
int lib_4f4370(void *a, void *b, void *c)
{
  return rpWorldSectorNativeWrite(a, c);
}
