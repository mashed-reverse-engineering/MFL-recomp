/* compile: /G5 */
#include "mfl_tier2.h"
/*00501380*/ extern int __cdecl rpGeometryNativeWrite(void *a, void *b);
/* Native write of a geometry (forwards stream and geometry). */
int lib_4f4320(void *a, void *b, void *c)
{
  return rpGeometryNativeWrite(a, c);
}
