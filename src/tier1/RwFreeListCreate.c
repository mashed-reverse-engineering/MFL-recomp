/* compile: /G5 */
#include "mfl_tier2.h"
/*004e90e0*/ extern void *__cdecl FreeListCreate(int a, int b, int c, int d, int e, int f);
/* Creates a free list (entry size, entries per block, alignment, flags) through the generic constructor with preallocation off. */
void *RwFreeListCreate(int a, int b, int c, int d)
{
  return FreeListCreate(a, b, c, 1, 0, d);
}
