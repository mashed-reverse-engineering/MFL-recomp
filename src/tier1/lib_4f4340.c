/* compile: /G5 */
#include "mfl_tier2.h"
/*00501600*/ extern int __cdecl rpGeometryNativeRead(void *a, void *b);
/* Native read of a geometry; returns the stream or NULL. */
void *lib_4f4340(void *a, void *b, void *c)
{
  return rpGeometryNativeRead(a, c) ? a : 0;
}
