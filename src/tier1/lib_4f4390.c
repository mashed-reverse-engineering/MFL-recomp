/* compile: /G5 */
#include "mfl_tier2.h"
/*00501d30*/ extern int __cdecl rpWorldSectorNativeRead(void *a, void *b);
/* Native read of a world sector; returns the stream or NULL. */
void *lib_4f4390(void *a, void *b, void *c)
{
  return rpWorldSectorNativeRead(a, c) ? a : 0;
}
